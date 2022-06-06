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
#include <getopt.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xma.h>
#include <xrm.h>


#include "xlnx_app_utils.h"
#include "xlnx_enc_constants.h"
#include "xlnx_enc_xma_props.h"
#include "xlnx_enc_xrm_interface.h"
#include "xlnx_lookahead.h"


typedef struct XlnxEncExpertOptions {
    char* dynamic_params_file;
    int   dynamic_params_check;
} XlnxEncExpertOptions;

typedef struct XlnxDynIdrFrames {
    uint32_t* idr_arr;
    uint32_t  idr_arr_idx;
    size_t    len_idr_arr;
} XlnxDynIdrFrames;

/* Encoder Context */
typedef struct {
    size_t                 num_frames_sent;
    size_t                 num_frames_received;
    XmaDataBuffer          output_xma_buffer;
    XmaEncoderSession*     enc_session;
    XmaFrame               enc_in_xframe;
    XlnxEncoderXrmCtx      enc_xrm_ctx;
    XlnxEncoderProperties* enc_props;
    XlnxDynIdrFrames*      dynamic_idr;
    XmaEncoderProperties   xma_enc_props;
    bool                   flush_sent;
    int32_t                pts;
} XlnxEncoderCtx;

/* Encoder function declarations */

int32_t xlnx_enc_create_enc_ctx(XlnxEncoderProperties* enc_props,
                                XlnxEncoderCtx*        enc_ctx);

int32_t xlnx_enc_process_frame(XlnxEncoderCtx* enc_ctx, XmaFrame* input_xframe,
                               int32_t* recv_size);

void xlnx_enc_deinit(XlnxEncoderCtx* enc_ctx);

#endif // _XLNX_ENCODER_H_
