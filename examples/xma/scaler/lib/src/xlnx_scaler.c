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

#include "xlnx_scaler.h"
#include "xlnx_scal_xrm_interface.h"

/**
 * xlnx_scal_cleanup_scaler_ctx: Cleanup the scaler context - free xma frames,
 * destroy scaler session, free xrm resources.
 *
 * @param ctx: The scaler context
 */
void xlnx_scal_cleanup_scaler_ctx(XlnxScalerCtx* ctx)
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
    for(uint i = 0; i < ctx->abr_params.num_outputs[0]; i++) {
        if(ctx->output_xframe_list[i]) {
            xma_frame_free(ctx->output_xframe_list[i]);
        }
    }
    if(ctx->input_xframe) {
        xma_frame_free(ctx->input_xframe);
    }
}

/**
 * xlnx_scal_fpga_init: Get/allocate xrm resources, xma initialize.
 *
 * @param ctx: The scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_fpga_init(XlnxScalerCtx* ctx)
{
    // Reserve xrm resource and xma initialize
    int32_t ret = xlnx_scal_reserve_xrm_resource(&ctx->scaler_xrm_ctx,
                                                 &ctx->abr_xma_props[0]);
    if(ret != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    ret = xlnx_scal_alloc_xrm_cu(&ctx->scaler_xrm_ctx, &ctx->abr_xma_props[0]);
    if(ret != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    if(ctx->abr_params.is_halfrate_used) {
        ctx->abr_xma_props[1].plugin_lib = ctx->abr_xma_props[0].plugin_lib;
        ctx->abr_xma_props[1].dev_index  = ctx->abr_xma_props[0].dev_index;
        /* XMA to select the ddr bank based on xclbin meta data */
        ctx->abr_xma_props[1].ddr_bank_index =
            ctx->abr_xma_props[0].ddr_bank_index;
        ctx->abr_xma_props[1].cu_index   = ctx->abr_xma_props[0].cu_index;
        ctx->abr_xma_props[1].channel_id = ctx->abr_xma_props[0].channel_id;
    }

    return ret;
}

/**
 * xlnx_scal_create_xframes: Allocate the xframes for input/output
 *
 * @param ctx: The scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error.
 */
static int xlnx_scal_create_xframes(XlnxScalerCtx* ctx)
{
    XmaScalerProperties props = ctx->abr_xma_props[0];
    XmaFrameProperties  fprops;
    // Create an input frame for abr scaler
    fprops.width          = props.input.width;
    fprops.height         = props.input.height;
    fprops.format         = props.input.format;
    fprops.bits_per_pixel = props.input.bits_per_pixel;
    fprops.linesize[0] =
        xlnx_utils_get_valid_bytes_in_line(fprops.bits_per_pixel, fprops.width);
    fprops.linesize[1] = fprops.linesize[0];
    ctx->input_xframe  = xlnx_scal_utils_alloc_frame(&fprops);

    // Create an array of output frames for abr scaler
    for(uint output_id = 0; output_id < ctx->abr_params.num_outputs[0];
        output_id++) {
        fprops.width =
            XLNX_ALIGN(props.output[output_id].width, SCAL_OUT_STRIDE_ALIGN);
        fprops.height =
            XLNX_ALIGN(props.output[output_id].height, SCAL_OUT_HEIGHT_ALIGN);
        fprops.format         = props.output[output_id].format;
        fprops.bits_per_pixel = props.output[output_id].bits_per_pixel;
        fprops.linesize[0] =
            XLNX_ALIGN(xlnx_utils_get_valid_bytes_in_line(fprops.bits_per_pixel,
                                                          fprops.width),
                       SCAL_OUT_STRIDE_ALIGN);
        fprops.linesize[1] = fprops.linesize[0];

        ctx->output_xframe_list[output_id] =
            xlnx_scal_utils_alloc_frame(&fprops);
        if(!ctx->output_xframe_list[output_id]) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Failed to allocate output xma frame %d\n", output_id);
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_create_scaler_ctx: Uses ctx->abr_params to create the rest of the
 * scaler context. Does not init the fpga/create session.
 * @param ctx: A pointer to the scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_create_scaler_ctx(XlnxScalerCtx* ctx)
{
    int ret = XMA_APP_ERROR;
    if(ctx->abr_params.is_halfrate_used > 0) {
        ctx->num_sessions = 2;
    } else {
        ctx->num_sessions = 1;
    }
    for(int session_id = 0; session_id < ctx->num_sessions; session_id++) {
        ret = xlnx_scal_create_xma_props(&ctx->abr_params, session_id,
                                         &ctx->abr_xma_props[session_id]);
        if(ret != XMA_APP_SUCCESS) {
            return ret;
        }
    }
    // Creates ctx->input_ctx->xframe and ctx->output_ctx.xframe_list
    ret = xlnx_scal_create_xframes(ctx);
    return ret;
}

/**
 * xlnx_scal_create_scaler_sessions: Creates the scaler sessions using the
 * scaler props. One session for all rates, another for full rate only (If there
 * are half rate sessions specified.)
 * @param ctx: A pointer to the scaler context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_create_scaler_sessions(XlnxScalerCtx* ctx)
{
    // Create an abr scaler session based on the requested properties
    for(int session_id = 0; session_id < ctx->num_sessions; session_id++) {
        ctx->session[session_id] =
            xma_scaler_session_create(&ctx->abr_xma_props[session_id]);
        if(!ctx->session[session_id]) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Failed to create scaler session\n");
            return XMA_APP_ERROR;
        }
        /* Send first session handle to next session */
        ctx->abr_params.mixrate_session_as_uint64 =
            (uint64_t)ctx->session[session_id];
    }
    return XMA_APP_SUCCESS;
}
