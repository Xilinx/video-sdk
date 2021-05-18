/*
 * Copyright (C) 2021, Xilinx Inc - All rights reserved
 * Xilinx SDAccel Media Accelerator API
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

#include "xvbm.h"
#include "xlnx_transcoder.h"

/*-----------------------------------------------------------------------------
xlnx_tran_update_num_channels: Update num channels for the current session.

Parameters:
transcode_ctx: Transcoder context

-----------------------------------------------------------------------------*/
static void xlnx_tran_update_num_channels(XlnxTranscoderCtx *transcode_ctx)
{

    if(transcode_ctx->curr_sess_channels != transcode_ctx->num_enc_channels) {
        transcode_ctx->curr_sess_channels = transcode_ctx->num_enc_channels;
    }
    else {
        transcode_ctx->curr_sess_channels = transcode_ctx->num_scal_fullrate + 
            (transcode_ctx->num_enc_channels - transcode_ctx->num_scal_out);
    }

    return;
}

/*-----------------------------------------------------------------------------
xlnx_tran_xvbm_buf_inc: Increments the decoder output buffer reference count

Parameters:
transcode_ctx: Transcoder context

-----------------------------------------------------------------------------*/
static void xlnx_tran_xvbm_buf_inc(XlnxTranscoderCtx *transcode_ctx) 
{
    XmaFrame *dec_out = transcode_ctx->dec_ctx.out_frame;
    if (dec_out->data[0].buffer && 
                        transcode_ctx->non_scal_channels) {
        /* Encoder and scaler will use this buffer */
        if(transcode_ctx->num_scal_out)
            xvbm_buffer_refcnt_inc(dec_out->data[0].buffer);

        transcode_ctx->dec_idx_arr[
            transcode_ctx->dec_out_index%DEC_MAX_OUT_BUFFERS] =
                xvbm_buffer_get_id(dec_out->data[0].buffer);
        transcode_ctx->dec_pool_handle = 
            xvbm_get_pool_handle(dec_out->data[0].buffer);
        transcode_ctx->dec_out_index++;
    }
}

/*-----------------------------------------------------------------------------
xlnx_tran_get_la_input: Gets lookahead input frame pointer

Parameters:
transcode_ctx: Transcoder context

Return:
Lookahead input frame pointer
-----------------------------------------------------------------------------*/
static XmaFrame* xlnx_tran_get_la_input(XlnxTranscoderCtx *transcode_ctx)
{
    XlnxEncoderCtx *enc_ctx = 
                    &transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx];
    XmaFrame* la_in_frame = enc_ctx->la_in_frame;

    if((transcode_ctx->enc_chan_idx == 0) && 
                               transcode_ctx->non_scal_channels) {
        la_in_frame->data[0].buffer = xvbm_get_buffer_handle(
                transcode_ctx->dec_pool_handle, transcode_ctx->dec_idx_arr[
                transcode_ctx->la_in_index%DEC_MAX_OUT_BUFFERS]);
        transcode_ctx->la_in_index++;
    }
    return la_in_frame;
}

