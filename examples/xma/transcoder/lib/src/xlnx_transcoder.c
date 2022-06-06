/*
 * Copyright (C) 2021, Xilinx Inc - All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include "xlnx_transcoder.h"
#include "xvbm.h"

/**
 * xlnx_tran_update_num_channels: Update num channels for the current session.
 *
 * @param transcode_ctx: Transcoder context
 */
static void xlnx_tran_update_num_channels(XlnxTranscoderCtx* transcode_ctx)
{

    if(transcode_ctx->curr_sess_channels != transcode_ctx->num_enc_channels) {
        transcode_ctx->curr_sess_channels = transcode_ctx->num_enc_channels;
    } else {
        transcode_ctx->curr_sess_channels =
            transcode_ctx->num_scal_fullrate +
            (transcode_ctx->num_enc_channels - transcode_ctx->num_scal_out);
    }

    return;
}

/**
 * xlnx_tran_xvbm_buf_inc: Increments the decoder output buffer reference
 *                         count and copies the decoder xma frame.
 *
 * @param transcode_ctx: Transcoder context
 */
static void xlnx_tran_xvbm_buf_inc(XlnxTranscoderCtx* transcode_ctx)
{
    XmaFrame* dec_out = transcode_ctx->dec_ctx.out_frame;
    XmaFrame* enc_in  = transcode_ctx->enc_ctx[0].in_frame;
    if(dec_out->data[0].buffer && transcode_ctx->non_scal_channels) {
        /* Encoder and scaler will use this buffer */
        if(transcode_ctx->num_scal_out)
            xvbm_buffer_refcnt_inc(dec_out->data[0].buffer);

        transcode_ctx
            ->dec_idx_arr[transcode_ctx->dec_out_index % DEC_MAX_OUT_BUFFERS] =
            xvbm_buffer_get_id(dec_out->data[0].buffer);
        transcode_ctx->dec_pool_handle =
            xvbm_get_pool_handle(dec_out->data[0].buffer);
        transcode_ctx->dec_out_index++;

        XmaSideDataHandle* side_data = enc_in->side_data;
        memcpy(enc_in, dec_out, sizeof(XmaFrame));
        enc_in->side_data = side_data;
    }
}

/**
 * xlnx_tran_hdr_refcnt_incr: Checks for HDR data present in decoder output
 frame
 * and increments the HDR side data reference count if it
 * will be sent to the encoder and scaler.
 *
 * @param transcode_ctx: Transcoder context
 */
static void xlnx_tran_hdr_refcnt_incr(XlnxTranscoderCtx* transcode_ctx)
{
    XmaFrame* dec_out = transcode_ctx->dec_ctx.out_frame;
    XmaFrame* enc_in  = transcode_ctx->enc_ctx[0].in_frame;
    if(dec_out->data[0].buffer && transcode_ctx->non_scal_channels &&
       transcode_ctx->num_scal_out) {

        /* Add HDR side data from decoder output to the encoder input xma
           frame */
        XmaSideDataHandle hdr_sd =
            xma_frame_get_side_data(dec_out, XMA_FRAME_HDR);
        if(hdr_sd) {
            xma_frame_add_side_data(enc_in, hdr_sd);
        }
    }
}

/**
 * Get the lookahead input frame pointer, convert data to host if necessary
 * @param transcode_ctx The transcode ctx
 * @return The lookahead input frame pointer
 */
static XmaFrame* xlnx_tran_get_la_input(XlnxTranscoderCtx* transcode_ctx)
{
    XlnxEncoderCtx* enc_ctx =
        &transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx];
    XmaFrame* la_in_frame = enc_ctx->in_frame;
    if((transcode_ctx->enc_chan_idx == 0) && transcode_ctx->non_scal_channels) {
        la_in_frame->data[0].buffer = xvbm_get_buffer_handle(
            transcode_ctx->dec_pool_handle,
            transcode_ctx->dec_idx_arr[transcode_ctx->la_in_index %
                                       DEC_MAX_OUT_BUFFERS]);
        transcode_ctx->la_in_index++;
    }
    if(!enc_ctx->convert_input) {
        return la_in_frame;
    }
    if(la_in_frame->data[0].buffer != NULL) {
        if(xlnx_utils_copy_dev_frame_to_host_frame(
               la_in_frame, enc_ctx->send_frame) != XMA_APP_SUCCESS) {
            return NULL;
        }
        /* This will check/decrement the ref cnt before freeing. */
        xvbm_buffer_pool_entry_free(la_in_frame->data[0].buffer);
    }
    return enc_ctx->send_frame;
}

