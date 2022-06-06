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

#include "xlnx_scal_xma_props.h"

/**
 * xlnx_scal_cleanup_props: Free resources allocated in the scaler props.
 *
 * @param scaler_xma_props: The xma scaler properties
 */
void xlnx_scal_cleanup_props(XmaScalerProperties* scaler_xma_props)
{
    if(scaler_xma_props->params) {
        free(scaler_xma_props->params);
    }
}

/**
 * xlnx_scal_create_params: Create the custom scaler params
 *
 * @param param_ctx: The parameter context containing scaler param values
 * @param session_id: The session id corresponding to the current session
 * @param scaler_xma_props: The scaler xma properties which will be filled
 * @return XMA_APP_SUCCESS on success
 */
static int32_t xlnx_scal_create_params(XlnxScalerProps*     param_ctx,
                                       XmaScalerProperties* scaler_xma_props)
{
    scaler_xma_props->params[0].name   = "logLevel";
    scaler_xma_props->params[0].type   = XMA_UINT32;
    scaler_xma_props->params[0].length = sizeof(param_ctx->log_level);
    scaler_xma_props->params[0].value  = &param_ctx->log_level;

    scaler_xma_props->params[1].name   = "latency_logging";
    scaler_xma_props->params[1].type   = XMA_UINT32;
    scaler_xma_props->params[1].length = sizeof(param_ctx->log_level);
    scaler_xma_props->params[1].value  = &param_ctx->latency_logging;

    scaler_xma_props->params[2].name   = "enable_pipeline";
    scaler_xma_props->params[2].type   = XMA_UINT32;
    scaler_xma_props->params[2].length = sizeof(param_ctx->enable_pipeline);
    scaler_xma_props->params[2].value  = &param_ctx->enable_pipeline;

    scaler_xma_props->params[3].name = "MixRate";
    scaler_xma_props->params[3].type = XMA_UINT64;
    scaler_xma_props->params[3].length =
        sizeof(param_ctx->mixrate_session_as_uint64);
    scaler_xma_props->params[3].value = &param_ctx->mixrate_session_as_uint64;
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_create_props: Fill the input/output fields of the xma scaler
 * props.
 *
 * @param param_ctx: The parameter context containing scaler param values
 * @param session_id: The session id corresponding to the current session
 * @param scaler_xma_props: The scaler xma properties which will be filled
 * @return XMA_APP_SUCCESS on success
 */
static int32_t xlnx_scal_create_props(XlnxScalerProps*     param_ctx,
                                      int                  session_id,
                                      XmaScalerProperties* scaler_xma_props)
{
    // setup frame poperties
    strcpy(scaler_xma_props->hwvendor_string, "Xilinx");
    scaler_xma_props->hwscaler_type        = XMA_POLYPHASE_SCALER_TYPE;
    scaler_xma_props->dev_index            = param_ctx->device_id;
    scaler_xma_props->num_outputs          = param_ctx->num_outputs[session_id];
    scaler_xma_props->input.format         = param_ctx->input_pix_fmt;
    scaler_xma_props->input.bits_per_pixel = param_ctx->input_bits_per_pixel;
    scaler_xma_props->input.width          = param_ctx->input_width;
    scaler_xma_props->input.height         = param_ctx->input_height;
    scaler_xma_props->input.stride         = param_ctx->input_width;
    scaler_xma_props->input.framerate.numerator   = param_ctx->input_fps_num;
    scaler_xma_props->input.framerate.denominator = param_ctx->input_fps_den;
    scaler_xma_props->param_cnt                   = MAX_SCALER_PARAM;
    scaler_xma_props->params =
        calloc(1, MAX_SCALER_PARAM * sizeof(XmaParameter));
    if(session_id > 0) {
        scaler_xma_props->input.framerate.numerator /= 2;
    }

    // assign output params
    int prop_index = 0;
    for(uint output_id = 0; output_id < param_ctx->num_outputs[0];
        output_id++) {
        if(param_ctx->is_halfrate[output_id] && session_id > 0) {
            /* We only want to add full rate outputs to the second session */
            continue;
        }
        scaler_xma_props->output[prop_index].format =
            param_ctx->output_pix_fmts[output_id];
        scaler_xma_props->output[prop_index].bits_per_pixel =
            param_ctx->output_bits_per_pixels[output_id];
        scaler_xma_props->output[prop_index].width =
            param_ctx->output_widths[output_id];
        scaler_xma_props->output[prop_index].height =
            param_ctx->output_heights[output_id];
        scaler_xma_props->output[prop_index].stride =
            param_ctx->output_widths[output_id];
        scaler_xma_props->output[prop_index].coeffLoad =
            param_ctx->coeff_loads[output_id];
        scaler_xma_props->output[prop_index].framerate.numerator =
            scaler_xma_props->input.framerate.numerator;
        scaler_xma_props->output[prop_index].framerate.denominator = 1;
        prop_index++;
    }
    for(uint i = 0; i < param_ctx->num_outputs[0]; i++) {
        if(scaler_xma_props->output[i].coeffLoad == 2) {
            strcpy(scaler_xma_props->input.coeffFile, "FilterCoef.txt");
            break;
        }
    }
    // Xrm related scaler props will be set in create_dec_xrm_ctx
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_create_xma_props: Create the xma scaler properties given the
 * param ctx and session id.
 *
 * @param param_ctx: The parameter context containing scaler param values
 * @param session_id: The session id corresponding to the current session
 * @param scaler_xma_props: The scaler xma properties which will be filled
 * @return XMA_APP_SUCCESS on success
 */
int32_t xlnx_scal_create_xma_props(XlnxScalerProps* param_ctx, int session_id,
                                   XmaScalerProperties* scaler_xma_props)
{
    // Setup custom xma plugin params
    xlnx_scal_create_props(param_ctx, session_id, scaler_xma_props);
    xlnx_scal_create_params(param_ctx, scaler_xma_props);
    return XMA_APP_SUCCESS;
}
