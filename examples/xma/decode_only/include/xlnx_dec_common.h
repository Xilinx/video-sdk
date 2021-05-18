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
#ifndef _INCLUDED_XLNX_DEC_COMMON_H_
#define _INCLUDED_XLNX_DEC_COMMON_H_

#include <xma.h>

#define XLNX_DEC_APP_MODULE     "xlnx_decoder_app"
#define DECODER_APP_LOG_ERROR(msg...) \
            xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE, msg)
#define DECODER_APP_LOG_INFO(msg...) \
            xma_logmsg(XMA_INFO_LOG, XLNX_DEC_APP_MODULE, msg)
 
#define MAX_DEC_PARAMS             11
#define MAX_DEC_WIDTH              3840
#define MAX_DEC_HEIGHT             2160

#define DEFAULT_ENTROPY_BUFF_COUNT 2
#define MIN_ENTROPY_BUFF_COUNT     2
#define MAX_ENTROPY_BUFF_COUNT     10

#define DEC_APP_ERROR              XMA_ERROR
#define DEC_APP_SUCCESS            XMA_SUCCESS

#endif