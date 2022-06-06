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

#ifndef _XLNX_ENCODER_ARG_PARSE_H_
#define _XLNX_ENCODER_ARG_PARSE_H_

#include <ctype.h>
#include <getopt.h>

#include "xlnx_encoder.h"

#define FLAG_HELP            "help"
#define FLAG_DEVICE_ID       "d"
#define FLAG_STREAM_LOOP     "stream_loop"
#define FLAG_INPUT_FILE      "i"
#define FLAG_CODEC_TYPE      "c:v"
#define FLAG_INPUT_WIDTH     "w"
#define FLAG_INPUT_HEIGHT    "h"
#define FLAG_INPUT_PIX_FMT   "pix_fmt"
#define FLAG_BITRATE         "b:v"
#define FLAG_BIT_RATE        "b"
#define FLAG_FPS             "fps"
#define FLAG_INTRA_PERIOD    "g"
#define FLAG_CONTROL_RATE    "control-rate"
#define FLAG_MAX_BITRATE     "max-bitrate"
#define FLAG_SLICE_QP        "slice-qp"
#define FLAG_MIN_QP          "min-qp"
#define FLAG_MAX_QP          "max-qp"
#define FLAG_NUM_BFRAMES     "bf"
#define FLAG_IDR_PERIOD      "periodicity-idr"
#define FLAG_DYN_IDR         "force_key_frame"
#define FLAG_PROFILE         "profile"
#define FLAG_LEVEL           "level"
#define FLAG_NUM_SLICES      "slices"
#define FLAG_QP_MODE         "qp-mode"
#define FLAG_ASPECT_RATIO    "aspect-ratio"
#define FLAG_SCALING_LIST    "scaling-list"
#define FLAG_LOOKAHEAD_DEPTH "lookahead-depth"
#define FLAG_TEMPORAL_AQ     "temporal-aq"
#define FLAG_SPATIAL_AQ      "spatial-aq"
#define FLAG_SPATIAL_AQ_GAIN "spatial-aq-gain"
#define FLAG_QP              "qp"
#define FLAG_NUM_FRAMES      "frames"
#define FLAG_NUM_CORES       "cores"
#define FLAG_TUNE_METRICS    "tune-metrics"
#define FLAG_LATENCY_LOGGING "latency_logging"
#define FLAG_EXPERT_OPTIONS  "expert-options"
#define FLAG_OUTPUT_FILE     "o"

typedef enum {
    HELP_ARG = 0,
    DEVICE_ID_ARG,
    LOOP_COUNT_ARG,
    INPUT_FILE_ARG,
    ENCODER_ARG,
    INPUT_WIDTH_ARG,
    INPUT_HEIGHT_ARG,
    INPUT_PIX_FMT_ARG,
    BITRATE_ARG,
    FPS_ARG,
    INTRA_PERIOD_ARG,
    CONTROL_RATE_ARG,
    MAX_BITRATE_ARG,
    SLICE_QP_ARG,
    MIN_QP_ARG,
    MAX_QP_ARG,
    NUM_BFRAMES_ARG,
    IDR_PERIOD_ARG,
    DYNAMIC_IDR,
    PROFILE_ARG,
    LEVEL_ARG,
    NUM_SLICES_ARG,
    QP_MODE_ARG,
    ASPECT_RATIO_ARG,
    SCALING_LIST_ARG,
    LOOKAHEAD_DEPTH_ARG,
    TEMPORAL_AQ_ARG,
    SPATIAL_AQ_ARG,
    SPATIAL_AQ_GAIN_ARG,
    QP_ARG,
    NUM_FRAMES_ARG,
    LATENCY_LOGGING_ARG,
    NUM_CORES_ARG,
    TUNE_METRICS_ARG,
    EXPERT_OPTIONS_ARG,
    OUTPUT_FILE_ARG
} XlnxEncArgIdentifiers;

/* Encoder Context */
typedef struct XlnxEncoderArguments {
    XlnxLookaheadProperties la_props;
    XlnxEncoderProperties   enc_props;
    char*                   input_file;
    XlnxFormatType          pix_fmt;
    XlnxDynIdrFrames        dynamic_idr;
    char*                   output_file;
    size_t                  num_frames;
    int32_t                 loop_count;
    XlnxEncExpertOptions    expert_options;
} XlnxEncoderArguments;

int32_t xlnx_enc_get_arguments(int32_t argc, char* argv[],
                               XlnxEncoderArguments* arguments);

void xlnx_enc_copy_enc_props_to_la(XlnxEncoderArguments* arguments);

#endif // _XLNX_ENCODER_H_
