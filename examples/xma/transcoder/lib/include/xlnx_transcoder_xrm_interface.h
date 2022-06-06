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

#ifndef _XLNX_TRANSCODER_XRM_INTERFACE_H_
#define _XLNX_TRANSCODER_XRM_INTERFACE_H_

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include <xma.h>
#include <xrm.h>

#include "xlnx_app_utils.h"
#include "xlnx_transcoder_constants.h"
#include "xlnx_transcoder_xma_props.h"
#include "xlnx_xrm_utils.h"


typedef struct XlnxTranscodeLoad {
    int dec_load;
    int scal_load;
    int enc_load;
    int enc_num;
    int la_load;
} XlnxTranscoderLoad;

typedef struct {
    xrmContext         xrm_ctx;
    XlnxTranscoderLoad transcode_load;
    int32_t            reserve_idx;
    int32_t            dec_res_in_use;
    int32_t            scal_res_in_use;
    int32_t            enc_res_in_use;
    int32_t            lookahead_res_in_use;
    int32_t            num_scal_out;
    int32_t            num_fullrate_out;
    int32_t            num_enc_channels;
    int8_t             la_enable;
} XlnxTranscoderXrmCtx;

int32_t xlnx_tran_xma_init(XlnxTranscoderXrmCtx*     app_xrm_ctx,
                           XlnxTranscoderProperties* transcode_props);

int32_t xlnx_dec_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XmaDecoderProperties* xma_dec_props,
                          xrmCuListResourceV2*  decode_cu_list_res);

int32_t xlnx_scal_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                           XmaScalerProperties*  xma_scal_props,
                           xrmCuListResourceV2*  scal_cu_list_res);

int32_t xlnx_la_allocate_xrm_cu(XlnxTranscoderXrmCtx* app_xrm_ctx,
                                XmaFilterProperties*  xma_la_props,
                                xrmCuListResourceV2*  lookahead_cu_list_res);

int32_t xlnx_enc_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XmaEncoderProperties* xma_enc_props,
                          xrmCuListResourceV2*  encode_cu_list_res);

void xlnx_tran_xrm_deinit(XlnxTranscoderXrmCtx* app_xrm_ctx);

#endif // _XLNX_TRANSCODER_XRM_INTERFACE_H_
