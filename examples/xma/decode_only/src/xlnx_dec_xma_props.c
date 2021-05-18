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
#include "xlnx_dec_xma_props.h"

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_decoder_props: Frees resources allocated in up decoder 
    properties
Parameters:
    dec_xma_props: The decoder properties 
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_decoder_props(XmaDecoderProperties* dec_xma_props)
{
    if(dec_xma_props->params) {
        free(dec_xma_props->params);
    }
}

/*------------------------------------------------------------------------------
dec_fill_custom_xma_params: Link the custom decoder params in the decoder 
    context to what will be sent to the decoder plugin
Parameters:
    param_ctx: The context containing custom decoder parameters 
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
static int32_t dec_fill_custom_xma_params(XlnxDecoderProperties* param_ctx, 
                                          XmaDecoderProperties* dec_xma_props)
{
    dec_xma_props->params[0].name   = "bitdepth";
    dec_xma_props->params[0].type   = XMA_UINT32;
    dec_xma_props->params[0].length = sizeof(param_ctx->bit_depth);
    dec_xma_props->params[0].value  = &(param_ctx->bit_depth);

    dec_xma_props->params[1].name   = "codec_type";
    dec_xma_props->params[1].type   = XMA_UINT32;
    dec_xma_props->params[1].length = sizeof(param_ctx->codec_type);
    dec_xma_props->params[1].value  = &(param_ctx->codec_type);

    dec_xma_props->params[2].name   = "low_latency";
    dec_xma_props->params[2].type   = XMA_UINT32;
    dec_xma_props->params[2].length = sizeof(param_ctx->low_latency);
    dec_xma_props->params[2].value  = &(param_ctx->low_latency);

    dec_xma_props->params[3].name   = "entropy_buffers_count";
    dec_xma_props->params[3].type   = XMA_UINT32;
    dec_xma_props->params[3].length = sizeof(param_ctx->entropy_buf_cnt);
    dec_xma_props->params[3].value  = &(param_ctx->entropy_buf_cnt);

    dec_xma_props->params[4].name   = "zero_copy";
    dec_xma_props->params[4].type   = XMA_UINT32;
    dec_xma_props->params[4].length = sizeof(param_ctx->zero_copy);
    dec_xma_props->params[4].value  = &(param_ctx->zero_copy);

    dec_xma_props->params[5].name   = "profile";
    dec_xma_props->params[5].type   = XMA_UINT32;
    dec_xma_props->params[5].length = sizeof(param_ctx->profile_idc);
    dec_xma_props->params[5].value  = &(param_ctx->profile_idc);

    dec_xma_props->params[6].name   = "level";
    dec_xma_props->params[6].type   = XMA_UINT32;
    dec_xma_props->params[6].length = sizeof(param_ctx->level_idc);
    dec_xma_props->params[6].value  = &(param_ctx->level_idc);

    dec_xma_props->params[7].name   = "chroma_mode";
    dec_xma_props->params[7].type   = XMA_UINT32;
    dec_xma_props->params[7].length = sizeof(param_ctx->chroma_mode);
    dec_xma_props->params[7].value  = &(param_ctx->chroma_mode);

    dec_xma_props->params[8].name   = "scan_type";
    dec_xma_props->params[8].type   = XMA_UINT32;
    dec_xma_props->params[8].length = sizeof(param_ctx->scan_type);
    dec_xma_props->params[8].value  = &(param_ctx->scan_type);

    dec_xma_props->params[9].name   = "latency_logging";
    dec_xma_props->params[9].type   = XMA_UINT32;
    dec_xma_props->params[9].length = sizeof(param_ctx->latency_logging);
    dec_xma_props->params[9].value  = &(param_ctx->latency_logging);

    dec_xma_props->params[10].name   = "splitbuff_mode";
    dec_xma_props->params[10].type   = XMA_UINT32;
    dec_xma_props->params[10].length = sizeof(param_ctx->splitbuff_mode);
    dec_xma_props->params[10].value  = &(param_ctx->splitbuff_mode);

    return DEC_APP_SUCCESS;
}

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
                                      XmaDecoderProperties* dec_xma_props)
{
    strcpy (dec_xma_props->hwvendor_string, "MPSoC");
    dec_xma_props->hwdecoder_type         = XMA_MULTI_DECODER_TYPE;
    dec_xma_props->dev_index              = param_ctx->device_id;
    dec_xma_props->width                  = param_ctx->width;
    dec_xma_props->height                 = param_ctx->height;
    dec_xma_props->bits_per_pixel         = param_ctx->bit_depth;
    dec_xma_props->framerate.numerator    = param_ctx->fps;
    dec_xma_props->framerate.denominator  = 1;
    dec_xma_props->param_cnt              = MAX_DEC_PARAMS;
    dec_xma_props->params                 = calloc(1, MAX_DEC_PARAMS * 
                                            sizeof(XmaParameter));
    dec_fill_custom_xma_params(param_ctx, dec_xma_props);
    /* Xrm related dec_xma_props (plugin_lib, ddr_bank_index, cu_index, 
    channel_id, dev_index if necessary) will be set in 
    xlnx_dec_create_dec_xrm_ctx */
    return DEC_APP_SUCCESS;
}
