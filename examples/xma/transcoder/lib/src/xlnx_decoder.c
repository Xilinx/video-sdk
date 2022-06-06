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

#include "xlnx_decoder.h"

static struct option dec_options[] = {
    {FLAG_DEC_DEVICE_ID, required_argument, 0, DEC_DEVICE_ID_ARG},
    {FLAG_DEC_INPUT_FILE, required_argument, 0, DEC_INPUT_FILE_ARG},
    {FLAG_DEC_CODEC_TYPE, required_argument, 0, DEC_CODEC_ID_ARG},
    {FLAG_DEC_LOW_LATENCY, required_argument, 0, DEC_LOW_LATENCY_ARG},
    {FLAG_DEC_LATENCY_LOGGING, required_argument, 0, DEC_LATENCY_LOGGING_ARG},
    {0, 0, 0, 0}};

/**
 * xlnx_dec_update_props: Updates XMA decoder properties and the parameters
 *
 * @param dec_ctx: Decoder context
 * @param xma_dec_props: XMA decoder properties structure
 */
void xlnx_dec_update_props(XlnxDecoderCtx*       dec_ctx,
                           XmaDecoderProperties* xma_dec_props)
{
    XlnxDecoderProperties* dec_props  = &dec_ctx->dec_props;
    XlnxDecFrameData*      frame_data = &dec_ctx->frame_data;

    dec_props->width     = frame_data->width;
    dec_props->height    = frame_data->height;
    dec_props->fps       = (frame_data->fr_num / frame_data->fr_den);
    dec_props->bit_depth = frame_data->luma_bit_depth;

    if(dec_props->codec_type == DECODER_ID_H264) {
        dec_props->profile =
            frame_data
                ->h264_seq_parameter_set
                    [frame_data
                         ->h264_pic_parameter_set[frame_data->current_h264_pps]
                         .seq_param_set_id]
                .profile_idc;
    } else if(dec_props->codec_type == DECODER_ID_HEVC) {
        dec_props->profile =
            frame_data->hevc_seq_parameter_set[frame_data->latest_hevc_sps]
                .profile_idc;
    }

    if(dec_props->codec_type == DECODER_ID_H264) {
        dec_props->level =
            frame_data
                ->h264_seq_parameter_set
                    [frame_data
                         ->h264_pic_parameter_set[frame_data->current_h264_pps]
                         .seq_param_set_id]
                .level_idc;
    } else if(dec_props->codec_type == DECODER_ID_HEVC) {
        dec_props->level =
            frame_data->hevc_seq_parameter_set[frame_data->latest_hevc_sps]
                .level_idc;
    }

    xlnx_dec_get_xma_props(&dec_ctx->dec_props, xma_dec_props);

    return;
}

/**
 * init_parse_data: Initialize the decoder header structure
 *
 * @param dec_frame_data: Decoder frame data
 * @return XMA_APP_SUCCESS on success
 */
