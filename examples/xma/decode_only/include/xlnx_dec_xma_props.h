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
#ifndef _INCLUDED_XLNX_DEC_XMA_PROPS_H_
#define _INCLUDED_XLNX_DEC_XMA_PROPS_H_

#include <string.h>

#include <xmaplugin.h>
#include <xrm.h>
#include <xvbm.h> 

#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"

typedef struct XlnxDecoderProperties
{
    int32_t  device_id; // -1 by default
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t log_level;
    uint32_t bit_depth;
    uint32_t codec_type;
    uint32_t low_latency;
    uint32_t entropy_buf_cnt;
    uint32_t zero_copy;
    uint32_t profile_idc;
    uint32_t level_idc;
    uint32_t chroma_mode;
    uint32_t scan_type;
    uint32_t latency_logging;
    uint32_t splitbuff_mode;
} XlnxDecoderProperties;

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_decoder_props: Frees resources allocated in up decoder 
    properties
Parameters:
    dec_xma_props: The decoder properties 
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_decoder_props(XmaDecoderProperties* dec_xma_props);

/*------------------------------------------------------------------------------
xlnx_dec_create_xma_dec_props: Sets the decoder properties. Xrm related 
    properties will be set later in xlnx_dec_create_dec_xrm_ctx and 
    xlnx_dec_allocate_xrm_dec_cu
Parameters:
    dec_xma_props: The decoder properties to be set
    param_ctx: The context used to set the decoder properties.
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
int32_t xlnx_dec_create_xma_dec_props(XlnxDecoderProperties* param_ctx,
                                      XmaDecoderProperties* dec_xma_props);


#endif