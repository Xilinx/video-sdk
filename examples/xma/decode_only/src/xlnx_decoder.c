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

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_input_ctx: Close the input file and free the input buffer.
Parameters:
    input_ctx: The input context
------------------------------------------------------------------------------*/
static void xlnx_dec_cleanup_input_ctx(XlnxDecoderInputCtx* input_ctx)
{
    if(fcntl(input_ctx->input_file_info.in_file, F_GETFD) != -1 || 
       errno != EBADF) {
        close(input_ctx->input_file_info.in_file);
        free(input_ctx->input_file_info.input_buffer.data);
    }
}

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_channel_ctx: Close the output file and free the xma frame.
Parameters:
    channel_ctx: The channel context
------------------------------------------------------------------------------*/
static void xlnx_dec_cleanup_channel_ctx(XlnxDecoderChannelCtx* channel_ctx)
{
    if(channel_ctx->out_fp) {
        fclose(channel_ctx->out_fp);
    }
    if(channel_ctx->xframe) {
        xma_frame_free(channel_ctx->xframe);
    }
}

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_ctx: Cleanup the context - free resources, close files.

Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_ctx(XlnxDecoderCtx* ctx)
{
    if(!ctx) {
        return;
    }
    if(ctx->xma_dec_session) {
        xma_dec_session_destroy(ctx->xma_dec_session);
    }
    xlnx_dec_cleanup_xrm_ctx(&ctx->dec_xrm_ctx);
    xlnx_dec_cleanup_decoder_props(&ctx->dec_xma_props);
    xlnx_dec_cleanup_channel_ctx(&ctx->channel_ctx);
    xlnx_dec_cleanup_input_ctx(&ctx->input_ctx);
}