static int32_t xlnx_dec_init_parse_data(XlnxDecFrameData* dec_frame_data)
{
    memset(dec_frame_data, 0, sizeof(XlnxDecFrameData));

    int i;
    for(i = 0; i < 32; i++)
        dec_frame_data->h264_seq_parameter_set[i].valid = 0;
    for(i = 0; i < 256; i++)
        dec_frame_data->h264_pic_parameter_set[i].valid = 0;

    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt_bottom = -1;
    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt[0]     = -1;
    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt[1]     = -1;
    dec_frame_data->last_h264_slice_header.frame_num                  = 0;
    dec_frame_data->last_h264_slice_header.idr_pic_id                 = 0;
    dec_frame_data->last_h264_slice_header.pic_order_cnt_lsb          = 0;
    dec_frame_data->last_h264_slice_header.pic_parameter_set_id       = 0;
    dec_frame_data->last_h264_slice_header.field_pic_flag             = 0;
    dec_frame_data->last_h264_slice_header.bottom_field_flag          = 0;
    dec_frame_data->last_h264_slice_header.nal_ref_idc                = 0;
    dec_frame_data->last_h264_slice_header.nal_unit_type              = 0;

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_dec_context_init: Initializes decoder context
 *
 * @param dec_ctx: Decoder context
 */
void xlnx_dec_context_init(XlnxDecoderCtx* dec_ctx)
{

    /* Initialize the decoder parameters to default */
    dec_ctx->dec_props.dev_index             = DEFAULT_DEVICE_ID;
    dec_ctx->dec_props.entropy_buffers_count = 2;
    dec_ctx->dec_props.latency_logging       = 0;
    dec_ctx->dec_props.splitbuff_mode        = 0;
    dec_ctx->dec_props.bit_depth             = 8;
    dec_ctx->dec_props.codec_type            = -1;
    dec_ctx->flush_sent                      = false;
    dec_ctx->dec_props.low_latency           = 0;
    dec_ctx->dec_props.scan_type             = 1;
    dec_ctx->dec_props.chroma_mode           = 420;
    /* always zero copy output */
    dec_ctx->dec_props.zero_copy = 1;

    memset(dec_ctx->out_frame, 0, sizeof(XmaFrame));
    dec_ctx->out_frame->data[0].buffer_type = XMA_DEVICE_BUFFER_TYPE;
    dec_ctx->out_frame->data[0].buffer      = NULL;

    xlnx_dec_init_parse_data(&dec_ctx->frame_data);
    return;
}

/**
 * xlnx_dec_validate_arguments: Validates decoder arguments
 *
 * @param dec_props: Decoder properties to be validated
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t
    xlnx_dec_validate_arguments(const XlnxDecoderProperties* dec_props)
{
    if(dec_props->dev_index < DEFAULT_DEVICE_ID ||
       dec_props->dev_index > XLNX_MAX_DEVICE_COUNT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported device ID %d\n", dec_props->dev_index);
        return XMA_APP_ERROR;
    }
    if((dec_props->low_latency != 0) && (dec_props->low_latency != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid decoder low latency %d\n", dec_props->low_latency);
        return XMA_APP_ERROR;
    }

    if((dec_props->latency_logging != 0) && (dec_props->latency_logging != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid decoder low latency %d\n",
                   dec_props->latency_logging);
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_dec_parse_args: Parses the decode command line arguments
 *
 * @param argc: Argument count
 * @param *argv[]: Arguments
 * @param dec_ctx: Decoder context
 * @param the caller
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_parse_args(int32_t argc, char* argv[], XlnxDecoderCtx* dec_ctx,
                            int32_t param_flag)
{

    int32_t                option_index;
    int32_t                flag      = 0;
    int32_t                ret       = XMA_APP_SUCCESS;
    XlnxDecoderProperties* dec_props = &dec_ctx->dec_props;
    while(flag != DEC_INPUT_FILE_ARG) {
        if(param_flag == 0) {
            flag = getopt_long_only(argc, argv, "", dec_options, &option_index);
            if(flag == -1) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in decoder parameters parsing\n");
                return XMA_APP_ERROR;
            }
        } else {
            flag       = param_flag;
            param_flag = 0;
        }

        switch(flag) {
            case DEC_CODEC_ID_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264"))
                    dec_props->codec_type = DECODER_ID_H264;
                else if(!strcmp(optarg, "mpsoc_vcu_hevc"))
                    dec_ctx->dec_props.codec_type = DECODER_ID_HEVC;
                else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Unsupported decoder codec option %s\n", optarg);
                    return XMA_APP_ERROR;
                }
                break;

            case DEC_DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&dec_props->dev_index, optarg,
                                             FLAG_DEC_DEVICE_ID);
                break;

            case DEC_LOW_LATENCY_ARG:
                ret = xlnx_utils_set_int_arg(&dec_props->low_latency, optarg,
                                             FLAG_DEC_LOW_LATENCY);
                break;

            case DEC_INPUT_FILE_ARG:
                dec_ctx->in_file = open(optarg, O_RDONLY);
                if(dec_ctx->in_file < 0) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Error opening input file %s\n", optarg);
                    return XMA_APP_ERROR;
                }
                break;

            case DEC_LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&dec_props->latency_logging,
                                             optarg, FLAG_DEC_LATENCY_LOGGING);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in parsing decoder arguments\n");
                return XMA_APP_ERROR;
        }

        if(ret == XMA_APP_ERROR)
            return XMA_APP_ERROR;
    }

    return xlnx_dec_validate_arguments(&dec_ctx->dec_props);
}

/**
 * xlnx_dec_parse_frame: Parses decoder frame to get the properties
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_parse_frame(XlnxDecoderCtx* dec_ctx)
{
    int32_t ret;

    /* Parsing the input file for decoder properties */
    /* Initializing input buffer */
    if((ret = xlnx_dec_get_in_buf(dec_ctx->in_file, &dec_ctx->in_buffer,
                                  1024)) != XMA_APP_SUCCESS)
        return XMA_APP_ERROR;

    /* parsing the first unit to get frame size and frame rate */
    XlnxDecFrameData* in_frame_data = &dec_ctx->frame_data;

    if(dec_ctx->dec_props.codec_type == DECODER_ID_H264) {
        if((ret = xlnx_dec_parse_h264_au(dec_ctx->in_file, &dec_ctx->in_buffer,
                                         in_frame_data, &dec_ctx->in_offset)) !=
           XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to find first unit in h264 input video file!\n");
            return XMA_APP_ERROR;
        }
    } else if(dec_ctx->dec_props.codec_type == DECODER_ID_HEVC) {
        if((ret = xlnx_dec_parse_hevc_au(dec_ctx->in_file, &dec_ctx->in_buffer,
                                         in_frame_data, &dec_ctx->in_offset)) !=
           XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to find first unit in hevc input video file!\n");
            return XMA_APP_ERROR;
        }
    }

    if((in_frame_data->width == 0) || (in_frame_data->height == 0)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Decoder frame size not set!\n");
        return XMA_APP_ERROR;
    } else if((in_frame_data->fr_num == 0) || (in_frame_data->fr_den == 0)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Decoder frame rate not set!\n");
        return XMA_APP_ERROR;
    }

    if((in_frame_data->luma_bit_depth != BITS_PER_PIXEL_8) &&
       (in_frame_data->luma_bit_depth != BITS_PER_PIXEL_10)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported input bit depth\n");
        return XMA_APP_ERROR;
    }

    return ret;
}

