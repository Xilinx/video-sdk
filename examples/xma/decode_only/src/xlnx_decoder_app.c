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

int signal_caught;

/*-----------------------------------------------------------------------------
dec_signal_handler: Signal handler function
Parameters:
signum: Signal number
-----------------------------------------------------------------------------*/
static void dec_signal_handler(int32_t signum)
{
    switch (signum)
    {
        case SIGHUP:
        case SIGINT:
        case SIGQUIT:
        case SIGABRT:
        case SIGTERM:
            signal_caught = 1;
            break;
    }
}

/*------------------------------------------------------------------------------
dec_set_signal_handler: Signal handler initialization.
Return:
DEC_APP_SUCCESS or APP_FAILURE
------------------------------------------------------------------------------*/
static int dec_set_signal_handler()
{
    signal_caught = 0;
    struct sigaction action;
    action.sa_handler = dec_signal_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGHUP,  &action, NULL);
    sigaction(SIGINT,  &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
dec_write_host_buffer_to_file: Write the host buffer to a file
Parameters:
    ctx: The decoder context
    hb: The host buffer
    size: The size of the buffer to write
    file: The file to which hb should be written
Return:
    DEC_APP_SUCCESS or APP_FAILURE
------------------------------------------------------------------------------*/
static int dec_write_host_buffer_to_file(XlnxDecoderCtx* ctx, uint8_t* hb, 
                                         size_t size, FILE* file)
{
    int    stride          = ctx->dec_params.width;
    int    aligned_width   = ALIGN(ctx->dec_params.width, STRIDE_ALIGN);
    int    aligned_height  = ALIGN(ctx->dec_params.height, HEIGHT_ALIGN);
    size_t plane_size      = ctx->dec_params.width * ctx->dec_params.height;
    size_t buff_plane_size = aligned_width * aligned_height;
    int offset = 0;
    int bytes_written;
    int bytes_read;
    int num_planes = xma_frame_planes_get(
                                        &ctx->channel_ctx.xframe->frame_props);
    for(int plane_id = 0; plane_id < num_planes; plane_id++) {
        bytes_written = 0;
        bytes_read = 0;
        if(plane_id > 0) {
            buff_plane_size = aligned_width * aligned_width / 2;
            plane_size = ctx->dec_params.width * ctx->dec_params.height / 2;
        }
        while(bytes_read < buff_plane_size) {
            if(bytes_written < plane_size) {
                fwrite(hb + offset, 1, stride, file);
                bytes_written += stride;
            }
            offset += aligned_width;
            bytes_read += aligned_width;
        }
    }
    fflush(file);
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_dec_fpga_init: Get/allocate xrm resources, xma initialize.

Parameters:
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static void dec_export_to_file(XlnxDecoderCtx* ctx)
{
    XlnxDecoderChannelCtx channel_ctx = ctx->channel_ctx;
    FILE*                 out_fp      = channel_ctx.out_fp;
    uint8_t*              host_buffer = 0;
    size_t                buffer_size = 0;
    host_buffer = xlnx_dec_get_buffer_from_fpga(ctx, &buffer_size);
    dec_write_host_buffer_to_file(ctx, host_buffer, buffer_size, out_fp);
}

/*------------------------------------------------------------------------------
dec_decode_frame: Decode a frame

Parameters:
    ctx: The decoder context
Return:
    XMA_SUCCESS on success
------------------------------------------------------------------------------*/
static int dec_decode_frame(XlnxDecoderCtx* ctx)
{
    int ret;
    int offset  = 0;
    ret = xlnx_dec_scan_next_au(ctx, &offset); // eof, success, eos, error...
    if(ret == RET_EOF) {
        ret = xlnx_dec_scan_next_au(ctx, &offset);
    }
    if(ret == RET_EOS) {
        ret = xlnx_dec_send_null_data(ctx);
    } else {
        ret = xlnx_dec_send_data(ctx, offset);
    }
    if(ret == XMA_ERROR) {
        DECODER_APP_LOG_ERROR("Unable to send data to decoder plugin.\n");
        return XMA_ERROR;
    }
    ret = xma_dec_session_recv_frame(ctx->xma_dec_session, 
                                     ctx->channel_ctx.xframe);
    if(ret == XMA_SUCCESS && (ctx->num_frames_decoded < 
       ctx->channel_ctx.num_frames_to_decode)) {
        
        ctx->num_frames_decoded++;
        xlnx_dec_print_segment_performance(ctx);
        /* we have recieved a decoded frame, write it to the output file */
        dec_export_to_file(ctx);
    } else {
        usleep(5);
    }
    return ret;
}

/*------------------------------------------------------------------------------
dec_decode_frame: Decode a file

Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
static void dec_decode_file(XlnxDecoderCtx* ctx)
{
    int ret = XMA_SUCCESS;
    dec_set_signal_handler();
    xlnx_utils_set_non_blocking(1);
    xlnx_utils_start_tracking_time(&ctx->timer);
    while(ret != XMA_EOS && ret != XMA_ERROR) {
        if(xlnx_utils_was_q_pressed() || signal_caught ||
           ctx->num_frames_decoded >= ctx->channel_ctx.num_frames_to_decode) {
            
            ctx->input_ctx.input_file_info.loop = 0;
            lseek (ctx->input_ctx.input_file_info.in_file, 0, SEEK_END);
            ctx->input_ctx.input_file_info.input_buffer.size = 0;
        }
        ret = dec_decode_frame(ctx);
    }
    fprintf(stderr, "\n");
    xlnx_utils_set_non_blocking(0);
    DECODER_APP_LOG_INFO("\nDecode session done\n");
}

/*------------------------------------------------------------------------------
main: Main function for xma decoder app
Parameters:
    argc: Number of arguments
    argv: Pointer to the arguments
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int main(int argc, char* argv[])
{
    XlnxDecoderArguments arguments = xlnx_dec_get_arguments(argc, argv);
    XlnxDecoderCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    xlnx_dec_create_decoder_context(arguments, &ctx);
    /* Load xclbin */
    if(xlnx_dec_fpga_init(&ctx) != DEC_APP_SUCCESS) {
        xlnx_dec_cleanup_ctx(&ctx);
        exit(DEC_APP_ERROR);
    }
    ctx.xma_dec_session = xma_dec_session_create(&ctx.dec_xma_props);
    if(!ctx.xma_dec_session) {
        DECODER_APP_LOG_ERROR("Failed to create decoder session\n");
        xlnx_dec_cleanup_ctx(&ctx);
        exit(DEC_APP_ERROR);
    }
    /* Run Decoder */
    dec_decode_file(&ctx);
    xlnx_dec_print_total_performance(&ctx);
    xlnx_dec_cleanup_ctx(&ctx);
    return DEC_APP_SUCCESS;
}
