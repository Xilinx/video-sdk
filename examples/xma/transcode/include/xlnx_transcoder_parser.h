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

#ifndef _XLNX_TRANSCODER_PARSER_H_
#define _XLNX_TRANSCODER_PARSER_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_decoder.h"
#include "xlnx_encoder.h"
#include "xlnx_transcoder.h"
#include "xlnx_transcoder_constants.h"

#define FLAG_TRANSCODE_HELP             "help"
#define FLAG_TRANSCODE_DEVICE_ID        "d"
#define FLAG_TRANSCODE_STREAM_LOOP      "stream_loop"
#define FLAG_TRANSCODE_NUM_FRAMES       "frames"
#define FLAG_TRANSCODE_GENERIC_MAX      "c:v"

char* xlnx_tran_get_help();

int32_t xlnx_tran_parser(int32_t argc, char *argv[], 
	                     XlnxTranscoderCtx *transcode_ctx,
                         XlnxTranscoderProperties *transcode_props);

#endif // _XLNX_TRANSCODER_PARSER_H_
