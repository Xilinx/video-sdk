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

#ifndef _XLNX_ENC_XRM_INTERFACE_H_
#define _XLNX_ENC_XRM_INTERFACE_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_enc_constants.h"

#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))

typedef struct {
    xrmContext*       xrm_ctx;
    xrmCuListResource encode_cu_list_res;
    xrmCuResource     lookahead_cu_res;
    int32_t           device_id;
    int32_t           enc_res_idx;
    int32_t           enc_load;
    int32_t           la_load;
    int32_t           enc_num;
    int32_t           enc_res_in_use;
    int32_t           lookahead_res_inuse;
} XlnxEncoderXrmCtx;

int32_t xlnx_enc_device_init(XlnxEncoderXrmCtx *enc_xrm_ctx,
                        XmaEncoderProperties *xma_enc_props,
                        int32_t lookahead_enable);

void xlnx_enc_xrm_deinit(XlnxEncoderXrmCtx *enc_xrm_ctx);

#endif // _XLNX_ENC_XRM_INTERFACE_H_