/*-----------------------------------------------------------------------------
xlnx_tran_xrm_init: Initializes XRM Transcoder structure.

Parameters:
transcode_ctx: Transcoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static void xlnx_tran_xrm_init(XlnxTranscoderCtx *transcode_ctx)
{

    XlnxTranscoderXrmCtx *app_xrm_ctx = &transcode_ctx->app_xrm_ctx;

    app_xrm_ctx->num_scal_out = transcode_ctx->num_scal_out;
    app_xrm_ctx->num_enc_channels = transcode_ctx->num_enc_channels;
    app_xrm_ctx->la_enable = 
           (transcode_ctx->enc_ctx[0].enc_props.lookahead_depth > 0) ? 1 : 0;

    app_xrm_ctx->num_fullrate_out = 0;
    if(transcode_ctx->scal_ctx.num_sessions > 1) {
        app_xrm_ctx->num_fullrate_out = 
            transcode_ctx->scal_ctx.session_nb_outputs[SCAL_SESSION_FULL_RATE];
    }
}

/*-----------------------------------------------------------------------------
xlnx_tran_device_init: Initializes XRM Transcoder structure and does XMA 
                       initialization.

Parameters:
transcode_ctx: Transcoder context
transcode_props: Transcoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_tran_device_init(XlnxTranscoderCtx *transcode_ctx, 
                              XlnxTranscoderProperties *transcode_props)
{

    xlnx_tran_xrm_init(transcode_ctx);
    return xlnx_tran_xma_init(&transcode_ctx->app_xrm_ctx, 
           &transcode_props->xma_dec_props, &transcode_props->xma_scal_props, 
           &transcode_props->xma_enc_props[0]);
}

/*-----------------------------------------------------------------------------
xlnx_tran_update_state: Outputs transcoder next state in state machine

Parameters:
transcode_ctx: Transcoder context
ret: return value from XMA fucntions

-----------------------------------------------------------------------------*/
static void xlnx_tran_update_state(XlnxTranscoderCtx *transcode_ctx, 
        int32_t ret)
{

    int32_t tr_state = transcode_ctx->transcoder_state;
    int32_t out_state = -1;

    switch(tr_state) {

        case TRANSCODE_DEC_READ_FRAME:
            out_state = TRANSCODE_DEC_SEND_INPUT;
            /* If EOF reaches or number of frames to be transcoded is read, 
               initiate flush */
            if (ret == DEC_INPUT_EOF) {
                if (transcode_ctx->loop_count-- > 0) {
                    lseek (transcode_ctx->dec_ctx.in_file, 0, SEEK_SET);
                }
                else
                    out_state = TRANSCODE_DEC_SEND_LAST_FRAME;
            }
            break;

        case TRANSCODE_DEC_SEND_INPUT:
        case TRANSCODE_DEC_SEND_LAST_FRAME:
            if((tr_state == TRANSCODE_DEC_SEND_LAST_FRAME) && 
                !(xlnx_dec_get_input_size(&transcode_ctx->dec_ctx))) {
                /* If the state is TRANSCODE_DEC_SEND_LAST_FRAME and the 
                   decoder input buffer is completely sent, trigger EOF */
                out_state = TRANSCODE_EOF;
            }
            else {
                out_state = TRANSCODE_DEC_GET_OUTPUT;
            }
            if(transcode_ctx->in_frame_cnt >= transcode_ctx->num_frames) {
                out_state = TRANSCODE_EOF;
            }
            break;

        case TRANSCODE_DEC_GET_OUTPUT:
        case TRANSCODE_DEC_FLUSH:
            if(ret == XMA_EOS) {
                if(transcode_ctx->num_scal_out) {
                    out_state = TRANSCODE_SCAL_FLUSH;
                    transcode_ctx->flush_mode = TRANSCODE_SCAL_FLUSH_MODE;
                }
                else {
                    out_state = TRANSCODE_LA_FLUSH;
                    transcode_ctx->flush_mode = TRANSCODE_LA_FLUSH_MODE;
                }
            }
            else if(ret == XMA_TRY_AGAIN) {
                out_state = TRANSCODE_DEC_READ_FRAME;
            }
            else {
                /* Go to scaler logic, only if scaler channels are present */
                if(transcode_ctx->num_scal_out)
                    out_state = TRANSCODE_SCAL_PROCESS_FRAME;
                else
                    out_state = TRANSCODE_LA_PROCESS;
            }
            break;

        case TRANSCODE_SCAL_PROCESS_FRAME:
        case TRANSCODE_SCAL_FLUSH:
            if (ret == XMA_SUCCESS) {
                out_state = TRANSCODE_LA_PROCESS;
            }
            else if(ret == XMA_EOS) {
                out_state = TRANSCODE_LA_FLUSH;
                transcode_ctx->curr_sess_channels = 
                                transcode_ctx->num_enc_channels;
                transcode_ctx->flush_mode = TRANSCODE_LA_FLUSH_MODE;
            }
            else if((ret == XMA_SEND_MORE_DATA) && 
                   (tr_state != TRANSCODE_SCAL_FLUSH)) {
                /* This happens only when scaler pipeline is enabled */
                out_state = TRANSCODE_DEC_READ_FRAME;
            }
           break;

        case TRANSCODE_LA_PROCESS:
        case TRANSCODE_LA_FLUSH:
            if(ret == TRANSCODE_APP_SUCCESS) {
                out_state = TRANSCODE_ENC_PROCESS_FRAME;
            }
            else if (ret == XMA_SEND_MORE_DATA) {
                /* Loop through all the channels of encoder */
                transcode_ctx->enc_chan_idx++;
                if(transcode_ctx->enc_chan_idx < 
                    transcode_ctx->curr_sess_channels) {
                    out_state = TRANSCODE_LA_PROCESS;
                } else {
                    out_state = TRANSCODE_DEC_READ_FRAME;
                }
            }
            else if(ret == XMA_EOS) {
                out_state = TRANSCODE_ENC_NULL_FRAME;
            }

            if((tr_state == TRANSCODE_LA_FLUSH) && 
            (transcode_ctx->enc_ctx[transcode_ctx->enc_chan_idx].la_bypass)) {
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
                    out_state = TRANSCODE_DEC_READ_FRAME;
                }
            }
            else if(tr_state == TRANSCODE_ENC_FLUSH) {

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
                        transcode_ctx->curr_sess_channels){
                        transcode_ctx->enc_chan_idx = 0;
                    }
                }
            }
            else { /* TRANSCODE_ENC_NULL_FRAME state */
                transcode_ctx->enc_chan_idx++;
                if(transcode_ctx->enc_chan_idx < 
                    transcode_ctx->curr_sess_channels) {
                    out_state = TRANSCODE_LA_FLUSH;
                }
                else {
                    transcode_ctx->enc_chan_idx = 0;
                    out_state = TRANSCODE_ENC_FLUSH;
                    transcode_ctx->flush_mode = TRANSCODE_ENC_FLUSH_MODE;
                }
            }
            break;

        default:
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Error unsupported transcoder state \n");
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

