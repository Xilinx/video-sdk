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

#ifndef _XLNX_ENCODER_H_
#define _XLNX_ENCODER_H_

#include <dlfcn.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_app_utils.h"
#include "xlnx_lookahead.h"
#include "xlnx_transcoder_constants.h"
#include "xlnx_transcoder_xma_props.h"
#include "xlnx_transcoder_xrm_interface.h"
#include "xlnx_xvbm_utils.h"

#define FLAG_ENC_CODEC_TYPE      "c:v"
#define FLAG_ENC_DEVICE_ID       "dl"
#define FLAG_XVBM_CONVERT        "xvbm_convert"
#define FLAG_ENC_BITRATE         "b:v"
#define FLAG_ENC_BIT_RATE        "b"
#define FLAG_ENC_FPS             "fps"
#define FLAG_ENC_INTRA_PERIOD    "g"
#define FLAG_ENC_CONTROL_RATE    "control-rate"
#define FLAG_ENC_MAX_BITRATE     "max-bitrate"
#define FLAG_ENC_SLICE_QP        "slice-qp"
#define FLAG_ENC_MIN_QP          "min-qp"
#define FLAG_ENC_MAX_QP          "max-qp"
#define FLAG_ENC_NUM_BFRAMES     "bf"
#define FLAG_ENC_IDR_PERIOD      "periodicity-idr"
#define FLAG_DYN_IDR             "force_key_frame"
#define FLAG_ENC_PROFILE         "profile"
#define FLAG_ENC_LEVEL           "level"
#define FLAG_ENC_NUM_SLICES      "slices"
#define FLAG_ENC_QP_MODE         "qp-mode"
#define FLAG_ENC_ASPECT_RATIO    "aspect-ratio"
#define FLAG_ENC_SCALING_LIST    "scaling-list"
#define FLAG_ENC_LOOKAHEAD_DEPTH "lookahead-depth"
#define FLAG_ENC_TEMPORAL_AQ     "temporal-aq"
#define FLAG_ENC_SPATIAL_AQ      "spatial-aq"
#define FLAG_ENC_SPATIAL_AQ_GAIN "spatial-aq-gain"
#define FLAG_ENC_QP              "qp"
#define FLAG_ENC_NUM_CORES       "cores"
#define FLAG_ENC_TUNE_METRICS    "tune-metrics"
#define FLAG_ENC_LATENCY_LOGGING "latency_logging"
#define FLAG_ENC_OUTPUT_FILE     "o"
#define FLAG_ENC_EXPERT_OPTIONS  "expert-options"

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

typedef struct XlnxEncExpertOptions {
    char* dynamic_params_file;
    int   dynamic_params_check;
} XlnxEncExpertOptions;

typedef struct EncDynParams {
    DynparamsHandle  dynamic_param_handle;
    uint32_t         dynamic_params_count;
    uint32_t         dynamic_params_index;
    void*            dyn_params_lib;
    XlnxDynParamsObj dyn_params_obj;
    InitDynParams    xlnx_enc_init_dyn_params_obj;
} EncDynParams;

typedef struct XlnxDynIdrFrames {
    uint32_t* idr_arr;
    uint32_t  idr_arr_idx;
    size_t    len_idr_arr;
} XlnxDynIdrFrames;

typedef struct XlnxEncoderCtx {
    XmaEncoderSession* enc_session;
    XmaFrame*          in_frame; // Decoder/scaler output
    XmaFrame*      send_frame; // Frame to be sent, != in_frame if xvbm_convert.
    bool           convert_input;
    XmaDataBuffer* xma_buffer;
    xrmCuListResourceV2   encode_cu_list_res;
    XlnxLookaheadCtx      la_ctx;
    XlnxEncoderProperties enc_props;
    XlnxEncExpertOptions  expert_options;
    XlnxDynIdrFrames      dynamic_idr;
    EncDynParams          enc_dyn_params;
    size_t                num_frames_sent;
    int32_t               loop_count;
    uint32_t              num_frames;
    int32_t               la_bypass;
    int32_t               flush_frame_sent;
    int32_t               in_file;
    int32_t               out_file;
} XlnxEncoderCtx;

typedef struct {
    char* key;
    int   value;
} XlnxEncProfileLookup;

void xlnx_enc_context_init(XlnxEncoderCtx* enc_ctx);

int32_t xlnx_enc_update_props(XlnxEncoderCtx*       enc_ctx,
                              XmaEncoderProperties* xma_enc_props);

int32_t xlnx_enc_parse_args(int32_t argc, char* argv[], XlnxEncoderCtx* enc_ctx,
                            int32_t param_flag);

int32_t xlnx_enc_session(XlnxTranscoderXrmCtx* app_xrm_ctx,
                         XlnxEncoderCtx*       enc_ctx,
                         XmaEncoderProperties* xma_enc_props,
                         XmaFilterProperties*  xma_la_props);

int32_t xlnx_enc_process_frame(XlnxEncoderCtx* enc_ctx, int32_t enc_null_frame,
                               int32_t* enc_out_size);

int32_t xlnx_enc_deinit(xrmContext xrm_ctx, XlnxEncoderCtx* enc_ctx,
                        XmaEncoderProperties* xma_enc_props,
                        XmaFilterProperties*  xma_la_props);

#endif //_XLNX_ENCODER_H_