/**
 * dec_session: Creates decoder session
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param dec_ctx: Decoder context
 * @param xma_dec_props: XMA decoder properties
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_session(XlnxTranscoderXrmCtx* app_xrm_ctx,
                         XlnxDecoderCtx*       dec_ctx,
                         XmaDecoderProperties* xma_dec_props)
{

    if(xlnx_dec_cu_alloc(app_xrm_ctx, xma_dec_props,
                         &dec_ctx->decode_cu_list_res) != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error in decoder CU allocation\n");
        return XMA_APP_ERROR;
    }

    dec_ctx->dec_session = xma_dec_session_create(xma_dec_props);
    if(!dec_ctx->dec_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Failed decoder session create\n");
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}

/**
 * dec_read_frame: Reads decoder frame data from the input file
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise ERROR
 */
int32_t xlnx_dec_read_frame(XlnxDecoderCtx* dec_ctx)
{

    int32_t ret = XMA_APP_SUCCESS;

    XlnxDecFrameData* in_frame_data = &dec_ctx->frame_data;
    if(dec_ctx->dec_props.codec_type == DECODER_ID_H264) {
        if((ret = xlnx_dec_parse_h264_au(dec_ctx->in_file, &dec_ctx->in_buffer,
                                         in_frame_data, &dec_ctx->in_offset)) <=
           XMA_APP_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to find first unit in h264 input video file!\n");
            return ret;
        }
    } else if(dec_ctx->dec_props.codec_type == DECODER_ID_HEVC) {
        if((ret = xlnx_dec_parse_hevc_au(dec_ctx->in_file, &dec_ctx->in_buffer,
                                         in_frame_data, &dec_ctx->in_offset)) <=
           XMA_APP_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to find first unit in hevc input video file!\n");
            return ret;
        }
    }
    return ret;
}