/*------------------------------------------------------------------------------
xlnx_dec_fpga_init: Get/allocate xrm resources, xma initialize.

Parameters:
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_dec_fpga_init(XlnxDecoderCtx* ctx)
{
    // Reserve xrm resource and xma initialize
    int32_t ret = xlnx_dec_reserve_xrm_resource(&ctx->dec_xrm_ctx, 
                                           &ctx->dec_xma_props);
    if(ret != DEC_APP_SUCCESS) {
        return DEC_APP_ERROR;
    }
    if(ctx->dec_params.device_id == -1) {
        return xlnx_dec_allocate_xrm_dec_cu(&ctx->dec_xrm_ctx, 
                                       &ctx->dec_xma_props);
    }
    return xlnx_dec_cu_alloc_device_id(&ctx->dec_xrm_ctx, 
                                  &ctx->dec_xma_props);
}

/*------------------------------------------------------------------------------
xlnx_dec_validate_parse_data: Checks the parse_data to make sure it doesn't have
    obvious violations.
Parameters:
    parse_data: The parse data struct to be checked.
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_dec_validate_parse_data(XlnxDecFrameParseData parse_data)
{
    if(parse_data.width == 0 || parse_data.height == 0 ||
       parse_data.width > MAX_DEC_WIDTH || parse_data.height > MAX_DEC_WIDTH ||
       parse_data.width * parse_data.height > MAX_DEC_WIDTH * MAX_DEC_HEIGHT) { 
        DECODER_APP_LOG_ERROR("Invalid resolution! %dx%d\n", parse_data.width, 
                              parse_data.height);
        return (DEC_APP_ERROR);
    }
    if(parse_data.fr_num == 0 || parse_data.fr_den == 0) {
        DECODER_APP_LOG_ERROR("Invalid fps! Numerator: %d, denominator %d\n", 
                              parse_data.fr_num, parse_data.fr_den);
        return (DEC_APP_ERROR);
    }
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_input_ctx: Create the input context based on commandline 
    arguments
Parameters:
    arguments: The arguments containing commandline info
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on succes
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_dec_create_input_ctx(XlnxDecoderArguments arguments, 
                                         XlnxDecoderCtx* ctx)
{
    int ret;
    XlnxDecoderInputCtx* input_ctx = &ctx->input_ctx;
    struct XlnxStateInfo input_info;
    input_info.input_buffer.data       = NULL;
    input_info.input_buffer.size       = 0;
    input_info.input_buffer.allocated  = 0;
    input_info.loop                    = arguments.loop_count;
    input_info.in_file = open(arguments.input_file, O_RDONLY);
    if(input_info.in_file == -1) {
        DECODER_APP_LOG_ERROR("Unable to open input file %s\n", 
                              arguments.input_file);
        exit(DEC_APP_ERROR);
    }
    XlnxDecFrameParseData parse_data;
    xlnx_dec_init_parse_data(&parse_data);
    ret = xlnx_dec_get_in_buf(input_info.in_file, &input_info.input_buffer, 
                              4096);
    if(ret != RET_SUCCESS) {
        DECODER_APP_LOG_ERROR("Unable to read from file %s!\n", 
                              arguments.input_file);
        close(input_info.in_file);
        exit(DEC_APP_ERROR);
    }
    int offset = 0;
    if(arguments.decoder == HEVC_CODEC_TYPE) {
        ret = xlnx_dec_parse_next_h265_au(input_info.in_file, 
                                         &input_info.input_buffer, 
                                         &parse_data, &offset);
    } else {
        ret = xlnx_dec_parse_next_h264_au(input_info.in_file, 
                                         &input_info.input_buffer, 
                                         &parse_data, &offset);
    }
    if(ret != RET_SUCCESS)
    {
        DECODER_APP_LOG_ERROR("Unable to parse first input unit from input "
                              "file %s!\n", arguments.input_file);
        close(input_info.in_file);
        exit(DEC_APP_ERROR);
    }
    ret = xlnx_dec_validate_parse_data(parse_data);
    if(ret != DEC_APP_SUCCESS) {
        exit(DEC_APP_ERROR);
    }
    input_ctx->input_file_info  = input_info;
    input_ctx->parse_data       = parse_data;
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_channel_ctx: Set the channel variables and open the output file 
    for writing.
Parameters:
    arguments: The arguments containing commandline info
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
// Just opening the output file and copying input resolution 
static int32_t xlnx_dec_create_channel_ctx(XlnxDecoderArguments arguments, 
                                           XlnxDecoderCtx* ctx)
{
    XlnxDecoderChannelCtx* channel_ctx = &ctx->channel_ctx;
    channel_ctx->out_fp = fopen(arguments.output_file, "wb");
    if(!channel_ctx->out_fp) {
        DECODER_APP_LOG_ERROR("Unable to open output file %s\n", 
                              arguments.output_file);
        exit(DEC_APP_ERROR);
    }
    channel_ctx->num_frames_to_decode  = arguments.num_frames;
    
    /* channel_ctx.xframe will be created later. */
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_xma_param_ctx: Add custom decoder params to the decoder context
Parameters:
    arguments: The arguments containing commandline info
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int32_t xlnx_dec_create_xma_param_ctx(XlnxDecoderArguments arguments, 
                                             XlnxDecoderCtx* ctx)
                                        
