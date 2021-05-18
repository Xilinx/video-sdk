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

#include "xlnx_scaler.h"
#include "xlnx_scal_xrm_interface.h"

/*------------------------------------------------------------------------------
xlnx_scal_cleanup_input_ctx: Free xma frame and close the input file

Parameters:
    input_ctx: The scaler input context
------------------------------------------------------------------------------*/
static void xlnx_scal_cleanup_input_ctx(XlnxScalerInputCtx* input_ctx) 
{
    if(input_ctx->xframe) {
        xma_frame_free(input_ctx->xframe);
    }
    if(input_ctx->in_fp) {
        fclose(input_ctx->in_fp);
    }
}

/*------------------------------------------------------------------------------
xlnx_scal_cleanup_output_ctx: Free the output xma frames and close output files

Parameters:
    output_ctx: The scaler output context
    num_outputs: The number of xframes/output files there are
------------------------------------------------------------------------------*/
static void xlnx_scal_cleanup_output_ctx(XlnxScalerOutputCtx* output_ctx, 
                                         int num_outputs) 
{
    for(int i=0; i < num_outputs; i++) {
        if(output_ctx->xframe_list[i]) {
            xma_frame_free(output_ctx->xframe_list[i]);
        }
        if(output_ctx->out_fp[i]) {
            fclose(output_ctx->out_fp[i]);
        }
    }
}

/*------------------------------------------------------------------------------
xlnx_scal_cleanup_ctx: Cleanup the context - free resources, close files.

Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_cleanup_ctx(XlnxScalerCtx* ctx)
{
    if(!ctx) {
        return;
    }
    for(int session_id = 0; session_id < ctx->num_sessions; session_id++) {
        if(ctx->session[session_id]) {
            xma_scaler_session_destroy(ctx->session[session_id]);
        }
    }
    xlnx_scal_cleanup_xrm_ctx(&ctx->scaler_xrm_ctx);
    if(ctx->abr_params.is_halfrate_used) {
        xlnx_scal_cleanup_props(&ctx->abr_xma_props[1]);
    }
    xlnx_scal_cleanup_props(&ctx->abr_xma_props[0]);
    xlnx_scal_cleanup_output_ctx(&ctx->output_ctx, ctx->abr_params.
                                 num_outputs[0]);
    xlnx_scal_cleanup_input_ctx(&ctx->input_ctx);    
}

/*------------------------------------------------------------------------------
xlnx_scal_fpga_init: Get/allocate xrm resources, xma initialize.

Parameters:
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_fpga_init(XlnxScalerCtx* ctx) 
{
    // Reserve xrm resource and xma initialize
    int32_t ret = xlnx_scal_reserve_xrm_resource(&ctx->scaler_xrm_ctx, 
                                           &ctx->abr_xma_props[0]);
    if(ret != SCALER_APP_SUCCESS) {
        return SCALER_APP_ERROR;
    }
    if(ctx->abr_params.device_id == -1) {
        ret = xlnx_scal_alloc_xrm_scaler_cu(&ctx->scaler_xrm_ctx, 
                                         &ctx->abr_xma_props[0]);
    } else {
        ret = xlnx_scal_cu_alloc_device_id(&ctx->scaler_xrm_ctx, 
                                         &ctx->abr_xma_props[0]);
    }
     if(ctx->abr_params.is_halfrate_used) {
        ctx->abr_xma_props[1].plugin_lib     = ctx->abr_xma_props[0].
                                               plugin_lib;
        ctx->abr_xma_props[1].dev_index      = ctx->abr_xma_props[0].
                                               dev_index;
        /* XMA to select the ddr bank based on xclbin meta data */
        ctx->abr_xma_props[1].ddr_bank_index = ctx->abr_xma_props[0].
                                               ddr_bank_index;
        ctx->abr_xma_props[1].cu_index       = ctx->abr_xma_props[0].
                                               cu_index;
        ctx->abr_xma_props[1].channel_id     = ctx->abr_xma_props[0].
                                               channel_id;
    }

    return ret;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_input_ctx: Create the input context. Open the input file, 
    set loop count.