/**
 * xlnx_enc_set_if_idr_frame: Set frames as IDR frames at run time
 *
 * @param enc_ctx: Encoder context
 * @param input_xframe: Lookahead input frame
 * @param enc_frame_cnt: Encoder frame number
 */
static void xlnx_enc_set_if_idr_frame(XlnxEncoderCtx* enc_ctx,
                                      XmaFrame*       input_xframe,
                                      uint32_t        enc_frame_cnt)
{
    uint32_t* idr_arr     = enc_ctx->dynamic_idr.idr_arr;
    size_t    len_idr_arr = enc_ctx->dynamic_idr.len_idr_arr;
    uint32_t  i           = enc_ctx->dynamic_idr.idr_arr_idx;

    input_xframe->is_idr = 0;
    for(; i < len_idr_arr; i++) {
        if(idr_arr[i] == enc_frame_cnt) {
            input_xframe->is_idr = 1;
            break;
        } else if(idr_arr[i] > enc_frame_cnt) {
            break;
        }
    }

    enc_ctx->dynamic_idr.idr_arr_idx = i;
}

/**
 * xlnx_get_runtime_dyn_params: Gets encoder runtime dynamic parameters
 *
 * @param transcode_ctx: Transcoder context
 * @param la_in_frame: Lookahead input frame
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_get_runtime_dyn_params(XlnxTranscoderCtx* transcode_ctx,
                                           XmaFrame*          la_in_frame)
{
    XlnxEncoderCtx* enc_ctx =
        &transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx];
    EncDynParams* enc_dyn_params =
        &transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx].enc_dyn_params;

    uint32_t enc_frame_cnt;
    if(enc_ctx->enc_props.lookahead_depth > 0) {
        enc_frame_cnt = enc_ctx->la_ctx.num_frames_sent;
    } else {
        enc_frame_cnt = enc_ctx->num_frames_sent;
    }

    /* Check for Dynamic IDR option */
    if(enc_ctx->dynamic_idr.len_idr_arr) {
        xlnx_enc_set_if_idr_frame(enc_ctx, la_in_frame, enc_frame_cnt);
    }

    /* Check if dynamic encoder parameters are present and add them as
    frame side data */
    if((enc_dyn_params->dynamic_params_count > 0) &&
       (enc_dyn_params->dynamic_params_index <
        enc_dyn_params->dynamic_params_count)) {
        uint32_t dyn_frame_num = (*(
            enc_dyn_params->dyn_params_obj.xlnx_enc_get_dyn_param_frame_num))(
            enc_dyn_params->dynamic_param_handle,
            enc_dyn_params->dynamic_params_index);

        if(dyn_frame_num == (enc_frame_cnt)) {
            uint32_t num_b_frames = (*(
                enc_dyn_params->dyn_params_obj.xlnx_enc_get_runtime_b_frames))(
                enc_dyn_params->dynamic_param_handle,
                enc_dyn_params->dynamic_params_index);
            /* Dynamic b-frames have to be less than or equal to number
            of B-frames specified on the command line or default value,
            whichever is set at the beginning of encoding */
            if(num_b_frames > enc_ctx->enc_props.num_bframes) {
                xma_logmsg(
                    XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Dynamic b-frames=%d at frameNum=%d cannot be greater"
                    " than initial number of b-frames=%d\n",
                    num_b_frames, dyn_frame_num,
                    enc_ctx->enc_props.num_bframes);
                return XMA_APP_ERROR;
            }

            /* If tune-metrics is enabled, then reset all the AQ parameters*/
            if(enc_ctx->enc_props.tune_metrics) {
                (*(enc_dyn_params->dyn_params_obj
                       .xlnx_enc_reset_runtime_aq_params))(
                    enc_dyn_params->dynamic_param_handle,
                    enc_dyn_params->dynamic_params_index);
            }

            if((*(enc_dyn_params->dyn_params_obj.xlnx_enc_add_dyn_params))(
                   enc_dyn_params->dynamic_param_handle, la_in_frame,
                   enc_dyn_params->dynamic_params_index) != XMA_APP_SUCCESS) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Failed to add dynamic parameters as side data\n");
                return XMA_APP_ERROR;
            }

            enc_dyn_params->dynamic_params_index++;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_tran_xrm_init: Initializes XRM Transcoder structure.
 *
 * @param transcode_ctx: Transcoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static void xlnx_tran_xrm_init(XlnxTranscoderCtx* transcode_ctx)
{

    XlnxTranscoderXrmCtx* app_xrm_ctx = &transcode_ctx->app_xrm_ctx;

    app_xrm_ctx->num_scal_out     = transcode_ctx->num_scal_out;
    app_xrm_ctx->num_enc_channels = transcode_ctx->num_enc_channels;
    app_xrm_ctx->la_enable =
        (transcode_ctx->enc_ctx[0].enc_props.lookahead_depth > 0) ? 1 : 0;

    app_xrm_ctx->num_fullrate_out = 0;
    if(transcode_ctx->scal_ctx.num_sessions > 1) {
        app_xrm_ctx->num_fullrate_out =
            transcode_ctx->scal_ctx.session_nb_outputs[SCAL_SESSION_FULL_RATE];
    }
}

/**
 * xlnx_tran_device_init: Initializes XRM Transcoder structure and does XMA
 * initialization.
 *
 * @param transcode_ctx: Transcoder context
 * @param transcode_props: Transcoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_tran_device_init(XlnxTranscoderCtx*        transcode_ctx,
                              XlnxTranscoderProperties* transcode_props)
{

    xlnx_tran_xrm_init(transcode_ctx);
    return xlnx_tran_xma_init(&transcode_ctx->app_xrm_ctx, transcode_props);
}

/**
 * xlnx_tran_update_state: Outputs transcoder next state in state machine
 *
 * @param transcode_ctx: Transcoder context
 * @param ret: return value from XMA fucntions

 */
static void xlnx_tran_update_state(XlnxTranscoderCtx* transcode_ctx,
                                   int32_t            ret)
{

    int32_t tr_state  = transcode_ctx->transcoder_state;
    int32_t out_state = -1;

    switch(tr_state) {

        case TRANSCODE_DEC_READ_FRAME:
            out_state = TRANSCODE_DEC_SEND_INPUT;
            /* If EOF reaches or number of frames to be transcoded is read,
               initiate flush */
            if(ret == DEC_INPUT_EOF) {
                if(transcode_ctx->loop_count-- > 0) {
                    lseek(transcode_ctx->dec_ctx.in_file, 0, SEEK_SET);
                } else {
                    out_state = TRANSCODE_DEC_SEND_LAST_FRAME;
                }
            }
            break;

        case TRANSCODE_DEC_SEND_INPUT:
        case TRANSCODE_DEC_SEND_LAST_FRAME:
            if((tr_state == TRANSCODE_DEC_SEND_LAST_FRAME) &&
               !(xlnx_dec_get_input_size(&transcode_ctx->dec_ctx))) {
                /* If the state is TRANSCODE_DEC_SEND_LAST_FRAME and the
                   decoder input buffer is completely sent, trigger EOF */
                out_state = TRANSCODE_EOF;
            } else {
                out_state = TRANSCODE_DEC_GET_OUTPUT;
            }
            if(transcode_ctx->out_frame_cnt >= transcode_ctx->num_frames) {
                out_state = TRANSCODE_EOF;
            }
            break;

        case TRANSCODE_DEC_GET_OUTPUT:
        case TRANSCODE_DEC_FLUSH:
            if(ret == XMA_EOS) {
                if(transcode_ctx->num_scal_out) {
                    out_state                 = TRANSCODE_SCAL_FLUSH;
                    transcode_ctx->flush_mode = TRANSCODE_SCAL_FLUSH_MODE;
                } else {
                    out_state                 = TRANSCODE_LA_FLUSH;
                    transcode_ctx->flush_mode = TRANSCODE_LA_FLUSH_MODE;
                }
            } else if(ret == XMA_TRY_AGAIN) {
                out_state = TRANSCODE_DEC_READ_FRAME;
            } else {
                /* Go to scaler logic, only if scaler channels are present */
                if(transcode_ctx->num_scal_out) {
                    out_state = TRANSCODE_SCAL_PROCESS_FRAME;
                } else {
                    out_state = TRANSCODE_LA_PROCESS;
                }
            }
            break;

        case TRANSCODE_SCAL_PROCESS_FRAME:
        case TRANSCODE_SCAL_FLUSH:
            if(ret == XMA_SUCCESS) {
                out_state = TRANSCODE_LA_PROCESS;
            } else if(ret == XMA_EOS) {
                out_state = TRANSCODE_LA_FLUSH;
                transcode_ctx->curr_sess_channels =
                    transcode_ctx->num_enc_channels;
                transcode_ctx->flush_mode = TRANSCODE_LA_FLUSH_MODE;
            } else if((ret == XMA_SEND_MORE_DATA) &&
                      (tr_state != TRANSCODE_SCAL_FLUSH)) {
                /* This happens only when scaler pipeline is enabled */
                out_state = TRANSCODE_DEC_READ_FRAME;
            }
            break;

        case TRANSCODE_LA_PROCESS:
        case TRANSCODE_LA_FLUSH:
            if(ret == XMA_APP_SUCCESS) {
                out_state = TRANSCODE_ENC_PROCESS_FRAME;
            } else if(ret == XMA_SEND_MORE_DATA) {
                /* Loop through all the channels of encoder */
                transcode_ctx->enc_chan_idx++;
                if(transcode_ctx->enc_chan_idx <
                   transcode_ctx->curr_sess_channels) {
                    out_state = TRANSCODE_LA_PROCESS;
                } else {
                    out_state = TRANSCODE_DEC_READ_FRAME;
                }
            } else if(ret == XMA_EOS) {
                out_state = TRANSCODE_ENC_NULL_FRAME;
            }

            if((tr_state == TRANSCODE_LA_FLUSH) &&
               (transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx]
                    .la_bypass)) {
                out_state = TRANSCODE_ENC_NULL_FRAME;
            }
            break;

        case TRANSCODE_ENC_PROCESS_FRAME:
        case TRANSCODE_ENC_NULL_FRAME:
        case TRANSCODE_ENC_FLUSH:
            if(tr_state == TRANSCODE_ENC_PROCESS_FRAME) {
                transcode_ctx->enc_chan_idx++;
                /* Loop through all the channels of encoder */
                if(transcode_ctx->enc_chan_idx <
                   transcode_ctx->curr_sess_channels) {
                    out_state = TRANSCODE_LA_PROCESS;
                } else {
                    transcode_ctx->enc_chan_idx = 0;
                    out_state                   = TRANSCODE_DEC_READ_FRAME;
                }
            } else if(tr_state == TRANSCODE_ENC_FLUSH) {

                out_state = TRANSCODE_ENC_FLUSH;
                if(ret == XMA_EOS) {
                    /* Perform encoder flush one channel at a time */
                    transcode_ctx->eos_count++;
                    transcode_ctx->enc_chan_idx++;
                    if(transcode_ctx->eos_count >=
                       transcode_ctx->curr_sess_channels) {
                        out_state = TRANSCODE_DONE;
                    }
                    if(transcode_ctx->enc_chan_idx >=
                       transcode_ctx->curr_sess_channels) {
                        transcode_ctx->enc_chan_idx = 0;
                    }
                }
            } else { /* TRANSCODE_ENC_NULL_FRAME state */
                transcode_ctx->enc_chan_idx++;
                if(transcode_ctx->enc_chan_idx <
                   transcode_ctx->curr_sess_channels) {
                    out_state = TRANSCODE_LA_FLUSH;
                } else {
                    transcode_ctx->enc_chan_idx = 0;
                    out_state                   = TRANSCODE_ENC_FLUSH;
                    transcode_ctx->flush_mode   = TRANSCODE_ENC_FLUSH_MODE;
                }
            }
            break;

        default:
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error unsupported transcoder state\n");
            out_state = TRANSCODE_DONE;
            break;
    }

    /* This switch case avoids transcoder flush logic going to normal */
    switch(transcode_ctx->flush_mode) {

        case TRANSCODE_DEC_FLUSH_MODE:
            if(out_state < TRANSCODE_DEC_FLUSH) {
                out_state = TRANSCODE_DEC_FLUSH;
            }
            break;

        case TRANSCODE_SCAL_FLUSH_MODE:
            if(out_state < TRANSCODE_SCAL_FLUSH) {
                out_state = TRANSCODE_SCAL_FLUSH;
            }
            break;

        case TRANSCODE_LA_FLUSH_MODE:
            if(out_state < TRANSCODE_LA_FLUSH) {
                out_state = TRANSCODE_LA_FLUSH;
            }
            break;

        default:
            break;
    }

    transcode_ctx->transcoder_state = out_state;
    return;
}

