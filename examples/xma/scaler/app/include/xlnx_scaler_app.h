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

#ifndef _XLNX_ABR_SCALER_APP_H_
#define _XLNX_ABR_SCALER_APP_H_

#include "xlnx_scaler.h"

typedef struct XlnxScalerAppCtx {
    FILE*              in_fp;
    XlnxFormatType     in_pix_fmt;
    int32_t            loops_remaining;
    FILE*              out_fp[MAX_SCALER_OUTPUTS];
    XlnxFormatType     out_pix_fmt[MAX_SCALER_OUTPUTS];
    size_t             num_frames_to_scale;
    size_t             num_frames_scaled;
    XlnxAppTimeTracker timer;
    XlnxScalerCtx      scaler_ctx;
} XlnxScalerAppCtx;

#endif
