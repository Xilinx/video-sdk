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

#ifndef _XLNX_ENC_DYN_PARAMS_H_
#define _XLNX_ENC_DYN_PARAMS_H_

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <xma.h>

typedef void* DynparamsHandle;

/* Functions pointers for loading functions from dynamic params library */
typedef DynparamsHandle (*fp_xlnx_enc_get_dyn_params)(char*, uint32_t*);
typedef uint32_t (*fp_xlnx_enc_get_dyn_param_frame_num)(DynparamsHandle,
                                                        uint32_t);
typedef uint32_t (*fp_xlnx_enc_get_runtime_b_frames)(DynparamsHandle, uint32_t);
typedef void (*fp_xlnx_enc_reset_runtime_aq_params)(DynparamsHandle, uint32_t);
typedef int32_t (*fp_xlnx_enc_add_dyn_params)(DynparamsHandle, XmaFrame*,
                                              uint32_t);
typedef void (*fp_xlnx_enc_deinit_dyn_params)(
    DynparamsHandle dynamic_params_handle);

typedef struct XlnxDynParamsObj {
    fp_xlnx_enc_get_dyn_params          xlnx_enc_get_dyn_params;
    fp_xlnx_enc_get_dyn_param_frame_num xlnx_enc_get_dyn_param_frame_num;
    fp_xlnx_enc_get_runtime_b_frames    xlnx_enc_get_runtime_b_frames;
    fp_xlnx_enc_reset_runtime_aq_params xlnx_enc_reset_runtime_aq_params;
    fp_xlnx_enc_add_dyn_params          xlnx_enc_add_dyn_params;
    fp_xlnx_enc_deinit_dyn_params       xlnx_enc_deinit_dyn_params;
} XlnxDynParamsObj;

#define XLNX_DYN_PARAMS_PARSER     "xlnx_dyn_params_parser"
#define D_PARAM_MAX_LENGTH         256
#define D_PARAM_B_FRAMES           "NumB"
#define D_PARAM_BITRATE            "BR"
#define D_PARAM_T_AQ               "tAQ"
#define D_PARAM_S_AQ               "sAQ"
#define D_PARAM_S_AQ_GAIN          "sAQGain"
#define ENC_MIN_SPAT_AQ_GAIN       0
#define ENC_MAX_SPAT_AQ_GAIN       100
#define ENC_MIN_BITRATE            0
#define ENC_MAX_BITRATE            INT_MAX
#define ENC_MIN_BFRAMES            0
#define ENC_MAX_BFRAMES            7
#define ENC_MIN_TAQ                0
#define ENC_MAX_TAQ                1
#define ENC_MIN_SAQ                0
#define ENC_MAX_SAQ                1
#define ENC_RUNTIME_PARAMS_CHANGED 1

typedef struct XlnxLaDynParams {
    bool     is_spatial_gain_changed;
    uint32_t spatial_aq_gain;
    bool     is_temporal_mode_changed;
    bool     temporal_aq_mode;
    bool     is_spatial_mode_changed;
    bool     spatial_aq_mode;
} XlnxLaDynParams;

typedef struct XlnxEncDynParams {
    bool     is_bitrate_changed;
    uint32_t bit_rate;
    bool     is_bframes_changed;
    uint8_t  num_b_frames;
} XlnxEncDynParams;

typedef struct XlnxDynParams {
    XlnxEncDynParams enc_dyn_param;
    XlnxLaDynParams  la_dyn_param;
} XlnxDynParams;

typedef struct XlnxDynParamFrames {
    uint32_t      frame_num;
    XlnxDynParams dyn_params;
} XlnxDynParamFrames;

DynparamsHandle xlnx_enc_get_dyn_params(char*     dyn_params_config_file,
                                        uint32_t* dynamic_frames_count);

int32_t xlnx_enc_add_dyn_params(DynparamsHandle dynamic_params_handle,
                                XmaFrame* la_input_xframe, uint32_t index);

uint32_t xlnx_enc_get_dyn_param_frame_num(DynparamsHandle dynamic_params_handle,
                                          uint32_t        index);

uint32_t xlnx_enc_get_runtime_b_frames(DynparamsHandle dynamic_params_handle,
                                       uint32_t        index);

void xlnx_enc_reset_runtime_aq_params(DynparamsHandle dynamic_params_handle,
                                      uint32_t        index);

void xlnx_enc_init_dyn_params_obj(XlnxDynParamsObj* fp_dyn_params);

void xlnx_enc_deinit_dyn_params(DynparamsHandle dynamic_params_handle);

#endif // _XLNX_ENC_DYN_PARAMS_H_
