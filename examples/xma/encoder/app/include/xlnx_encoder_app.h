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

#ifndef _XLNX_ENCODER_APP_H_
#define _XLNX_ENCODER_APP_H_

#include "xlnx_enc_arg_parse.h"
#include "xlnx_encoder.h"
#include "xlnx_lookahead.h"

#define XLNX_ENC_INIT_DYN_PARAMS_OBJ "xlnx_enc_init_dyn_params_obj"

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

typedef void (*InitDynParams)(XlnxDynParamsObj*);

typedef struct EncDynParams {
    DynparamsHandle  dynamic_param_handle;
    uint32_t         dynamic_params_count;
    uint32_t         dynamic_params_index;
    void*            dyn_params_lib;
    XlnxDynParamsObj dyn_params_obj;
    InitDynParams    xlnx_enc_init_dyn_params_obj;
} EncDynParams;

typedef struct XlnxEncoderAppCtx {
    XmaFrame           la_input_xframe;
    XmaFrame*          enc_input_xframe; // enc_input_xframe == la output
    uint32_t           enc_state;
    XlnxEncoderCtx     enc_ctx;
    XlnxLookaheadCtx   la_ctx;
    EncDynParams       enc_dyn_params;
    FILE*              in_file;
    XlnxFormatType     pix_fmt;
    FILE*              out_file;
    size_t             num_frames_to_encode;
    int32_t            loop_count;
    bool               should_convert_input;
    XlnxAppTimeTracker timer;
} XlnxEncoderAppCtx;

#endif // _XLNX_ENCODER_APP_H_