/*-----------------------------------------------------------------------------
xlnx_tran_session_create: Creates transcoder session

Parameters:
transcode_ctx: Transcoder context
transcode_props: Transcoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_tran_session_create(XlnxTranscoderCtx *transcode_ctx, 
        XlnxTranscoderProperties *transcode_props)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    ret = xlnx_dec_session(&transcode_ctx->app_xrm_ctx, 
          &transcode_ctx->dec_ctx, &transcode_props->xma_dec_props);
    if(ret != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error in decoder session create \n");
        return TRANSCODE_APP_FAILURE;
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Decoder session creation successful \n");

    if(transcode_ctx->num_scal_out) {
        ret = xlnx_scal_session(&transcode_ctx->app_xrm_ctx, 
                &transcode_ctx->scal_ctx, &transcode_props->xma_scal_props);

        if(ret != TRANSCODE_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Error in scaler session create \n");
            return TRANSCODE_APP_FAILURE;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Scaler session creation successful \n");
    }

    for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {
        ret = xlnx_enc_session(&transcode_ctx->app_xrm_ctx, 
            &transcode_ctx->enc_ctx[i], &transcode_props->xma_enc_props[i],
            &transcode_props->xma_la_props[i]);
        if(ret != TRANSCODE_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Error in encoder session create \n");
            return TRANSCODE_APP_FAILURE;
        }
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Encoder session creation successful \n");

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_tran_frame_process: Process a frame through a state in transcoder pipeline

Parameters:
transcode_ctx: Transcoder context
transcode_stop: Transcoder stop flag

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_tran_frame_process(XlnxTranscoderCtx *transcode_ctx, 
        int32_t *transcode_stop)
{

    int32_t ret = TRANSCODE_APP_FAILURE;
    int32_t enc_out_size = 0;

    int32_t chan_idx;
    int32_t enc_null_frame = 0;
    XmaFrame* la_out_frame;
    XmaFrame* la_in_frame;

    XlnxDecoderCtx *dec_ctx = &transcode_ctx->dec_ctx;
    XlnxScalerCtx *scal_ctx = &transcode_ctx->scal_ctx;
    XlnxEncoderCtx *enc_ctx = &transcode_ctx->enc_ctx[0];

    if(*transcode_stop) {
        transcode_ctx->transcoder_state = TRANSCODE_STOP;
        *transcode_stop = 0;
    }

    switch(transcode_ctx->transcoder_state) {

        case TRANSCODE_DEC_READ_FRAME:
            if((ret = xlnx_dec_read_frame(dec_ctx)) == TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in reading input frame \n");
                return TRANSCODE_STOP;
            }
            else {
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            break;

        case TRANSCODE_DEC_SEND_INPUT:
        case TRANSCODE_DEC_SEND_LAST_FRAME:
            if((ret = xlnx_dec_send_frame(dec_ctx)) <= TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in sending frame to decoder \n");
                return TRANSCODE_STOP;
            }
            else {
                /* Input frame has been incremented only if the decoder 
                   consumes the frame that is read. Read frame doesn't 
                   necessarily read one frame at a time*/
                if(ret == TRANSCODE_APP_SUCCESS) {
                    transcode_ctx->in_frame_cnt++;
                }
                xlnx_tran_update_state(transcode_ctx, ret);
            }
            break;

        case TRANSCODE_DEC_GET_OUTPUT:
        case TRANSCODE_DEC_FLUSH:
            if(transcode_ctx->transcoder_state == TRANSCODE_DEC_FLUSH) {
                /* send null output buffer for decoding to continue on device */
                if((ret = xlnx_dec_send_null_frame(dec_ctx)) <= 
                    TRANSCODE_APP_FAILURE) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                            "Error sending null frame to the decoder \n");
                    return TRANSCODE_STOP;
                }
            }

            if((ret = xlnx_dec_recv_frame(dec_ctx)) <= TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in getting decoder output \n");
                return TRANSCODE_STOP;
            }
            else {
                xlnx_tran_update_state(transcode_ctx, ret);
                /* The decoder output buffer will be used by scaler and 
                   encoder, so incrementing the buffer reference count.*/
                if(ret == TRANSCODE_APP_SUCCESS) {
                    xlnx_tran_xvbm_buf_inc(transcode_ctx);
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

            if((ret = xlnx_scal_process_frame (scal_ctx)) <= TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Failed in scaler frame process \n");
                return TRANSCODE_STOP;
            }
            else {
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
            la_in_frame = NULL;

            if(transcode_ctx->transcoder_state != TRANSCODE_LA_FLUSH) {
                la_in_frame = xlnx_tran_get_la_input(transcode_ctx);
            }

            if((ret = xlnx_la_send_recv_frame(
                    &enc_ctx[transcode_ctx->enc_chan_idx].la_ctx, la_in_frame, 
                    &la_out_frame)) <= TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "xlnx_la_send_recv_frame failed with error %d \n", ret);
                return TRANSCODE_STOP;
            }
            else {
                if(la_out_frame != NULL)
                    enc_ctx[transcode_ctx->enc_chan_idx].enc_in_frame = 
                                                              la_out_frame;
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
                      &enc_out_size)) <= TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Failed in encoder frame process \n");
                return TRANSCODE_STOP;
            }
            else {
                xlnx_tran_update_state(transcode_ctx, ret);
            }

            if((ret == TRANSCODE_APP_SUCCESS) && enc_out_size) {
                ret = write (enc_ctx[chan_idx].out_file, 
                    enc_ctx[chan_idx].xma_buffer->data.buffer, enc_out_size);
                if(!chan_idx) {
                    transcode_ctx->out_frame_cnt++;
                }
                enc_out_size = 0;
            }
            break;

        case TRANSCODE_EOF:
            if (xlnx_dec_send_flush_frame(dec_ctx) <= TRANSCODE_APP_FAILURE)
            {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error sending end of file signal to decoder\n");
                return TRANSCODE_STOP;
            }
            else {
                transcode_ctx->transcoder_state = TRANSCODE_DEC_FLUSH;
                transcode_ctx->flush_mode = TRANSCODE_DEC_FLUSH_MODE;
            }
            break;

        case TRANSCODE_STOP:
            /* pretend we reached the end of the source and prepare to quit */
            transcode_ctx->loop_count = 0;
            lseek (dec_ctx->in_file, 0, SEEK_END);
            transcode_ctx->transcoder_state = TRANSCODE_EOF;
            break;

        default:
            break;
    }

    if(transcode_ctx->transcoder_state == TRANSCODE_DONE) {
        return TRANSCODE_STOP;
    }
    else {
        return TRANSCODE_APP_SUCCESS;
    }

}

