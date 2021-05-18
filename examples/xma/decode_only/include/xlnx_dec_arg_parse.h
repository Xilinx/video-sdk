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
#ifndef _XLNX_DEC_ARG_PARSE_H_
#define _XLNX_DEC_ARG_PARSE_H_

#include <getopt.h>
#include <ctype.h>
#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"

#define HEVC_PATTERN_MATCH "^(mpsoc_vcu_)?([h|H]?265|hevc|HEVC)$"
#define AVC_PATTERN_MATCH  "^(mpsoc_vcu_)?([h|H]?264|avc|AVC)$"

#define FLAG_HELP             "help"
#define FLAG_LOG_LEVEL        "log"
#define FLAG_DEVICE_ID        "d"
#define FLAG_STREAM_LOOP      "stream_loop"
#define FLAG_INPUT_FILE       "i"
#define FLAG_CODEC_TYPE       "c:v"
#define FLAG_LOW_LATENCY      "low_latency"
#define FLAG_ENTROPY_BUF_CNT  "entropy_buf_cnt"
#define FLAG_LATENCY_LOGGING  "latency_logging"
#define FLAG_SPLITBUFF_MODE   "splitbuff_mode"
#define FLAG_NUM_FRAMES       "frames"
#define FLAG_OUTPUT_FILE      "o"

typedef enum
{
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
    OUTPUT_FILE_ARG
} decoder_argument_identifiers;

typedef struct XlnxDecoderArguments
{
    int      log_level;                   /* -log             */
    int      device_id;                   /* -d               */
    int      loop_count;                  /* -stream_loop     */
    char*    input_file;                  /* -i               */
    int      decoder;                     /* -c:v             */
    int      low_latency;                 /* -low_latency     */
    int      entropy_buf_cnt;             /* -entropy_buf_cnt */
    int      latency_logging;             /* -latency_logging */
    int      splitbuff_mode;              /* -splitbuff_mod   */
    size_t   num_frames;                  /* -frames          */
    char*    output_file;                 /* -o               */
} XlnxDecoderArguments;

/*------------------------------------------------------------------------------
xlnx_dec_get_arguments: Parse the commandline into XlnxDecoderArguments

Parameters:
    argc: The number of commandline arguments
    argv: The arguments themselves
Return:
    A XlnxDecoderArguments struct which contains the commandline information
------------------------------------------------------------------------------*/
XlnxDecoderArguments xlnx_dec_get_arguments(int argc, char* argv[]);

/*------------------------------------------------------------------------------
xlnx_dec_dump_arguments: Dump the contents of arguments

Parameters:
    arguments: The arguments struct
------------------------------------------------------------------------------*/
void xlnx_dec_dump_arguments(XlnxDecoderArguments arguments);

#endif //_XLNX_DEC_ARG_PARSE_H_
