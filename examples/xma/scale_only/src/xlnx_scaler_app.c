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
#include "xlnx_app_utils.h"
#include "xlnx_scal_common.h"

int signal_caught;

/*-----------------------------------------------------------------------------
xlnx_scal_signal_handler: Signal handler function
Parameters:
_signum: Signal number
-----------------------------------------------------------------------------*/
static void xlnx_scal_signal_handler(int32_t signum)
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
xlnx_scal_set_signal_handler: Signal handler initialization.
Return:
SCALER_APP_SUCCESS or APP_FAILURE
------------------------------------------------------------------------------*/
static int xlnx_scal_set_signal_handler()
{
    signal_caught = 0;
    struct sigaction action;
    action.sa_handler = xlnx_scal_signal_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGHUP,  &action, NULL);
    sigaction(SIGINT,  &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_write_output_frame: Write the output frame for an output

Parameters:
    ctx: The scaler context
    output_id: The output index for the xframelist
Return:
    SCALER_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int xlnx_scal_write_output_frame(XlnxScalerCtx* ctx, int output_id, 
                                        int xframe_id) {
    XlnxScalerOutputCtx* output_ctx = &ctx->output_ctx;
    XlnxScalerProps*     abr_params = &ctx->abr_params;
    int num_planes = xma_frame_planes_get(&output_ctx->xframe_list[xframe_id]->
                                          frame_props);
    int frame_stride;
    uint8_t* frame_buffer;
    int out_height = abr_params->output_heights[output_id];
    for(int plane_id = 0; plane_id < num_planes; plane_id++) {
        frame_stride = output_ctx->xframe_list[xframe_id]->frame_props.
                       linesize[plane_id];
        if(plane_id > 0) {
            out_height = abr_params->output_heights[output_id] / 2;
        }
        frame_buffer = output_ctx->xframe_list[xframe_id]->data[plane_id].
                       buffer;
        for(int i = 0; i < out_height; i++)
        {
            fwrite(frame_buffer, 1, abr_params->output_widths[output_id], 
                   output_ctx->out_fp[output_id]);
            frame_buffer += frame_stride;
        }
    }
    fflush(output_ctx->out_fp[output_id]);
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_write_output_frames: Write all of the output frames for a session

Parameters:
    ctx: The scaler context
    session_id: 0 is all rate, 1 is full rate only.
Return:
    SCALER_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int xlnx_scal_write_output_frames(XlnxScalerCtx* ctx, int session_id) {
    int xframe_id = 0;
    for(int output_id = 0; output_id < ctx->abr_params.num_outputs[0]; 
        output_id++) {
        /* If this is full rate only session and the current output is half 
        rate, then skip it. */
        if(session_id != 0 && ctx->abr_params.is_halfrate[output_id]) {
            continue;
        }
        xlnx_scal_write_output_frame(ctx, output_id, xframe_id);
        xframe_id++;
    }
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_scale_frame: Scale a frame

Parameters:
    ctx: The scaler context
    session_id: Tells whether to use session 0 (all rates) or session 1 
        (full rate)
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int xlnx_scal_scale_frame(XlnxScalerCtx* ctx, int session_id) {
    int send_rc, recv_rc;
    //send frame to abr scaler
    send_rc = xma_scaler_session_send_frame(ctx->session[session_id], 
                                            ctx->input_ctx.xframe);
    //receive frame from abr scaler
    if ((send_rc == XMA_SUCCESS) || (send_rc == XMA_FLUSH_AGAIN)) {
        recv_rc = xma_scaler_session_recv_frame_list(ctx->session[session_id], 
                                                   ctx->output_ctx.xframe_list);
        if (recv_rc != XMA_SUCCESS) {
            SCALER_APP_LOG_ERROR("Failed to receive frame list from XMA "
                                 "plugin\n");
            exit(SCALER_APP_ERROR); // TODO
        }
        xlnx_scal_write_output_frames(ctx, session_id);
        ctx->num_frames_scaled++;
        xlnx_scal_print_segment_performance(ctx);
    }
    return send_rc;
}

/*------------------------------------------------------------------------------
xlnx_scal_scale_file: Scale a file

Parameters:
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int xlnx_scal_scale_file(XlnxScalerCtx* ctx) {
    int send_rc             = XMA_SUCCESS;
    int num_frames_to_scale = ctx->output_ctx.num_frames_to_scale;
    xlnx_scal_set_signal_handler();
    xlnx_utils_set_non_blocking(1);
    xlnx_utils_start_tracking_time(&ctx->timer);  
    while(send_rc != XMA_EOS && send_rc != XMA_ERROR) {
        if(xlnx_utils_was_q_pressed() || signal_caught ||
           ctx->num_frames_scaled >= num_frames_to_scale) {
           
            ctx->input_ctx.loops_remaining  = 0;
            fseek(ctx->input_ctx.in_fp, 0, SEEK_END);
        }
        xlnx_scal_get_input_frame(ctx);
        if(ctx->num_sessions > 1 && ctx->num_frames_scaled % 2 != 0) {
            send_rc = xlnx_scal_scale_frame(ctx, 1);
        } else {
            send_rc = xlnx_scal_scale_frame(ctx, 0);
        }
    }
    printf("\n");
    xlnx_utils_set_non_blocking(0);
    SCALER_APP_INFO_LOG("\nScale session done\n");
    return SCALER_APP_SUCCESS;
}

int main(int argc, char* argv[])
{
    XlnxScalArguments  arguments = xlnx_scal_get_arguments(argc, argv);
    XlnxScalerCtx ctx;
    memset(&ctx, 0, sizeof(XlnxScalerCtx));
    xlnx_scal_create_context(arguments, &ctx);
    // Load xclbin
    if(xlnx_scal_fpga_init(&ctx) != SCALER_APP_SUCCESS) {
        xlnx_scal_cleanup_ctx(&ctx);
        exit(SCALER_APP_ERROR);
    }

    if(xlnx_scal_create_scaler_sessions(&ctx) != SCALER_APP_SUCCESS) {
        xlnx_scal_cleanup_ctx(&ctx);
        exit(SCALER_APP_ERROR);
    }

    // run abr scaler
    xlnx_scal_scale_file(&ctx);
    xlnx_scal_print_total_performance(&ctx);
    xlnx_scal_cleanup_ctx(&ctx);
    return 0;
}
