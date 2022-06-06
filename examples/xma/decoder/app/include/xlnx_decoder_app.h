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
#ifndef _XLNX_DECODER_APP_H_
#define _XLNX_DECODER_APP_H_
#include "xlnx_dec_arg_parse.h"
#include "xlnx_decoder.h"


typedef struct XlnxDecoderAppCtx {
    FILE*              out_fp;
    XlnxFormatType     out_pix_fmt;
    size_t             num_frames_to_decode;
    size_t             num_frames_decoded;
    XlnxDecoderCtx     dec_ctx;
    XmaFrame           output_xframe;
    XlnxAppTimeTracker timer;
} XlnxDecoderAppCtx;

#endif