/**
 * xlnx_tran_session_create: Creates transcoder session
 *
 * @param transcode_ctx: Transcoder context
 * @param transcode_props: Transcoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_tran_session_create(XlnxTranscoderCtx*        transcode_ctx,
                                 XlnxTranscoderProperties* transcode_props)
{

    int32_t ret = XMA_APP_ERROR;

    ret = xlnx_dec_session(&transcode_ctx->app_xrm_ctx, &transcode_ctx->dec_ctx,
                           &transcode_props->xma_dec_props);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error in decoder session create\n");
        return XMA_APP_ERROR;
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE,
               "Decoder session creation successful\n");

    if(transcode_ctx->num_scal_out) {
        ret = xlnx_scal_session(&transcode_ctx->app_xrm_ctx,
                                &transcode_ctx->scal_ctx,
                                &transcode_props->xma_scal_props);

        if(ret != XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error in scaler session create\n");
            return XMA_APP_ERROR;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Scaler session creation successful\n");
    }

    for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {
        ret = xlnx_enc_session(&transcode_ctx->app_xrm_ctx,
                               &transcode_ctx->enc_ctx[i],
                               &transcode_props->xma_enc_props[i],
                               &transcode_props->xma_la_props[i]);
        if(ret != XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error in encoder session create\n");
            return XMA_APP_ERROR;
        }
        XmaDataBuffer* xma_buffer = &transcode_ctx->xma_out_buffer[i];
        /* Allocate enough data to safely recv */
        xma_buffer->alloc_size = (3 * transcode_props->xma_enc_props[i].width *
                                  transcode_props->xma_enc_props[i].height) >>
                                 1;
        if(posix_memalign(&xma_buffer->data.buffer, BUFFER_ALLOC_ALIGN,
                          xma_buffer->alloc_size) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to allocate data buffer for encoder recv!\n");
            return XMA_APP_ERROR;
        }
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE,
               "Encoder session creation successful\n");

    return ret;
}