Parameters:
    arguments: The arguments 
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_create_input_ctx(XlnxScalArguments arguments,
                                          XlnxScalerCtx* ctx) 
{
    XlnxScalerInputCtx* input_ctx = &ctx->input_ctx;
    memset(input_ctx, 0, sizeof(XlnxScalerInputCtx));
    input_ctx->in_fp =  fopen(arguments.input_file, "rb");
    if(!input_ctx->in_fp) {
        SCALER_APP_INFO_LOG("Unable to open input file %s\n", 
                            arguments.input_file);
        exit(SCALER_APP_ERROR);
    }
    input_ctx->loops_remaining  = arguments.loop_count;
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_xframes: Allocate the xframes for input/output

Parameters:
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int xlnx_scal_create_xframes(XlnxScalerCtx* ctx) 
{
    XmaScalerProperties props = ctx->abr_xma_props[0];
    XmaFrameProperties fprops;
    // Create an input frame for abr scaler
    fprops.width            = props.input.width;
    fprops.height           = props.input.height;
    fprops.format           = props.input.format;
    fprops.bits_per_pixel   = props.input.bits_per_pixel;
    ctx->input_ctx.xframe  = xlnx_scal_utils_alloc_frame(&fprops);

    // Create an array of output frames for abr scaler
    for(int output_id = 0; output_id < ctx->abr_params.num_outputs[0]; 
        output_id++) {
        fprops.width            = ALIGN(props.output[output_id].width,  
                                        STRIDE_ALIGN);
        fprops.height           = ALIGN(props.output[output_id].height, 
                                        HEIGHT_ALIGN);
        fprops.format           = props.output[output_id].format;
        fprops.bits_per_pixel   = props.output[output_id].bits_per_pixel;
        ctx->output_ctx.xframe_list[output_id] = xlnx_scal_utils_alloc_frame(
                                                                       &fprops);
    }
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_output_ctx: Create the output context

Parameters:
    arguments: The arguments struct containing commandline info
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_create_output_ctx(XlnxScalArguments arguments, 
                                           XlnxScalerCtx* ctx) 
{
    XlnxScalerOutputCtx* output_ctx = &ctx->output_ctx;
    XlnxScalerProps*     param_ctx  = &ctx->abr_params;
    memset(output_ctx, 0, sizeof(XlnxScalerOutputCtx));
    output_ctx->num_frames_to_scale   = arguments.num_frames;
    XlnxScalOutArgs out_args;
    for(int output_id = 0; output_id < param_ctx->num_outputs[0]; output_id++) {
        out_args = arguments.out_arg_list[output_id];
        output_ctx->out_fp[output_id] =  fopen(out_args.output_file, "wb");
        if(!output_ctx->out_fp[output_id]) {
            SCALER_APP_LOG_ERROR("Unable to open output file %s\n", 
                                 out_args.output_file);
            exit(SCALER_APP_ERROR);
        }
    }
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_xma_param_ctx: Create the param ctx for xma props.

Parameters:
    arguments: The arguments struct containing commandline info
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_create_xma_param_ctx(XlnxScalArguments arguments, 
                                              XlnxScalerProps* param_ctx) 
{
    // Setup custom xma plugin params
    param_ctx->device_id            = arguments.device_id;
    param_ctx->log_level            = arguments.log_level;
    param_ctx->enable_pipeline      = arguments.enable_pipeline;
    param_ctx->latency_logging      = arguments.enable_latency_logging;

    param_ctx->input_pix_fmt        = arguments.pix_fmt;
    param_ctx->input_bits_per_pixel = 8;
    param_ctx->input_width          = arguments.input_width;
    param_ctx->input_height         = arguments.input_height;
    param_ctx->input_stride         = arguments.input_width;
    param_ctx->input_fps_num        = arguments.fps_num;
    param_ctx->input_fps_den        = arguments.fps_den;
    //assign output params

    param_ctx->num_outputs[0]        = arguments.outputs_used;
    param_ctx->num_outputs[1]        = arguments.num_fullrate_outputs;
    param_ctx->is_halfrate_used       = arguments.num_halfrate_outputs > 0;
    XlnxScalOutArgs out_args;
    for(int output_id = 0; output_id < param_ctx->num_outputs[0]; output_id++) {
        out_args = arguments.out_arg_list[output_id];
        param_ctx->output_widths[output_id]          = out_args.width;
        param_ctx->output_heights[output_id]         = out_args.height; 
        param_ctx->output_strides[output_id]         = out_args.width;
        param_ctx->output_bits_per_pixels[output_id] = 8;
        /* XMA_VCU_NV12_FMT_TYPE format only  */
        param_ctx->output_pix_fmts[output_id]        = arguments.pix_fmt; 
        param_ctx->is_halfrate[output_id]            = out_args.is_halfrate;
        param_ctx->coeff_loads[output_id]            = out_args.coeff_load;
    }
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_context: Creates the context based on user arguments. It 
    parses the header of the input file to get relevant codec info. This does 
    not create the xma session. Nor does it initialize the fpga.
Parameters:
    arguments: The argument struct containing scaler param, input, output file 
    info
    ctx: A pointer to the scaler context
Return:
    SCALER_APP_SUCCESS on success
------------------------------------------------------------------------------*/
int32_t xlnx_scal_create_context(XlnxScalArguments arguments, 
                                  XlnxScalerCtx* ctx)
{
    xlnx_scal_create_xma_param_ctx(arguments, &ctx->abr_params);
    xlnx_scal_create_input_ctx(arguments, ctx);
    xlnx_scal_create_output_ctx(arguments, ctx); 
    if(ctx->abr_params.is_halfrate_used > 0) {
        ctx->num_sessions = 2;
    } else {
        ctx->num_sessions = 1;
    }
    for(int session_id = 0; session_id < ctx->num_sessions; session_id++)
    {
        xlnx_scal_create_xma_props(&ctx->abr_params, session_id, 
                                    &ctx->abr_xma_props[session_id]);
    }
    // Creates ctx->input_ctx->xframe and ctx->output_ctx.xframe_list
    xlnx_scal_create_xframes(ctx); 
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_create_scaler_sessions: Creates the scaler sessions using the scaler
    props. One session for all rates, another for full rate only (If there are
    half rate sessions specified.)
Parameters:
    ctx: A pointer to the scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_create_scaler_sessions(XlnxScalerCtx* ctx)
{
    // Create an abr scaler session based on the requested properties
    for(int session_id = 0; session_id < ctx->num_sessions; session_id++) {
        ctx->session[session_id] = xma_scaler_session_create(
                                              &ctx->abr_xma_props[session_id]);
        if (!ctx->session[session_id]) {
            SCALER_APP_LOG_ERROR( "Failed to create scaler session\n");
            return SCALER_APP_ERROR;
        }
        /* Send first session handle to next session */
        ctx->abr_params.mixrate_session_as_uint64 = 
                                            (uint64_t)ctx->session[session_id]; 
    }
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_get_next_input_frame: Read an input frame from the input file into
    the input xframe

Parameters:
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_get_next_input_frame(XlnxScalerCtx* ctx)
{
    FILE*                in_fp  =  ctx->input_ctx.in_fp;
    XmaFrame*            xframe =  ctx->input_ctx.xframe;
    XmaScalerProperties* props  = &ctx->abr_xma_props[0];
    int    fread_rc = -1;
    int    num_planes;
    size_t size;
    XlnxAbrPixFmtDescription fmtdesc = xlnx_scal_utils_get_format_description(
                                                    xframe->frame_props.format);
    num_planes = xma_frame_planes_get(&xframe->frame_props);

    //Read next raw input
    for(int plane_id = 0; plane_id < num_planes; plane_id++) {
        size     = xlnx_scal_utils_get_plane_size(props->input.width, 
                                            props->input.height, 
                                            props->input.format, plane_id);
        fread_rc = fread(xframe->data[plane_id].buffer, fmtdesc.BytesPerPlane,
                         size, in_fp);
        /* Failed to get any input from file. Error */
        if (fread_rc <= 0 && ctx->num_frames_scaled == 0) {
            SCALER_APP_INFO_LOG("Unable to read input frame - plane %d\n", 
                                plane_id);
            exit(SCALER_APP_ERROR);
        /* Reached end of file. Set to null */
        }
        if(fread_rc > 0 && fread_rc < size) {
            xma_logmsg(XMA_WARNING_LOG, XLNX_SCALER_APP_MODULE, 
            "Corrupted packet found trying to read input file! Read %d bytes, "
            "but wanted %d!\n", fread_rc, size);
            /* Pretend we reached end of file cleanly. */
            return RET_EOF;
        }
        if(fread_rc <= 0) {
            return RET_EOF;
        /* Only increment after the first frame */
        } else if (plane_id == PLANE_Y && ctx->num_frames_scaled > 0) {
            ctx->pts++;
        }
    }
    return RET_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_get_input_frame: Read an input frame into the xframe buffer
Parameters:
    ctx: A pointer to the scaler context
        in_fp: ctx->input_ctx.in_fp
        xframe: ctx->input_ctx.xframe
        props: ctx->arb_xma_props
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_get_input_frame(XlnxScalerCtx* ctx)
{
    int ret = xlnx_scal_get_next_input_frame(ctx);
    if(ret == RET_EOF) {
        if(ctx->input_ctx.loops_remaining > 0 || 
           ctx->input_ctx.loops_remaining == -1) {
            ret = fseek(ctx->input_ctx.in_fp, 0, SEEK_SET);
            if(ctx->input_ctx.loops_remaining != -1) {
                ctx->input_ctx.loops_remaining--;
            }
            ret = xlnx_scal_get_next_input_frame(ctx);
        }
    }
    if(ret == RET_EOF) {
        int num_planes = xma_frame_planes_get(&ctx->input_ctx.xframe->
                                              frame_props);
        for(int plane_id = 0; plane_id < num_planes; plane_id++) {
            free(ctx->input_ctx.xframe->data[plane_id].buffer);
            ctx->input_ctx.xframe->data[plane_id].buffer = NULL;
        }
    }
    return ret;
}

/*------------------------------------------------------------------------------
xlnx_scal_print_segment_performance: Print the performance since the last 
    segment.
Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_print_segment_performance(XlnxScalerCtx* ctx)
{
    double time_since_last_segment = xlnx_utils_get_segment_time(&ctx->timer);
    if(time_since_last_segment < 0.5) {
        return;
    }
    fprintf(stderr, "\rFrame=%5zu Total FPS=%.03f Current FPS=%.03f\r", 
            ctx->num_frames_scaled, 
            (float) ctx->num_frames_scaled / 
            xlnx_utils_get_total_time(&ctx->timer), (ctx->num_frames_scaled - 
            ctx->timer.last_displayed_frame) / time_since_last_segment);
    fflush(stderr);
    ctx->timer.last_displayed_frame = ctx->num_frames_scaled;
    xlnx_utils_set_segment_time(&ctx->timer);
}

/*------------------------------------------------------------------------------
xlnx_scal_print_total_performance: Print the total performance since tracking
    began.
Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_print_total_performance(XlnxScalerCtx* ctx)
{
    double realtime_taken = xlnx_utils_get_total_time(&ctx->timer);
    fprintf(stderr, "\nFrames Scaled: %zu, Time Elapsed: %.03lf\r\n", 
            ctx->num_frames_scaled, realtime_taken);
    fprintf(stderr, "Real Time FPS: %.03lf\r\n", 
            ctx->num_frames_scaled / realtime_taken);
}

