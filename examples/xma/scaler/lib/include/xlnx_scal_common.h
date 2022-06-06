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

#ifndef _XLNX_SCAL_COMMON_H_
#define _XLNX_SCAL_COMMON_H_

#include <xma.h>

#define XLNX_SCALER_APP_MODULE "xlnx_scaler_app"

#define MAX_SCALER_PARAM 4
/* MAX_SCALER_OUTPUTS defined in /opt/xilinx/xrt/include/lib/xmalimits.h */
#define MAX_SCALER_WIDTH  3840
#define MAX_SCALER_HEIGHT 2160
#define MIN_SCALER_WIDTH  284
#define MIN_SCALER_HEIGHT 160

#endif
