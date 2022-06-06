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

#ifndef _XLNX_SCAL_XMA_PROPS_H_
#define _XLNX_SCAL_XMA_PROPS_H_

#include <string.h>

#include <xma.h>
#include <xmaplugin.h>

#include "xlnx_app_utils.h"
#include "xlnx_scal_common.h"

typedef struct XlnxScalerProps {
    int32_t  device_id;
    int32_t  log_level;
    int32_t  enable_pipeline;
    int32_t  latency_logging;
    bool     is_halfrate_used;
    uint64_t mixrate_session_as_uint64;
    uint32_t num_outputs[2];

    XmaFormatType input_pix_fmt;
    int32_t       input_bits_per_pixel;
    int32_t       input_width;
    int32_t       input_height;
    int32_t       input_stride;
    int32_t       input_fps_num;
    int32_t       input_fps_den;

    bool          is_halfrate[MAX_SCALER_OUTPUTS];
    XmaFormatType output_pix_fmts[MAX_SCALER_OUTPUTS];
    int32_t       output_bits_per_pixels[MAX_SCALER_OUTPUTS];
    int32_t       output_widths[MAX_SCALER_OUTPUTS];
    int32_t       output_heights[MAX_SCALER_OUTPUTS];
    int32_t       output_strides[MAX_SCALER_OUTPUTS];
    int32_t       coeff_loads[MAX_SCALER_OUTPUTS];

} XlnxScalerProps;

/**
 * xlnx_scal_cleanup_props: Free resources allocated in the scaler props.
 *
 * @param scaler_xma_props: The xma scaler properties
 */
void xlnx_scal_cleanup_props(XmaScalerProperties* scaler_xma_props);

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
                                   XmaScalerProperties* scaler_xma_props);

#endif
