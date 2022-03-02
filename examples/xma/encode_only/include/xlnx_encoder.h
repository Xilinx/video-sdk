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

#ifndef _XLNX_ENCODER_H_
#define _XLNX_ENCODER_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <dlfcn.h>
#include <regex.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_enc_xma_props.h"
#include "xlnx_enc_constants.h"
#include "xlnx_enc_xrm_interface.h"
#include "xlnx_lookahead.h"
#include "xlnx_app_utils.h"

#define FLAG_HELP             "help"
#define FLAG_DEVICE_ID        "d"
#define FLAG_STREAM_LOOP      "stream_loop"
#define FLAG_INPUT_FILE       "i"
#define FLAG_CODEC_TYPE       "c:v"
#define FLAG_INPUT_WIDTH      "w"
#define FLAG_INPUT_HEIGHT     "h"
#define FLAG_INPUT_PIX_FMT    "pix_fmt"
#define FLAG_BITRATE          "b:v"
#define FLAG_BIT_RATE         "b"
#define FLAG_FPS              "fps"
#define FLAG_INTRA_PERIOD     "g"
#define FLAG_CONTROL_RATE     "control-rate"
#define FLAG_MAX_BITRATE      "max-bitrate"
#define FLAG_SLICE_QP         "slice-qp"
#define FLAG_MIN_QP           "min-qp"
#define FLAG_MAX_QP           "max-qp"
#define FLAG_NUM_BFRAMES      "bf"
#define FLAG_IDR_PERIOD       "periodicity-idr"
#define FLAG_PROFILE          "profile"
#define FLAG_LEVEL            "level"
#define FLAG_NUM_SLICES       "slices"
#define FLAG_QP_MODE          "qp-mode"
#define FLAG_ASPECT_RATIO     "aspect-ratio"
#define FLAG_SCALING_LIST     "scaling-list"
#define FLAG_LOOKAHEAD_DEPTH  "lookahead-depth"
#define FLAG_TEMPORAL_AQ      "temporal-aq"
#define FLAG_SPATIAL_AQ       "spatial-aq"
#define FLAG_SPATIAL_AQ_GAIN  "spatial-aq-gain"
#define FLAG_QP               "qp"
#define FLAG_NUM_FRAMES       "frames"
#define FLAG_NUM_CORES        "cores"
#define FLAG_TUNE_METRICS     "tune-metrics"
#define FLAG_LATENCY_LOGGING  "latency_logging"
#define FLAG_OUTPUT_FILE      "o"

/* Encoder Context */
typedef struct {
    XmaDataBuffer         xma_buffer;
    XmaFrame              in_frame;
    XmaEncoderSession     *enc_session;
    XmaFrame              *enc_in_frame;
    XmaFrame              *la_in_frame;
    XlnxEncoderXrmCtx     enc_xrm_ctx;
    XlnxLookaheadCtx      la_ctx;
    XlnxEncoderProperties enc_props;
    XlnxAppTimeTracker    enc_timer;
    size_t                num_frames;
    size_t                in_frame_cnt;
    size_t                out_frame_cnt;
    int32_t               loop_count;
    uint32_t              la_bypass;
    uint32_t              enc_state;
    int32_t               pts;
    FILE                  *in_file;
    FILE                  *out_file;
} XlnxEncoderCtx;

/* Encoder function declarations */
char *xlnx_enc_get_help();

int32_t xlnx_enc_parser(int32_t argc, char *argv[], XlnxEncoderCtx *enc_ctx,
        XmaEncoderProperties *xma_enc_props);

int32_t xlnx_enc_la_init(XlnxEncoderCtx *enc_ctx, 
                         XmaFilterProperties *xma_la_props);

int32_t xlnx_enc_read_frame(XlnxEncoderCtx *enc_ctx, XmaFrame *xma_frame);

int32_t xlnx_enc_create_session(XlnxEncoderCtx *enc_ctx, 
                           XmaEncoderProperties *xma_enc_props);

int32_t xlnx_enc_frame_process(XlnxEncoderCtx *enc_ctx, int32_t *enc_stop);

void xlnx_enc_deinit(XlnxEncoderCtx *enc_ctx, 
                     XmaEncoderProperties *xma_enc_props);

#endif // _XLNX_ENCODER_H_
