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

#include "xlnx_scaler_app.h"

int signal_caught;

/**
 * xlnx_scal_signal_handler: Signal handler function
 * @param _signum: Signal number
 */
static void xlnx_scal_signal_handler(int32_t signum)
{
    switch(signum) {
        case SIGHUP:
        case SIGINT:
        case SIGQUIT:
        case SIGABRT:
        case SIGTERM:
            signal_caught = 1;
            break;
    }
}

/**
 * xlnx_scal_set_signal_handler: Signal handler initialization.
 * @return XMA_APP_SUCCESS or APP_FAILURE
 */
static int xlnx_scal_set_signal_handler()
{
    signal_caught = 0;
    struct sigaction action;
    action.sa_handler = xlnx_scal_signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGHUP, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_cleanup_ctx: Cleanup the context - free resources, close files.
 *
 * @param ctx: The scaler context
 */
void xlnx_scal_cleanup_ctx(XlnxScalerAppCtx* ctx)
{
    if(ctx->in_fp) {
        fclose(ctx->in_fp);
    }
    int num_outputs = ctx->scaler_ctx.abr_params.num_outputs[0];
    for(int i = 0; i < num_outputs; i++) {
        if(ctx->out_fp[i]) {
            fclose(ctx->out_fp[i]);
        }
    }
    xlnx_scal_cleanup_scaler_ctx(&ctx->scaler_ctx);
}

/**
 * xlnx_scal_create_output_ctx: Open output files and set pix fmt
 * @param arguments: The arguments struct containing commandline info
 * @param ctx: The scaler app ctx
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_scal_set_output_params(XlnxScalArguments arguments,
                                           XlnxScalerAppCtx* ctx)
{
    int             num_outputs = ctx->scaler_ctx.abr_params.num_outputs[0];
    XlnxScalOutArgs out_args;
    for(int output_id = 0; output_id < num_outputs; output_id++) {
        out_args               = arguments.out_arg_list[output_id];
        ctx->out_fp[output_id] = fopen(out_args.output_file, "wb");
        if(!ctx->out_fp[output_id]) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Unable to open output file %s\n", out_args.output_file);
            return XMA_APP_ERROR;
        }
        ctx->out_pix_fmt[output_id] = out_args.pix_fmt;
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_create_xma_param_ctx: Create the param ctx for xma props.
 *
 * @param arguments: The arguments struct containing commandline info
 * @param ctx: The scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
static int32_t xlnx_scal_create_xma_param_ctx(XlnxScalArguments arguments,
                                              XlnxScalerProps*  param_ctx)
{
    // Setup custom xma plugin params
    param_ctx->device_id       = arguments.device_id;
    param_ctx->log_level       = arguments.log_level;
    param_ctx->enable_pipeline = arguments.enable_pipeline;
    param_ctx->latency_logging = arguments.enable_latency_logging;
    param_ctx->input_bits_per_pixel =
        xlnx_utils_get_bit_depth_app_fmt(arguments.input_pix_fmt);
    param_ctx->input_pix_fmt =
        xlnx_utils_get_xma_vcu_format(param_ctx->input_bits_per_pixel);
    param_ctx->input_width  = arguments.input_width;
    param_ctx->input_height = arguments.input_height;
    param_ctx->input_stride = xlnx_utils_get_valid_bytes_in_line(
        param_ctx->input_bits_per_pixel, arguments.input_width);
    param_ctx->input_fps_num = arguments.fps_num;
    param_ctx->input_fps_den = arguments.fps_den;
    // assign output params

    param_ctx->num_outputs[0]   = arguments.outputs_used;
    param_ctx->num_outputs[1]   = arguments.num_fullrate_outputs;
    param_ctx->is_halfrate_used = arguments.num_halfrate_outputs > 0;
    XlnxScalOutArgs out_args;
    for(uint output_id = 0; output_id < param_ctx->num_outputs[0];
        output_id++) {
        out_args = arguments.out_arg_list[output_id];
        param_ctx->output_bits_per_pixels[output_id] =
            param_ctx->input_bits_per_pixel;
        param_ctx->output_pix_fmts[output_id] = param_ctx->input_pix_fmt;
        param_ctx->output_widths[output_id]   = out_args.width;
        param_ctx->output_heights[output_id]  = out_args.height;
        param_ctx->output_strides[output_id] =
            xlnx_utils_get_valid_bytes_in_line(
                param_ctx->output_bits_per_pixels[output_id], out_args.width);
        param_ctx->is_halfrate[output_id] = out_args.is_halfrate;
        param_ctx->coeff_loads[output_id] = out_args.coeff_load;
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_create_context: Creates the context based on user arguments. It
 * parses the header of the input file to get relevant codec info. This does
 * not create the xma session. Nor does it initialize the fpga.
 * @param arguments: The argument struct containing scaler param, input, output
 * file
 * @param info
 * @param ctx: A pointer to the scaler context
 * @return XMA_APP_SUCCESS on success
 */
static int32_t xlnx_scal_create_context(XlnxScalArguments arguments,
                                        XlnxScalerAppCtx* ctx)
{
    ctx->loops_remaining     = arguments.loop_count;
    ctx->num_frames_to_scale = arguments.num_frames;
    ctx->in_fp               = fopen(arguments.input_file, "rb");
    if(!ctx->in_fp) {
        xma_logmsg(XMA_INFO_LOG, XLNX_SCALER_APP_MODULE,
                   "Unable to open input file %s\n", arguments.input_file);
        return XMA_APP_ERROR;
    }
    ctx->in_pix_fmt = arguments.input_pix_fmt;
    xlnx_scal_create_xma_param_ctx(arguments, &ctx->scaler_ctx.abr_params);
    /* Open output files, set output pix fmts */
    if(xlnx_scal_set_output_params(arguments, ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    if(xlnx_scal_create_scaler_ctx(&ctx->scaler_ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_get_input_frame: Read an input frame into the xframe buffer
 * @param ctx: A pointer to the scaler context
 * @param in_fp: ctx->input_ctx.in_fp
 * @param xframe: ctx->input_ctx.xframe
 * @param props: ctx->arb_xma_props
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
static int32_t xlnx_scal_get_input_frame(XlnxScalerAppCtx* ctx)
{
    XmaFrame*           input_xframe = ctx->scaler_ctx.input_xframe;
    XmaFrameProperties* fprops       = &input_xframe->frame_props;
    int                 ret          = xlnx_utils_read_frame(
        fprops->width, fprops->height, ctx->in_pix_fmt, ctx->in_fp,
        &input_xframe->data[0].buffer, &input_xframe->data[1].buffer);
    if(ret == XMA_APP_EOF) {
        int loops_remaining = ctx->loops_remaining;
        if(loops_remaining > 0 || loops_remaining == -1) {
            fseek(ctx->in_fp, 0, SEEK_SET);
            if(loops_remaining != -1) {
                ctx->loops_remaining--;
            }
            ret = xlnx_utils_read_frame(
                fprops->width, fprops->height, ctx->in_pix_fmt, ctx->in_fp,
                &input_xframe->data[0].buffer, &input_xframe->data[1].buffer);
        }
    }
    if(ret == XMA_APP_EOF) {
        int num_planes = xma_frame_planes_get(&input_xframe->frame_props);
        for(int plane_id = 0; plane_id < num_planes; plane_id++) {
            free(input_xframe->data[plane_id].buffer);
            input_xframe->data[plane_id].buffer = NULL;
        }
    }
    return ret;
}

/**
 * xlnx_scal_print_segment_performance: Print the performance since the last
 * segment.
 * @param ctx: The scaler context
 */
static void xlnx_scal_print_segment_performance(XlnxScalerAppCtx* ctx)
{
    double time_since_last_segment = xlnx_utils_get_segment_time(&ctx->timer);
    if(time_since_last_segment < 0.5) {
        return;
    }
    fprintf(stderr, "\rFrame=%5zu Total FPS=%.03f Current FPS=%.03f\r",
            ctx->num_frames_scaled,
            (float)ctx->num_frames_scaled /
                xlnx_utils_get_total_time(&ctx->timer),
            (ctx->num_frames_scaled - ctx->timer.last_displayed_frame) /
                time_since_last_segment);
    fflush(stderr);
    ctx->timer.last_displayed_frame = ctx->num_frames_scaled;
    xlnx_utils_set_segment_time(&ctx->timer);
}

/**
 * xlnx_scal_print_total_performance: Print the total performance since tracking
 * began.
 * @param ctx: The scaler context
 */
static void xlnx_scal_print_total_performance(XlnxScalerAppCtx* ctx)
{
    double realtime_taken = xlnx_utils_get_total_time(&ctx->timer);
    fprintf(stderr, "\nFrames Scaled: %zu, Time Elapsed: %.03lf\r\n",
            ctx->num_frames_scaled, realtime_taken);
    fprintf(stderr, "Real Time FPS: %.03lf\r\n",
            ctx->num_frames_scaled / realtime_taken);
}

/**
 * Write the output frame for an output
 * @param ctx The scaler app context
 * @param output_id The output index for the xframelist
 * @return XMA_APP_SUCCESS on success
 */
static int xlnx_scal_write_output_frame(XlnxScalerAppCtx* ctx, int output_id,
                                        int xframe_id)
{
    XlnxScalerProps* abr_params = &ctx->scaler_ctx.abr_params;
    XmaFrame* output_xframe     = ctx->scaler_ctx.output_xframe_list[xframe_id];
    void*     y_buffer          = output_xframe->data[0].buffer;
    void*     uv_buffer         = output_xframe->data[1].buffer;
    FILE*     file              = ctx->out_fp[output_id];
    uint16_t  out_width         = abr_params->output_widths[output_id];
    uint16_t  out_height        = abr_params->output_heights[output_id];
    uint32_t  valid_bytes_in_line = xlnx_utils_get_valid_bytes_in_line(
        abr_params->input_bits_per_pixel, out_width);
    /* total_bytes_in_line == output_xframe->frame_props.linesize[0] */
    uint16_t total_bytes_in_line =
        XLNX_ALIGN(valid_bytes_in_line, SCAL_OUT_STRIDE_ALIGN);
    switch(ctx->out_pix_fmt[output_id]) { // Write yuv buffer out to file
        case XMA_APP_YUV420P_FMT_TYPE:
            xlnx_utils_nv12_to_yuv420p(out_width, out_height,
                                       total_bytes_in_line, y_buffer, uv_buffer,
                                       file);
            break;
        case XMA_APP_YUV420P10LE_FMT_TYPE:
            xlnx_utils_xv15_to_yuv420p10le(
                out_width, out_height, total_bytes_in_line, (uint32_t*)y_buffer,
                (uint32_t*)uv_buffer, file);
            break;
        case XMA_APP_NV12_FMT_TYPE:
        case XMA_APP_XV15_FMT_TYPE:
            xlnx_utils_remove_vcu_padding(valid_bytes_in_line,
                                          total_bytes_in_line, out_height,
                                          y_buffer, uv_buffer, file);
            break;
        default:
            assert(!"Unformatted pixel format passed in!");
    }
    return XMA_APP_SUCCESS;
}

/**
 * Write all of the output frames for a session
 * @param ctx The scaler context
 * @param session_id 0 is all rate, 1 is full rate only.
 * @return XMA_APP_SUCCESS on success
 */
static int xlnx_scal_write_output_frames(XlnxScalerAppCtx* ctx, int session_id)
{
    int xframe_id   = 0;
    int num_outputs = ctx->scaler_ctx.abr_params.num_outputs[0];
    for(int output_id = 0; output_id < num_outputs; output_id++) {
        /* If this is full rate only session and the current output is half
        rate, then skip it. */
        if(session_id != 0 &&
           ctx->scaler_ctx.abr_params.is_halfrate[output_id]) {

            continue;
        }
        xlnx_scal_write_output_frame(ctx, output_id, xframe_id);
        xframe_id++;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Scale a frame
 * @param ctx The scaler app context
 * @param session_id The id of the session; used to track mixrate
 * @return The result of sending the frame, or xma error if recv is not
 * successful.
 */
static int xlnx_scal_scale_frame(XlnxScalerAppCtx* ctx, int session_id)
{
    int send_rc, recv_rc;
    /* send frame to scaler */
    XlnxScalerCtx* scaler_ctx = &ctx->scaler_ctx;
    send_rc = xma_scaler_session_send_frame(scaler_ctx->session[session_id],
                                            scaler_ctx->input_xframe);
    /* receive frame from scaler */
    if((send_rc == XMA_SUCCESS) || (send_rc == XMA_FLUSH_AGAIN)) {
        recv_rc = xma_scaler_session_recv_frame_list(
            scaler_ctx->session[session_id], scaler_ctx->output_xframe_list);
        if(recv_rc != XMA_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Failed to receive frame list from XMA plugin\n");
            return XMA_ERROR;
        }
        if(ctx->num_frames_scaled < ctx->num_frames_to_scale) {
            xlnx_scal_write_output_frames(ctx, session_id);
        }
        ctx->num_frames_scaled++;
        xlnx_scal_print_segment_performance(ctx);
    }
    return send_rc;
}

/**
 * xlnx_scal_scale_file: Scale a file
 *
 * @param ctx: The scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
static int xlnx_scal_scale_file(XlnxScalerAppCtx* ctx)
{
    int send_rc = XMA_SUCCESS;
    xlnx_scal_set_signal_handler();
    xlnx_utils_set_non_blocking(1);
    xlnx_utils_start_tracking_time(&ctx->timer);
    while(send_rc != XMA_EOS && send_rc != XMA_ERROR) {
        if(xlnx_utils_was_q_pressed() || signal_caught ||
           ctx->num_frames_scaled >= ctx->num_frames_to_scale) {

            ctx->loops_remaining = 0;
            fseek(ctx->in_fp, 0, SEEK_END);
        }
        xlnx_scal_get_input_frame(ctx);
        if(ctx->scaler_ctx.num_sessions > 1 &&
           ctx->num_frames_scaled % 2 != 0) {

            send_rc = xlnx_scal_scale_frame(ctx, 1);
        } else {
            send_rc = xlnx_scal_scale_frame(ctx, 0);
        }
    }
    fprintf(stderr, "\n");
    xlnx_utils_set_non_blocking(0);
    xma_logmsg(XMA_INFO_LOG, XLNX_SCALER_APP_MODULE, "\nScale session done\n");
    return send_rc != XMA_APP_ERROR ? XMA_APP_SUCCESS : XMA_APP_ERROR;
}

/**
 * xlnx_scal_create_fpga_sessions: Create scaler sessions
 * @param ctx: The scaler context
 * @return XMA_APP_SUCCESS or APP_FAILURE
 */
static int32_t xlnx_scal_create_fpga_sessions(XlnxScalerAppCtx* ctx)
{
    XlnxScalerCtx* scaler_ctx = &ctx->scaler_ctx;
    // Load xclbin
    if(xlnx_scal_fpga_init(scaler_ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    if(xlnx_scal_create_scaler_sessions(scaler_ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * main: Main routine
 * @param argc: Arguments count
 * @param argv: Argument array
 * @return XMA_APP_SUCCESS or APP_FAILURE
 */
int main(int argc, char* argv[])
{
    XlnxScalArguments arguments = xlnx_scal_get_arguments(argc, argv);
    XlnxScalerAppCtx  ctx;
    memset(&ctx, 0, sizeof(XlnxScalerAppCtx));
    if(xlnx_scal_create_context(arguments, &ctx) != XMA_APP_SUCCESS) {
        xlnx_scal_cleanup_ctx(&ctx);
        exit(XMA_APP_ERROR);
    }
    if(xlnx_scal_create_fpga_sessions(&ctx) != XMA_APP_SUCCESS) {
        xlnx_scal_cleanup_ctx(&ctx);
        exit(XMA_APP_ERROR);
    }
    // run abr scaler
    xlnx_scal_scale_file(&ctx);
    xlnx_scal_print_total_performance(&ctx);
    xlnx_scal_cleanup_ctx(&ctx);
    return XMA_APP_SUCCESS;
}