/**
 * dec_send_frame: Sends data to the decoder for processing
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_send_frame(XlnxDecoderCtx* dec_ctx)
{

    int                data_used;
    int                index       = 0;
    int32_t            ret         = XMA_APP_ERROR;
    XmaDecoderSession* dec_session = dec_ctx->dec_session;
    XlnxDecBuffer*     in_buf      = &(dec_ctx->in_buffer);

    while(index < dec_ctx->in_offset) {
        dec_ctx->dec_in_buf = xma_data_from_buffer_clone(
            in_buf->data + index, dec_ctx->in_offset - index);
        dec_ctx->dec_in_buf->pts = 0;

        ret = xma_dec_session_send_data(dec_session, dec_ctx->dec_in_buf,
                                        &data_used);
        xma_data_buffer_free(dec_ctx->dec_in_buf);
        if(ret == XMA_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error sending data to decoder =%d\n", ret);
            return XMA_APP_ERROR;
        } else if(ret == XMA_TRY_AGAIN) {
            break;
        }
        index += data_used;
    }

    memmove(in_buf->data, in_buf->data + index, in_buf->size - index);
    in_buf->size = in_buf->size - index;

    return ret;
}

/**
 * dec_recv_frame: Receives output data from the decoder
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_recv_frame(XlnxDecoderCtx* dec_ctx)
{

    int32_t ret = XMA_APP_ERROR;

    ret = xma_dec_session_recv_frame(dec_ctx->dec_session, dec_ctx->out_frame);
    return ret;
}

/**
 * dec_get_input_size: Returns the size of the input read and to be sent to the
 * decoder
 *
 * @param dec_ctx: Decoder context
 * @return Size of decoder input
 */
int32_t xlnx_dec_get_input_size(XlnxDecoderCtx* dec_ctx)
{

    return dec_ctx->in_buffer.size;
}

/**
 * dec_send_null_frame: Sends null frame to the decoder to flush the pipeline
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise ERROR
 */
int32_t xlnx_dec_send_null_frame(XlnxDecoderCtx* dec_ctx)
{

    XmaDataBuffer eos_buff;
    int           data_used = 0;

    eos_buff.data.buffer = NULL;
    eos_buff.alloc_size  = 0;
    eos_buff.is_eof      = 0;
    eos_buff.pts         = -1;

    return xma_dec_session_send_data(dec_ctx->dec_session, &eos_buff,
                                     &data_used);
}

/**
 * dec_send_flush_frame: Sends null frame to the decoder to start decoder flush
 *
 * @param dec_ctx: Decoder context
 * @return XMA_APP_SUCCESS on success, otherwise ERROR
 */
int32_t xlnx_dec_send_flush_frame(XlnxDecoderCtx* dec_ctx)
{

    XmaDataBuffer buffer;
    int32_t       offset = 0;

    buffer.data.buffer = NULL;
    buffer.alloc_size  = 0;
    buffer.is_eof      = 1;
    return xma_dec_session_send_data(dec_ctx->dec_session, &buffer, &offset);
}

/**
 * dec_deinit: Sends null frame to the decoder to start decoder flush
 *
 * @param xrm_ctx: XRM context
 * @param dec_ctx: Decoder context
 * @param xma_dec_props: XMA decoder properties
 * @return XMA_APP_SUCCESS on success, otherwise ERROR
 */
int32_t xlnx_dec_deinit(xrmContext xrm_ctx, XlnxDecoderCtx* dec_ctx,
                        XmaDecoderProperties* xma_dec_props)
{

    int32_t ret = XMA_APP_ERROR;

    if(dec_ctx->dec_session != NULL) {
        ret = xma_dec_session_destroy(dec_ctx->dec_session);
    }

    xlnx_dec_free_xma_props(xma_dec_props);

    close(dec_ctx->in_file);
    if(dec_ctx->in_buffer.data) {
        free(dec_ctx->in_buffer.data);
    }

    ret = xrmCuListReleaseV2(xrm_ctx, &dec_ctx->decode_cu_list_res);
   
    return ret;
}
