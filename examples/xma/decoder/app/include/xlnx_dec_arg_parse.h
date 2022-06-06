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
#ifndef _XLNX_DEC_ARG_PARSE_H_
#define _XLNX_DEC_ARG_PARSE_H_

#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"
#include "xlnx_dec_xma_props.h"
#include <ctype.h>
#include <getopt.h>


#define HEVC_PATTERN_MATCH "^(mpsoc_vcu_)?([h|H]?265|hevc|HEVC)$"
#define AVC_PATTERN_MATCH  "^(mpsoc_vcu_)?([h|H]?264|avc|AVC)$"

#define FLAG_HELP            "help"
#define FLAG_LOG_LEVEL       "log"
#define FLAG_DEVICE_ID       "d"
#define FLAG_STREAM_LOOP     "stream_loop"
#define FLAG_INPUT_FILE      "i"
#define FLAG_CODEC_TYPE      "c:v"
#define FLAG_LOW_LATENCY     "low_latency"
#define FLAG_ENTROPY_BUF_CNT "entropy_buf_cnt"
#define FLAG_LATENCY_LOGGING "latency_logging"
#define FLAG_SPLITBUFF_MODE  "splitbuff_mode"
#define FLAG_NUM_FRAMES      "frames"
#define FLAG_PIX_FMT         "pix_fmt"
#define FLAG_OUTPUT_FILE     "o"

typedef enum {
    HELP_ARG = 0,
    LOG_LEVEL_ARG,
    DEVICE_ID_ARG,
    LOOP_COUNT_ARG,
    INPUT_FILE_ARG,
    DECODER_ARG,
    LOW_LATENCY_ARG,
    ENTROPY_BUFFERS_COUNT_ARG,
    LATENCY_LOGGING_ARG,
    SPLITBUFF_MODE_ARG,
    NUM_FRAMES_ARG,
    PIX_FMT_ARG,
    OUTPUT_FILE_ARG
} decoder_argument_identifiers;

typedef struct XlnxDecoderArguments {
    XlnxDecoderProperties dec_props;
    char*                 input_file;
    char*                 output_file;
    int                   loop_count;
    XlnxFormatType        pix_fmt;
    size_t                num_frames;
} XlnxDecoderArguments;

/**
 * Parse the commandline into XlnxDecoderArguments
 * @param argc: The number of commandline arguments
 * @param argv: The arguments themselves
 * @return XMA_APP_SUCCESS on success
 */
int xlnx_dec_get_arguments(int argc, char* argv[],
                           XlnxDecoderArguments* dec_args);

#endif //_XLNX_DEC_ARG_PARSE_H_