{
    XlnxDecFrameParseData      parse_data =  ctx->input_ctx.parse_data;
    XlnxDecoderProperties* param_ctx  = &ctx->dec_params;
    param_ctx->device_id          =  arguments.device_id;
    param_ctx->fps                =  parse_data.fr_num / parse_data.fr_den;
    param_ctx->width              =  parse_data.width;
    param_ctx->height             =  parse_data.height;
    param_ctx->log_level          =  arguments.log_level;
    param_ctx->low_latency        =  arguments.low_latency;
    param_ctx->entropy_buf_cnt    =  arguments.entropy_buf_cnt;
    /* Decoder plugin only supports zero copy */
    param_ctx->zero_copy          =  1; 
    param_ctx->latency_logging    =  arguments.latency_logging;
    param_ctx->splitbuff_mode     =  arguments.splitbuff_mode;
    param_ctx->codec_type         =  arguments.decoder; // H264 0, H265 1
    param_ctx->bit_depth          =  8; 
    param_ctx->chroma_mode        =  420;
    /* 0 = unknown; 1 = progressive; 2 = top first; 3 = bottom first */
    param_ctx->scan_type          = 1;
    if(param_ctx->codec_type == H264_CODEC_TYPE) {
        param_ctx->profile_idc = parse_data.h264_seq_parameter_set[
            parse_data.h264_pic_parameter_set[parse_data.current_h264_pps].
            seq_parameter_set_id].profile_idc;

        param_ctx->level_idc = parse_data.h264_seq_parameter_set[
            parse_data.h264_pic_parameter_set[parse_data.current_h264_pps].
            seq_parameter_set_id].level_idc;
    }
    else if(param_ctx->codec_type == H265_CODEC_TYPE) {
        param_ctx->profile_idc = parse_data.hevc_seq_parameter_set[
            parse_data.latest_hevc_sps].profile_idc;

        param_ctx->level_idc = parse_data.hevc_seq_parameter_set[
            parse_data.latest_hevc_sps].level_idc;
    }

    xlnx_dec_create_xma_dec_props(param_ctx, &ctx->dec_xma_props);
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_xframe: Create the xma frame ctx->channel_ctx.xframe for decoding
Parameters:
    ctx: the decoder context
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int32_t xlnx_dec_create_xframe(XlnxDecoderCtx* ctx)
{
    XlnxDecoderProperties param_ctx     = ctx->dec_params;
    XmaFrame*             xframe        = calloc(1, sizeof(*xframe));
    xframe->side_data                   = NULL;
    xframe->frame_props.format          = XMA_VCU_NV12_FMT_TYPE;
    xframe->frame_props.width           = param_ctx.width;
    xframe->frame_props.height          = param_ctx.height;
    xframe->frame_props.bits_per_pixel  = param_ctx.bit_depth;
    xframe->frame_rate.numerator        = param_ctx.fps;
    xframe->frame_rate.denominator      = 1;

    for(int i = 0; i < xma_frame_planes_get(&xframe->frame_props); i++) {
        xframe->data[i].buffer       = NULL;
        xframe->data[i].buffer_type  = XMA_DEVICE_BUFFER_TYPE;
        xframe->data[i].refcount     = 1;
        xframe->data[i].is_clone     = 1;
    }
    ctx->channel_ctx.xframe = xframe;
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_dec_xrm_ctx: Initialize the decoder xrm context
Parameters:
    ctx: The decoder context
Return:
    The initialized xrm decoder context.
------------------------------------------------------------------------------*/
static XlnxDecoderXrmCtx xlnx_dec_create_dec_xrm_ctx(XlnxDecoderCtx* ctx)
{
    XlnxDecoderXrmCtx dec_xrm_ctx;
    memset(&dec_xrm_ctx, 0, sizeof(dec_xrm_ctx));
    dec_xrm_ctx.xrm_reserve_id    = 0;
    dec_xrm_ctx.dec_load          = 0;
    dec_xrm_ctx.decode_res_in_use = 0;
    memset(&dec_xrm_ctx.decode_cu_list_res, 0, 
           sizeof(dec_xrm_ctx.decode_cu_list_res));

    return dec_xrm_ctx;
}

/*------------------------------------------------------------------------------
xlnx_dec_create_decoder_context: Creates the context based on user arguments. 
    It parses the header of the input file to get relevant codec info. This 
    does not create the xma session. Nor does it initialize the fpga.
Parameters:
    arguments: The argument struct containing decoder param, input, output file 
    info
    ctx: A pointer to the decoder context
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
int32_t xlnx_dec_create_decoder_context(XlnxDecoderArguments arguments, 
                                        XlnxDecoderCtx* ctx)
{
    xlnx_dec_create_input_ctx(arguments, ctx);
    xlnx_dec_create_channel_ctx(arguments, ctx);
    xlnx_dec_create_xma_param_ctx(arguments, ctx);
    xlnx_dec_create_xframe(ctx);
    ctx->dec_xrm_ctx = xlnx_dec_create_dec_xrm_ctx(ctx);
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_scan_next_au: Scans the next access unit from the input file into 
    ctx->input_ctx.input_file_info.buffer. This should not be used for initial 
    setup.
Parameters:
    ctx: The decoder context
        input_ctx.input_file_info: Contains the input file and input buffer
        input_ctx.parse_data: Contains/stores the parse data
    offset: Pointer to the offset which is set by the decoder parser.
Return:
    RET_SUCCESS on success
    RET_EOS on end of stream
    RET_EOF on end of file
------------------------------------------------------------------------------*/
int xlnx_dec_scan_next_au(XlnxDecoderCtx* ctx, int* offset)
{
    int ret = RET_ERROR;
    int codec_type = ctx->dec_params.codec_type;
    struct XlnxStateInfo* input_info = &ctx->input_ctx.input_file_info;
    XlnxDecFrameParseData* parse_data = &ctx->input_ctx.parse_data;
    if(codec_type == H264_CODEC_TYPE) {
        ret = xlnx_dec_parse_next_h264_au(input_info->in_file, 
                                         &input_info->input_buffer, 
                                          parse_data, offset);
    } else if(codec_type == H265_CODEC_TYPE) {
        ret = xlnx_dec_parse_next_h265_au(input_info->in_file, 
                                         &input_info->input_buffer, 
                                          parse_data, offset);
    }
    if(ret == RET_ERROR) {
        DECODER_APP_LOG_ERROR("Error finding access unit from input file!\n");
        xlnx_dec_cleanup_ctx(ctx);
        exit(DEC_APP_ERROR);
    }
    if(ret == RET_EOF) { 
        if(input_info->loop > 0 || input_info->loop == -1) {
            lseek(input_info->in_file, 0, SEEK_SET);
            if(input_info->loop != -1) {
                input_info->loop--;
            }
        } else if(input_info->input_buffer.size <= 0) {
            return RET_EOS;
        }
    }
    return ret;
}

/*------------------------------------------------------------------------------
xlnx_dec_get_buffer_from_fpga: Transfer the buffers stored on fpga into the 
    host.
Parameters:
    ctx: The decoder context
    buffer_size: A pointer to the size of the buffer.
Return:
    A pointer to the host buffer of size * buffer_size
------------------------------------------------------------------------------*/
uint8_t* xlnx_dec_get_buffer_from_fpga(XlnxDecoderCtx* ctx, size_t*
                                       buffer_size)
{
    int aligned_width, aligned_height;
    int ret = XMA_ERROR;
    XmaFrame* decoded_frame = ctx->channel_ctx.xframe;
    uint8_t* host_buffer;
    aligned_width  = ALIGN(ctx->dec_params.width, STRIDE_ALIGN);
    aligned_height = ALIGN(ctx->dec_params.height, HEIGHT_ALIGN);
    *buffer_size  = aligned_width * aligned_height + // Y plane
                    (aligned_width * aligned_height) / 2; // UV plane

    host_buffer = (uint8_t *)xvbm_buffer_get_host_ptr(decoded_frame->data[0].
                                                      buffer);
    ret = xvbm_buffer_read(decoded_frame->data[0].buffer, host_buffer, 
                           (*buffer_size), 0);
    if(ret != XMA_SUCCESS) {
        DECODER_APP_LOG_ERROR("xvbm_buffer_read failed\n");
        xlnx_dec_cleanup_ctx(ctx);
        exit(DEC_APP_ERROR);
    }
    xvbm_buffer_pool_entry_free(decoded_frame->data[0].buffer);
    return host_buffer;
}

/*------------------------------------------------------------------------------
xlnx_dec_send_null_data: Send null data to flush the remaining frames out of 
    the fpga
Parameters:
    ctx: The decoder context
    is_eof: Whether this is the last frame to be sent or not.
Return:
    XMA_SUCCESS on success
    XMA_ERROR on error
------------------------------------------------------------------------------*/
int xlnx_dec_send_null_data(XlnxDecoderCtx* ctx)
{
    // Flush decoder
    int data_used = 0;
    int ret = DEC_APP_ERROR;
    if(!ctx->is_flush_sent) {
        ctx->is_flush_sent = true;
        ctx->input_ctx.xbuffer.is_eof = 1;
        ret = xma_dec_session_send_data(ctx->xma_dec_session, 
                                       &ctx->input_ctx.xbuffer, &data_used);
    } else {
        XmaDataBuffer eos_buffer;
        memset(&eos_buffer, 0, sizeof(eos_buffer));
        eos_buffer.data.buffer  =  NULL;
        eos_buffer.alloc_size   =  0;
        eos_buffer.is_eof       =  0;
        eos_buffer.pts          = -1;
        ret = xma_dec_session_send_data(ctx->xma_dec_session, &eos_buffer, 
                                        &data_used);
    }
    return ret;
}

/*------------------------------------------------------------------------------
xlnx_dec_send_data: sends data to the decoder for processing
Parameters:
    ctx: ctx for the decoder.
        session: Used to send data to xma plugin
        input_buffer: Contains the data to send 
    end: amount of data to send
Return:
    RET_SUCCESS on success, otherwise RET_ERROR
------------------------------------------------------------------------------*/
int xlnx_dec_send_data(XlnxDecoderCtx* ctx, int size)
{
    int data_used = 0;
    int offset    = 0;
    int rc        = XMA_ERROR;
    int pts       = ctx->pts;
    XlnxDecBuffer* input_buffer = &ctx->input_ctx.input_file_info.input_buffer;
    XmaDataBuffer* xbuffer      = &ctx->input_ctx.xbuffer;
    while(offset < size) {
        xbuffer->data.buffer = input_buffer->data + offset;
        xbuffer->alloc_size  = size;
        xbuffer->is_eof      = 0;
        xbuffer->pts         = pts;
        rc = xma_dec_session_send_data(ctx->xma_dec_session, xbuffer, 
                                       &data_used);
        if(rc == XMA_ERROR) {
            DECODER_APP_LOG_ERROR("Error sending data to decoder. Data %d\n", 
                                  ctx->num_frames_sent);
            xlnx_dec_cleanup_ctx(ctx);
            exit(DEC_APP_ERROR);
        } else if(rc == XMA_TRY_AGAIN) {
            /* plugin buffers full, need to take frames out. */
            break;
        }
        offset += data_used;
        pts = -1; // Only first packet will carry pts
        ctx->num_frames_sent++;
    }
    memmove(input_buffer->data, input_buffer->data + offset, input_buffer->size 
            - offset);
    input_buffer->size = input_buffer->size - offset;
    return RET_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_print_segment_performance: Print the performance since the last 
    segment.
Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_print_segment_performance(XlnxDecoderCtx* ctx)
{
    double time_since_last_segment = xlnx_utils_get_segment_time(
                                                                  &ctx->timer);
    if(time_since_last_segment < 0.5) {
        return;
    }
    fprintf(stderr, "\rFrame=%5zu Total FPS=%.03f Current FPS=%.03f\r", 
            ctx->num_frames_decoded, 
            (float) ctx->num_frames_decoded / 
            xlnx_utils_get_total_time(&ctx->timer), 
            (ctx->num_frames_decoded - ctx->timer.last_displayed_frame) / 
            time_since_last_segment);
    fflush(stderr);
    ctx->timer.last_displayed_frame = ctx->num_frames_decoded;
    xlnx_utils_set_segment_time(&ctx->timer);
}

/*------------------------------------------------------------------------------
xlnx_dec_print_total_performance: Print the total performance of the decoder.
Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_print_total_performance(XlnxDecoderCtx* ctx)
{
    double realtime_taken = xlnx_utils_get_total_time(&ctx->timer);
    fprintf(stderr, "\nFrames Decoded: %zu, Time Elapsed: %.03lf\r\n", 
            ctx->num_frames_decoded, realtime_taken);
    fprintf(stderr, "Total FPS: %.03lf\r\n",
            ctx->num_frames_decoded / realtime_taken);
}
