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

#ifndef _XLNX_SCALER_H_
#define _XLNX_SCALER_H_

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
#include "xlnx_transcoder_constants.h"
#include "xlnx_transcoder_xma_props.h"
#include "xlnx_transcoder_xrm_interface.h"
#include "xlnx_xvbm_utils.h"


#define FLAG_SCAL_DEVICE_ID       "dl"
#define FLAG_SCAL_FILTER          "multiscale_xma"
#define FLAG_SCAL_ENABLE_PIPELINE "enable_pipeline"
#define FLAG_SCAL_NUM_OUTPUTS     "num-outputs"
#define FLAG_SCAL_CONVERT_INPUT   "xvbm_convert"

#define OUTPUT                       "out_"
#define WIDTH                        "_width"
#define HEIGHT                       "_height"
#define RATE                         "_rate"
#define FLAG_SCAL_OUTPUT_WIDTH(NUM)  OUTPUT NUM WIDTH
#define FLAG_SCAL_OUTPUT_HEIGHT(NUM) OUTPUT NUM HEIGHT
#define FLAG_SCAL_OUTPUT_RATE(NUM)   OUTPUT NUM RATE
#define FLAG_SCAL_LATENCY_LOGGING    "latency_logging"
#define FLAG_SCAL_MAX                "c:v"

typedef enum {
    SCAL_SESSION_ALL_RATE = 0,
    SCAL_SESSION_FULL_RATE,
    SCAL_MAX_SESSIONS
} XlnxScalSessionType;

typedef struct XlnxScalerCtx {
    XmaFrame*            in_frame;
    XmaFrame*            send_frame; // != in_frame when using xvbm_convert
    XmaFrame*            out_frame[SCAL_MAX_ABR_CHANNELS];
    XmaScalerSession*    session[SCAL_MAX_SESSIONS];
    xrmCuListResourceV2  scal_cu_list_res[SCAL_MAX_SESSIONS];
    XlnxScalerProperties scal_props;
    int32_t              xrm_alloc_st[SCAL_MAX_SESSIONS];
    int32_t              session_nb_outputs[SCAL_MAX_SESSIONS];
    int32_t*             copyOutLink;
    int32_t              scaler_enable;
    bool                 convert_input;
    int32_t              num_sessions;
    int32_t              session_frame;
    int32_t              xrm_scalres_count;
    int32_t              flush;
    int32_t              send_status;
    int32_t              frames_out;
} XlnxScalerCtx;

int32_t xlnx_scal_session(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XlnxScalerCtx*        scal_ctx,
                          XmaScalerProperties*  xma_scal_props);

int32_t xlnx_scal_update_props(XlnxScalerCtx*       scal_ctx,
                               XmaScalerProperties* xma_scal_props);
int32_t xlnx_scal_parse_args(int32_t argc, char* argv[],
                             XlnxScalerCtx* scal_ctx, int32_t* param_flag);

void xlnx_scal_context_init(XlnxScalerCtx* scal_ctx);

int32_t xlnx_scal_process_frame(XlnxScalerCtx* scal_ctx);
int32_t xlnx_scal_deinit(xrmContext xrm_ctx, XlnxScalerCtx* scal_ctx,
                         XmaScalerProperties* xma_scal_props);

#endif //_XLNX_SCALER_H_
