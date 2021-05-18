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

#include "xlnx_decoder.h"

static struct option dec_options[] =
{
    {FLAG_DEC_INPUT_FILE,       required_argument, 0, DEC_INPUT_FILE_ARG},
    {FLAG_DEC_CODEC_TYPE,       required_argument, 0, DEC_CODEC_ID_ARG},
    {FLAG_DEC_LOW_LATENCY,      required_argument, 0, DEC_LOW_LATENCY_ARG},
    {FLAG_DEC_LATENCY_LOGGING,  required_argument, 0, DEC_LATENCY_LOGGING_ARG},
    {0, 0, 0, 0}
};

/*-----------------------------------------------------------------------------
xlnx_dec_cu_alloc_device_id: Perform decoder CU allocation based on device id.

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_dec_props: XMA decoder properties
decode_cu_list_res: Decoder CU list resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_cu_alloc_device_id(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                                      XmaDecoderProperties *xma_dec_props, 
                                      xrmCuListResource *decode_cu_list_res)
{
    xrmCuProperty decode_cu_hw_prop, decode_cu_sw_prop;

    int32_t ret = -1;

    memset(&decode_cu_hw_prop, 0, sizeof(xrmCuProperty));
    memset(&decode_cu_sw_prop, 0, sizeof(xrmCuProperty));
    memset(decode_cu_list_res, 0, sizeof(xrmCuListResource));

    strcpy(decode_cu_hw_prop.kernelName, "decoder");
    strcpy(decode_cu_hw_prop.kernelAlias, "DECODER_MPSOC");
    decode_cu_hw_prop.devExcl = false;
    decode_cu_hw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(app_xrm_ctx->transcode_load.dec_load);

    strcpy(decode_cu_sw_prop.kernelName, "kernel_vcu_decoder");
    decode_cu_sw_prop.devExcl = false;
    decode_cu_sw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);

    ret = xrmCuAllocFromDev(app_xrm_ctx->xrm_ctx, app_xrm_ctx->device_id, 
          &decode_cu_hw_prop, &decode_cu_list_res->cuResources[0]);

    if (ret <= TRANSCODE_APP_FAILURE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "xrm failed to allocate decoder resources on device %d\n", 
            app_xrm_ctx->device_id );
        return ret;
    }
    else {
        ret = xrmCuAllocFromDev(app_xrm_ctx->xrm_ctx, app_xrm_ctx->device_id, 
              &decode_cu_sw_prop, &decode_cu_list_res->cuResources[1]);
        if (ret <= TRANSCODE_APP_FAILURE) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "xrm failed to allocate decoder resources on device %d\n", 
                app_xrm_ctx->device_id );
            return ret;
        }
    }
    app_xrm_ctx->dec_res_in_use =1;

    /* Set XMA plugin SO and device index */
    xma_dec_props->plugin_lib = 
            decode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_dec_props->dev_index = decode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_dec_props->ddr_bank_index = -1;
    xma_dec_props->cu_index = decode_cu_list_res->cuResources[1].cuId;
    xma_dec_props->channel_id = decode_cu_list_res->cuResources[1].channelId;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_dec_cu_alloc_reserve_id: Perform decoder CU allocation based on reservation 
                         index.

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_dec_props: XMA decoder properties
decode_cu_list_res: Decoder CU list resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_cu_alloc_reserve_id(XlnxTranscoderXrmCtx *app_xrm_ctx,
                                       XmaDecoderProperties *xma_dec_props,
                                       xrmCuListResource *decode_cu_list_res)
{

    /* XRM decoder allocation */
    xrmCuListProperty decode_cu_list_prop;

    memset(&decode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(decode_cu_list_res, 0, sizeof(xrmCuListResource));

    decode_cu_list_prop.cuNum = 2;
    strcpy(decode_cu_list_prop.cuProps[0].kernelName, "decoder");
    strcpy(decode_cu_list_prop.cuProps[0].kernelAlias, "DECODER_MPSOC");
    decode_cu_list_prop.cuProps[0].devExcl = false;
    decode_cu_list_prop.cuProps[0].requestLoad = 
                                   XRM_PRECISION_1000000_BIT_MASK(app_xrm_ctx->transcode_load.dec_load);
    decode_cu_list_prop.cuProps[0].poolId = app_xrm_ctx->reserve_idx;

    strcpy(decode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_decoder");
    decode_cu_list_prop.cuProps[1].devExcl = false;
    decode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    decode_cu_list_prop.cuProps[1].poolId = app_xrm_ctx->reserve_idx;

    if (xrmCuListAlloc(app_xrm_ctx->xrm_ctx, &decode_cu_list_prop, 
                       decode_cu_list_res) != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed to allocate decoder cu from reserve id \n");
        return TRANSCODE_APP_FAILURE;
    }
    app_xrm_ctx->dec_res_in_use =1;

    /* Set XMA plugin SO and device index */
    xma_dec_props->plugin_lib = 
        decode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_dec_props->dev_index = decode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_dec_props->ddr_bank_index = -1;
    xma_dec_props->cu_index = decode_cu_list_res->cuResources[1].cuId;
    xma_dec_props->channel_id = decode_cu_list_res->cuResources[1].channelId;

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_cu_alloc: Allocates decoder CU

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_dec_props: Decoder XMA properties
decode_cu_list_res: Decoder CU list resource

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_cu_alloc(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                                 XmaDecoderProperties *xma_dec_props,
                                 xrmCuListResource *decode_cu_list_res)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    if(app_xrm_ctx->device_id >= 0) {
        ret = xlnx_dec_cu_alloc_device_id(app_xrm_ctx, xma_dec_props, 
                                          decode_cu_list_res);
    }
    else {
        ret = xlnx_dec_cu_alloc_reserve_id(app_xrm_ctx, xma_dec_props, 
                                           decode_cu_list_res);
    }

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_dec_update_props: Updates XMA decoder properties and the parameters

Parameters:
dec_ctx: Decoder context
xma_dec_props: XMA decoder properties structure
-----------------------------------------------------------------------------*/
void xlnx_dec_update_props(XlnxDecoderCtx *dec_ctx, 
                      XmaDecoderProperties *xma_dec_props)
{
    XlnxDecoderProperties *dec_props = &dec_ctx->dec_props;
    XlnxDecFrameData *frame_data = &dec_ctx->frame_data;

    dec_props->width  = frame_data->width;
    dec_props->height = frame_data->height;
    dec_props->fps    = (frame_data->fr_num/frame_data->fr_den);

    if (dec_props->codec_type == DECODER_ID_H264) {
        dec_props->profile = frame_data->h264_seq_parameter_set[
            frame_data->h264_pic_parameter_set[frame_data->current_h264_pps].
                seq_parameter_set_id].profile_idc;
    }
    else if (dec_props->codec_type == DECODER_ID_HEVC) {
        dec_props->profile = frame_data->hevc_seq_parameter_set[
            frame_data->latest_hevc_sps].profile_idc;
    }

    if (dec_props->codec_type == DECODER_ID_H264) {
        dec_props->level = frame_data->h264_seq_parameter_set[
            frame_data->h264_pic_parameter_set[frame_data->current_h264_pps].
                seq_parameter_set_id].level_idc;
    }
    else if (dec_props->codec_type == DECODER_ID_HEVC) {
        dec_props->level = frame_data->hevc_seq_parameter_set[
            frame_data->latest_hevc_sps].level_idc;
    }

    xlnx_dec_get_xma_props(&dec_ctx->dec_props, xma_dec_props);

    return;
}

/*-----------------------------------------------------------------------------
init_parse_data: Initialize the decoder header structure

Parameters:
dec_frame_data: Decoder frame data

Return:
TRANSCODE_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_init_parse_data (XlnxDecFrameData *dec_frame_data)
{
    memset (dec_frame_data, 0, sizeof (XlnxDecFrameData));

    int i;
    for (i = 0; i < 32; i++)
        dec_frame_data->h264_seq_parameter_set[i].valid = 0;
    for (i = 0; i < 256; i++)
        dec_frame_data->h264_pic_parameter_set[i].valid = 0;

    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt_bottom = -1;
    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt[0] = -1;
    dec_frame_data->last_h264_slice_header.delta_pic_order_cnt[1] = -1;
    dec_frame_data->last_h264_slice_header.frame_num = 0;
    dec_frame_data->last_h264_slice_header.idr_pic_id = 0;
    dec_frame_data->last_h264_slice_header.pic_order_cnt_lsb = 0;
    dec_frame_data->last_h264_slice_header.pic_parameter_set_id = 0;
    dec_frame_data->last_h264_slice_header.field_pic_flag = 0;
    dec_frame_data->last_h264_slice_header.bottom_field_flag = 0;
    dec_frame_data->last_h264_slice_header.nal_ref_idc = 0;
    dec_frame_data->last_h264_slice_header.nal_unit_type = 0;

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_context_init: Initializes decoder context

Parameters:
dec_ctx: Decoder context
-----------------------------------------------------------------------------*/
void xlnx_dec_context_init(XlnxDecoderCtx *dec_ctx)
{

    /* Initialize the decoder parameters to default */
    dec_ctx->dec_props.entropy_buffers_count = 2;
    dec_ctx->dec_props.latency_logging = 0;
    dec_ctx->dec_props.splitbuff_mode = 0;
    dec_ctx->dec_props.bit_depth = 8;
    dec_ctx->dec_props.codec_type = -1;
    dec_ctx->flush_sent = false;
    dec_ctx->dec_props.low_latency = 0;
    dec_ctx->dec_props.scan_type = 1;
    dec_ctx->dec_props.chroma_mode = 420;
    /* always zero copy output */
    dec_ctx->dec_props.zero_copy = 1;

    memset (dec_ctx->out_frame, 0, sizeof (XmaFrame));
    dec_ctx->out_frame->data[0].buffer_type = XMA_DEVICE_BUFFER_TYPE;
    dec_ctx->out_frame->data[0].buffer = NULL;

    xlnx_dec_init_parse_data(&dec_ctx->frame_data);
    return;
}

/*-----------------------------------------------------------------------------
xlnx_dec_validate_arguments: Validates decoder arguments

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_validate_arguments(XlnxDecoderCtx *dec_ctx)
{

    if((dec_ctx->dec_props.low_latency != 0) && 
        (dec_ctx->dec_props.low_latency != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
        "Invalid decoder low latency %d \n", dec_ctx->dec_props.low_latency);
        return TRANSCODE_APP_FAILURE;
    }

    if((dec_ctx->dec_props.latency_logging != 0) && 
        (dec_ctx->dec_props.latency_logging != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
            "Invalid decoder low latency %d \n", 
            dec_ctx->dec_props.latency_logging);
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_args: Parses the decode command line arguments

Parameters:
argc: Argument count
*argv[]: Arguments
dec_ctx: Decoder context
param_flag: Indicates whether the first decoder argument has been parsed by 
the caller

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_parse_args(int32_t argc, char *argv[], XlnxDecoderCtx *dec_ctx, 
        int32_t param_flag)
{

    int32_t option_index;
    int32_t flag = 0;
    int32_t ret = TRANSCODE_APP_SUCCESS;

    while(flag != DEC_INPUT_FILE_ARG)
    {
        if(param_flag == 0) {
            flag = getopt_long_only(argc, argv, "", dec_options, &option_index);
            if(flag == -1) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in decoder parameters parsing \n");
                return TRANSCODE_APP_FAILURE;
            }
        }
        else {
            flag = param_flag;
            param_flag = 0;
        }

        switch (flag)
        {
            case DEC_CODEC_ID_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264"))
                    dec_ctx->dec_props.codec_type = DECODER_ID_H264;
                else if(!strcmp(optarg, "mpsoc_vcu_hevc"))
                    dec_ctx->dec_props.codec_type = DECODER_ID_HEVC;
                else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                            "Unsupported decoder codec option %s \n", optarg);
                    return TRANSCODE_APP_FAILURE;
                }
                break;

            case DEC_LOW_LATENCY_ARG:
                ret = xlnx_utils_set_int_arg(&dec_ctx->dec_props.low_latency , optarg, FLAG_DEC_LOW_LATENCY);
                break;

            case DEC_INPUT_FILE_ARG:
                dec_ctx->in_file = open(optarg, O_RDONLY);
                if(dec_ctx->in_file == TRANSCODE_APP_FAILURE) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                            "Error opening input file %s \n", optarg);
                    return TRANSCODE_APP_FAILURE;
                }
                break;

            case DEC_LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&dec_ctx->dec_props.latency_logging, optarg, FLAG_DEC_LATENCY_LOGGING);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in parsing decoder arguments \n");
                return TRANSCODE_APP_FAILURE;
        }

        if(ret == TRANSCODE_APP_FAILURE)
            return TRANSCODE_APP_FAILURE;
    }

    return xlnx_dec_validate_arguments(dec_ctx);
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_frame: Parses decoder frame to get the properties

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_parse_frame(XlnxDecoderCtx *dec_ctx)
{
    int32_t ret;

    /* Parsing the input file for decoder properties */
    /* Initializing input buffer */
    if((ret = xlnx_dec_get_in_buf (dec_ctx->in_file, &dec_ctx->in_buffer, 1024)) 
            != TRANSCODE_APP_SUCCESS) 
        return TRANSCODE_APP_FAILURE;

    /* parsing the first unit to get frame size and frame rate */
    XlnxDecFrameData *in_frame_data = &dec_ctx->frame_data;

    if(dec_ctx->dec_props.codec_type == DECODER_ID_H264) {
        if((ret = xlnx_dec_parse_h264_au (dec_ctx->in_file, &dec_ctx->in_buffer, 
            in_frame_data, &dec_ctx->in_offset)) != TRANSCODE_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Failed to find first unit in h264 input video file!\n");
            return TRANSCODE_APP_FAILURE;
        }
    }
    else if(dec_ctx->dec_props.codec_type == DECODER_ID_HEVC) {
        if((ret = xlnx_dec_parse_hevc_au (dec_ctx->in_file, &dec_ctx->in_buffer, 
            in_frame_data, &dec_ctx->in_offset)) != TRANSCODE_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Failed to find first unit in hevc input video file!\n");
            return TRANSCODE_APP_FAILURE;
        }
    } 

    if((in_frame_data->width == 0) || (in_frame_data->height == 0)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Decoder frame size not set!\n");
        return TRANSCODE_APP_FAILURE;
    }
    else if((in_frame_data->fr_num == 0) || (in_frame_data->fr_den == 0)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Decoder frame rate not set!\n");
        return TRANSCODE_APP_FAILURE;
    }

    return ret;

}

/*-----------------------------------------------------------------------------
dec_session: Creates decoder session

Parameters:
app_xrm_ctx: Transcoder XRM context
dec_ctx: Decoder context
xma_dec_props: XMA decoder properties

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_session(XlnxTranscoderXrmCtx *app_xrm_ctx,
                         XlnxDecoderCtx *dec_ctx, 
                         XmaDecoderProperties *xma_dec_props)
{

    if(xlnx_dec_cu_alloc(app_xrm_ctx, xma_dec_props, 
                &dec_ctx->decode_cu_list_res) != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error in decoder CU allocation \n");
        return TRANSCODE_APP_FAILURE;
    }

    dec_ctx->dec_session = xma_dec_session_create(xma_dec_props);
    if(!dec_ctx->dec_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed decoder session create \n");
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
dec_read_frame: Reads decoder frame data from the input file

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise ERROR
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_read_frame(XlnxDecoderCtx *dec_ctx)
{

    int32_t ret = TRANSCODE_APP_SUCCESS;

    XlnxDecFrameData *in_frame_data = &dec_ctx->frame_data;
    if(dec_ctx->dec_props.codec_type == DECODER_ID_H264) {
        if((ret = xlnx_dec_parse_h264_au (dec_ctx->in_file, &dec_ctx->in_buffer, 
            in_frame_data, &dec_ctx->in_offset)) <= TRANSCODE_APP_FAILURE) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Failed to find first unit in h264 input video file!\n");
            return ret;
        }
    }
    else if(dec_ctx->dec_props.codec_type == DECODER_ID_HEVC) {
        if ((ret = xlnx_dec_parse_hevc_au (dec_ctx->in_file, &dec_ctx->in_buffer, 
            in_frame_data, &dec_ctx->in_offset)) <= TRANSCODE_APP_FAILURE) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Failed to find first unit in hevc input video file!\n");
            return ret;
        }
    }
    return ret;
}

/*-----------------------------------------------------------------------------
dec_send_frame: Sends data to the decoder for processing

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_send_frame(XlnxDecoderCtx *dec_ctx)
{

    int data_used;
    int index = 0;
    int32_t ret = TRANSCODE_APP_FAILURE;
    XmaDecoderSession* dec_session = dec_ctx->dec_session;
    XlnxDecBuffer *in_buf = &(dec_ctx->in_buffer);

    while(index < dec_ctx->in_offset)
    {
        dec_ctx->dec_in_buf = xma_data_from_buffer_clone(in_buf->data + index, 
                               dec_ctx->in_offset - index);
        dec_ctx->dec_in_buf->pts = 0;

        ret = xma_dec_session_send_data(dec_session, dec_ctx->dec_in_buf, 
                                         &data_used);
        xma_data_buffer_free(dec_ctx->dec_in_buf);
        if(ret == XMA_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Error sending data to decoder =%d\n", ret);
            return TRANSCODE_APP_FAILURE;
        } else if(ret == XMA_TRY_AGAIN) {
            break;
        }
        index += data_used;

    }

    memmove (in_buf->data, in_buf->data + index, in_buf->size - index);
    in_buf->size = in_buf->size - index;

    return ret;
}

/*-----------------------------------------------------------------------------
dec_recv_frame: Receives output data from the decoder

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_recv_frame(XlnxDecoderCtx *dec_ctx)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    ret = xma_dec_session_recv_frame(dec_ctx->dec_session, 
                                     dec_ctx->out_frame);
    return ret;
}

/*-----------------------------------------------------------------------------
dec_get_input_size: Returns the size of the input read and to be sent to the 
                    decoder

Parameters:
dec_ctx: Decoder context

Return:
Size of decoder input
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_get_input_size(XlnxDecoderCtx *dec_ctx)
{

    return dec_ctx->in_buffer.size;
}

/*-----------------------------------------------------------------------------
dec_send_null_frame: Sends null frame to the decoder to flush the pipeline

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise ERROR
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_send_null_frame(XlnxDecoderCtx *dec_ctx)
{

    XmaDataBuffer eos_buff;
    int data_used   = 0;

    eos_buff.data.buffer = NULL;
    eos_buff.alloc_size = 0;
    eos_buff.is_eof = 0;
    eos_buff.pts = -1;

    return xma_dec_session_send_data(dec_ctx->dec_session, &eos_buff, 
            &data_used);
}

/*-----------------------------------------------------------------------------
dec_send_flush_frame: Sends null frame to the decoder to start decoder flush

Parameters:
dec_ctx: Decoder context

Return:
TRANSCODE_APP_SUCCESS on success, otherwise ERROR
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_send_flush_frame(XlnxDecoderCtx *dec_ctx)
{

    XmaDataBuffer buffer;
    int32_t offset = 0;

    buffer.data.buffer = NULL;
    buffer.alloc_size = 0;
    buffer.is_eof = 1;
    return xma_dec_session_send_data(dec_ctx->dec_session, &buffer, &offset);
}

/*-----------------------------------------------------------------------------
dec_deinit: Sends null frame to the decoder to start decoder flush

Parameters:
xrm_ctx: XRM context
dec_ctx: Decoder context
xma_dec_props: XMA decoder properties

Return:
TRANSCODE_APP_SUCCESS on success, otherwise ERROR
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_deinit(xrmContext *xrm_ctx, XlnxDecoderCtx *dec_ctx, 
                        XmaDecoderProperties *xma_dec_props)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    ret = xrmCuListRelease(xrm_ctx, &dec_ctx->decode_cu_list_res);

    if(dec_ctx->dec_session != NULL) {
        ret = xma_dec_session_destroy(dec_ctx->dec_session);
    }

    xlnx_dec_free_xma_props(xma_dec_props);

    close(dec_ctx->in_file);
    if (dec_ctx->in_buffer.data) {
        free (dec_ctx->in_buffer.data);
    }

    return ret;
}