/**
 * xlnx_tran_frame_process: Process a frame through a state in transcoder
 * pipeline
 *
 * @param transcode_ctx: Transcoder context
 * @param transcode_stop: Transcoder stop flag
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_tran_frame_process(XlnxTranscoderCtx* transcode_ctx,
                                int32_t*           transcode_stop)
{

    int32_t ret          = XMA_APP_ERROR;
    int32_t enc_out_size = 0;

    int32_t   chan_idx;
    int32_t   enc_null_frame = 0;
    XmaFrame* la_out_frame;
    XmaFrame* la_in_frame;

    XlnxDecoderCtx* dec_ctx  = &transcode_ctx->dec_ctx;
    XlnxScalerCtx*  scal_ctx = &transcode_ctx->scal_ctx;
    XlnxEncoderCtx* enc_ctx  = &transcode_ctx->enc_ctx[0];

    if(*transcode_stop) {
        transcode_ctx->transcoder_state = TRANSCODE_STOP;
        *transcode_stop                 = 0;
    }

    switch(transcode_ctx->transcoder_state) {

        case TRANSCODE_DEC_READ_FRAME:
            if((ret = xlnx_dec_read_frame(dec_ctx)) == XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in reading input frame\n");
                return TRANSCODE_STOP;
            } else {
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            break;

        case TRANSCODE_DEC_SEND_INPUT:
        case TRANSCODE_DEC_SEND_LAST_FRAME:
            if((ret = xlnx_dec_send_frame(dec_ctx)) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in sending frame to decoder\n");
                return TRANSCODE_STOP;
            } else {
                /* Input frame has been incremented only if the decoder
                   consumes the frame that is read. Read frame doesn't
                   necessarily read one frame at a time*/
                if(ret == XMA_APP_SUCCESS) {
                    transcode_ctx->in_frame_cnt++;
                }
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            break;

        case TRANSCODE_DEC_GET_OUTPUT:
        case TRANSCODE_DEC_FLUSH:
            if(transcode_ctx->transcoder_state == TRANSCODE_DEC_FLUSH) {
                /* send null output buffer for decoding to continue on device */
                if((ret = xlnx_dec_send_null_frame(dec_ctx)) <= XMA_APP_ERROR) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Error sending null frame to the decoder\n");
                    return TRANSCODE_STOP;
                }
            }

            if((ret = xlnx_dec_recv_frame(dec_ctx)) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in getting decoder output\n");
                return TRANSCODE_STOP;
            } else {
                xlnx_tran_update_state(transcode_ctx, ret);
                /* The decoder output buffer will be used by scaler and
                   encoder, so incrementing the buffer reference count.*/
                if(ret == XMA_APP_SUCCESS) {
                    xlnx_tran_xvbm_buf_inc(transcode_ctx);
                    xlnx_tran_hdr_refcnt_incr(transcode_ctx);
                }
            }
            transcode_ctx->enc_chan_idx = 0;
            break;

        case TRANSCODE_SCAL_PROCESS_FRAME:
        case TRANSCODE_SCAL_FLUSH:
            if(transcode_ctx->transcoder_state == TRANSCODE_SCAL_FLUSH) {
                /* Send Null buffer during scaler flush  */
                scal_ctx->in_frame->data[0].buffer = NULL;
            }

            if((ret = xlnx_scal_process_frame(scal_ctx)) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Failed in scaler frame process\n");
                return TRANSCODE_STOP;
            } else {
                /* This function alternates the channel number for full rate
                   and all rate sessions */
                xlnx_tran_update_num_channels(transcode_ctx);
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            transcode_ctx->enc_chan_idx = 0;
            break;

        case TRANSCODE_LA_PROCESS:
        case TRANSCODE_LA_FLUSH:
            la_out_frame = NULL;
            la_in_frame  = NULL;

            if(transcode_ctx->transcoder_state != TRANSCODE_LA_FLUSH) {
                la_in_frame = xlnx_tran_get_la_input(transcode_ctx);
                if((ret = xlnx_get_runtime_dyn_params(
                        transcode_ctx, la_in_frame)) != XMA_APP_SUCCESS) {
                    xma_logmsg(
                        XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "xlnx_get_runtime_dyn_params failed with error %d\n",
                        ret);
                    return TRANSCODE_STOP;
                }
            }

            if((transcode_ctx->transcoder_state == TRANSCODE_LA_FLUSH) &&
               (transcode_ctx->out_frame_cnt == 0)) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Insufficient number of frames in input file!\n");
                return XMA_APP_ERROR;
            }

            if((ret = xlnx_la_send_recv_frame(
                    &enc_ctx[transcode_ctx->enc_chan_idx].la_ctx, la_in_frame,
                    &la_out_frame)) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "xlnx_la_send_recv_frame failed with error %d\n",
                           ret);
                return TRANSCODE_STOP;
            } else {
                if(la_out_frame != NULL) {
                    enc_ctx[transcode_ctx->enc_chan_idx].send_frame =
                        la_out_frame;
                }
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            break;

        case TRANSCODE_ENC_PROCESS_FRAME:
        case TRANSCODE_ENC_NULL_FRAME:
        case TRANSCODE_ENC_FLUSH:

            chan_idx = transcode_ctx->enc_chan_idx;
            if(transcode_ctx->transcoder_state == TRANSCODE_ENC_NULL_FRAME) {
                enc_null_frame = 1;
            }

            if((ret = xlnx_enc_process_frame(&enc_ctx[chan_idx], enc_null_frame,
                                             &enc_out_size)) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Failed in encoder frame process\n");
                return TRANSCODE_STOP;
            } else {
                xlnx_tran_update_state(transcode_ctx, ret);
            }

            if((ret == XMA_APP_SUCCESS) && enc_out_size &&
               transcode_ctx->out_frame_cnt < transcode_ctx->num_frames) {
                ret = write(enc_ctx[chan_idx].out_file,
                            enc_ctx[chan_idx].xma_buffer->data.buffer,
                            enc_out_size);
                if(!chan_idx) {
                    transcode_ctx->out_frame_cnt++;
                }
                enc_out_size = 0;
            }
            break;

        case TRANSCODE_EOF:
            if(xlnx_dec_send_flush_frame(dec_ctx) <= XMA_APP_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error sending end of file signal to decoder\n");
                return TRANSCODE_STOP;
            } else {
                transcode_ctx->transcoder_state = TRANSCODE_DEC_FLUSH;
                transcode_ctx->flush_mode       = TRANSCODE_DEC_FLUSH_MODE;
            }
            break;

        case TRANSCODE_STOP:
            /* pretend we reached the end of the source and prepare to quit */
            transcode_ctx->loop_count = 0;
            lseek(dec_ctx->in_file, 0, SEEK_END);
            transcode_ctx->transcoder_state = TRANSCODE_EOF;
            break;

        default:
            break;
    }

    if(transcode_ctx->transcoder_state == TRANSCODE_DONE) {
        return TRANSCODE_STOP;
    } else {
        return XMA_APP_SUCCESS;
    }
}
