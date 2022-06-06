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
#include "xlnx_dec_xma_props.h"

/**
 * Frees resources allocated in up decoder
 * properties
 * @param dec_xma_props: The decoder properties
 * @return XMA_APP_SUCCESS on success
 */
void xlnx_dec_cleanup_decoder_props(XmaDecoderProperties* xma_dec_props)
{
    if(xma_dec_props->params) {
        free(xma_dec_props->params);
    }
}

/**
 * Link the custom decoder params in the decoder
 * context to what will be sent to the decoder plugin
 * @param dec_props The context containing custom decoder parameters
 * @return XMA_APP_SUCCESS on success
 */
static int dec_fill_custom_xma_params(XlnxDecoderProperties* dec_props,
                                      XmaDecoderProperties*  xma_dec_props)
{
    xma_dec_props->params[0].name   = "bitdepth";
    xma_dec_props->params[0].type   = XMA_UINT32;
    xma_dec_props->params[0].length = sizeof(dec_props->bit_depth);
    xma_dec_props->params[0].value  = &(dec_props->bit_depth);

    xma_dec_props->params[1].name   = "codec_type";
    xma_dec_props->params[1].type   = XMA_UINT32;
    xma_dec_props->params[1].length = sizeof(dec_props->codec_type);
    xma_dec_props->params[1].value  = &(dec_props->codec_type);

    xma_dec_props->params[2].name   = "low_latency";
    xma_dec_props->params[2].type   = XMA_UINT32;
    xma_dec_props->params[2].length = sizeof(dec_props->low_latency);
    xma_dec_props->params[2].value  = &(dec_props->low_latency);

    xma_dec_props->params[3].name   = "entropy_buffers_count";
    xma_dec_props->params[3].type   = XMA_UINT32;
    xma_dec_props->params[3].length = sizeof(dec_props->entropy_buf_cnt);
    xma_dec_props->params[3].value  = &(dec_props->entropy_buf_cnt);

    xma_dec_props->params[4].name   = "zero_copy";
    xma_dec_props->params[4].type   = XMA_UINT32;
    xma_dec_props->params[4].length = sizeof(dec_props->zero_copy);
    xma_dec_props->params[4].value  = &(dec_props->zero_copy);

    xma_dec_props->params[5].name   = "profile";
    xma_dec_props->params[5].type   = XMA_UINT32;
    xma_dec_props->params[5].length = sizeof(dec_props->profile_idc);
    xma_dec_props->params[5].value  = &(dec_props->profile_idc);

    xma_dec_props->params[6].name   = "level";
    xma_dec_props->params[6].type   = XMA_UINT32;
    xma_dec_props->params[6].length = sizeof(dec_props->level_idc);
    xma_dec_props->params[6].value  = &(dec_props->level_idc);

    xma_dec_props->params[7].name   = "chroma_mode";
    xma_dec_props->params[7].type   = XMA_UINT32;
    xma_dec_props->params[7].length = sizeof(dec_props->chroma_mode);
    xma_dec_props->params[7].value  = &(dec_props->chroma_mode);

    xma_dec_props->params[8].name   = "scan_type";
    xma_dec_props->params[8].type   = XMA_UINT32;
    xma_dec_props->params[8].length = sizeof(dec_props->scan_type);
    xma_dec_props->params[8].value  = &(dec_props->scan_type);

    xma_dec_props->params[9].name   = "latency_logging";
    xma_dec_props->params[9].type   = XMA_UINT32;
    xma_dec_props->params[9].length = sizeof(dec_props->latency_logging);
    xma_dec_props->params[9].value  = &(dec_props->latency_logging);

    xma_dec_props->params[10].name   = "splitbuff_mode";
    xma_dec_props->params[10].type   = XMA_UINT32;
    xma_dec_props->params[10].length = sizeof(dec_props->splitbuff_mode);
    xma_dec_props->params[10].value  = &(dec_props->splitbuff_mode);

    return XMA_APP_SUCCESS;
}

/**
 * Validates the dec props are in proper ranges. Called internally by
 * xlnx_dec_create_xma_dec_props, but can also be called externally.
 * @param dec_props The xlnx decoder properties to be validated
 * @return XMA_SUCCESS or XMA_ERROR
 */
int xlnx_dec_validate_dec_props(XlnxDecoderProperties* dec_props)
{
    if(dec_props->low_latency != 0 && dec_props->low_latency != 1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid low latency %d! Valid values are 0 or 1.\n",
                   dec_props->entropy_buf_cnt);
        return XMA_APP_ERROR;
    }
    if(dec_props->splitbuff_mode != 0 && dec_props->splitbuff_mode != 1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid split buffers mode %d! Valid values are 0 or 1.\n",
                   dec_props->splitbuff_mode);
        return XMA_APP_ERROR;
    }
    if(dec_props->latency_logging != 0 && dec_props->latency_logging != 1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid low latency %d! Valid values are 0 or 1.\n",
                   dec_props->latency_logging);
        return XMA_APP_ERROR;
    }
    if(dec_props->entropy_buf_cnt < MIN_ENTROPY_BUFF_COUNT ||
       dec_props->entropy_buf_cnt > MAX_ENTROPY_BUFF_COUNT) {

        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid entropy buffer count %d! Valid values range"
                   " [2-10]. Default is 2.\n",
                   dec_props->entropy_buf_cnt);
        return XMA_APP_ERROR;
    }
    if(dec_props->codec_type == H264_CODEC_TYPE) {
        if(dec_props->width < MIN_H264_DEC_HEIGHT ||
           dec_props->height < MIN_H264_DEC_HEIGHT ||
           dec_props->width > MAX_H264_DEC_WIDTH ||
           dec_props->height > MAX_H264_DEC_WIDTH ||
           dec_props->width * dec_props->height >
               MAX_H264_DEC_WIDTH * MAX_H264_DEC_HEIGHT ||
           dec_props->width * dec_props->height <
               MIN_H264_DEC_HEIGHT * MIN_H264_DEC_WIDTH) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                       "Invalid resolutions %dx%d.\n Supported"
                       " %dx%d <= resolution <= %xx%d (or portrait "
                       "equivalent)\n",
                       dec_props->width, dec_props->height, MIN_H264_DEC_WIDTH,
                       MIN_H264_DEC_HEIGHT, MAX_H264_DEC_WIDTH,
                       MAX_H264_DEC_HEIGHT);
            return XMA_APP_ERROR;
        }
    } else if(dec_props->codec_type == HEVC_CODEC_TYPE) {
        if(dec_props->width < MIN_HEVC_DEC_HEIGHT ||
           dec_props->height < MIN_HEVC_DEC_HEIGHT ||
           dec_props->width > MAX_HEVC_DEC_WIDTH ||
           dec_props->height > MAX_HEVC_DEC_WIDTH ||
           dec_props->width * dec_props->height >
               MAX_HEVC_DEC_WIDTH * MAX_HEVC_DEC_HEIGHT ||
           dec_props->width * dec_props->height <
               MIN_HEVC_DEC_HEIGHT * MIN_HEVC_DEC_WIDTH) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                       "Invalid resolutions %dx%d.\n Supported"
                       " %dx%d <= resolution <= %xx%d (or portrait "
                       "equivalent)\n",
                       dec_props->width, dec_props->height, MIN_HEVC_DEC_WIDTH,
                       MIN_HEVC_DEC_HEIGHT, MAX_HEVC_DEC_WIDTH,
                       MAX_HEVC_DEC_HEIGHT);
            return XMA_APP_ERROR;
        }
    } else {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid decoder codec id %d.\n", dec_props->codec_type);
        return XMA_APP_ERROR;
    }
    if(dec_props->bit_depth != 8 && dec_props->bit_depth != 10) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Invalid decoder codec id %d.\n", dec_props->bit_depth);
        return XMA_APP_ERROR;
    }
    if(dec_props->zero_copy != 1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Zero copy must be enabled for decoder. Set to %d.\n",
                   dec_props->zero_copy);
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Set the default decoder properties to be used to create xma props.
 * Does not set that which should be found in the input header.
 * @param dec_props The xlnx decoder properties whose defaults will be set
 * @return XMA_APP_SUCCESS
 */
int xlnx_dec_set_default_dec_props(XlnxDecoderProperties* dec_props)
{
    memset(dec_props, 0, sizeof(XlnxDecoderProperties));
    dec_props->device_id       = DEFAULT_DEVICE_ID;
    dec_props->entropy_buf_cnt = DEFAULT_ENTROPY_BUFF_COUNT;
    dec_props->chroma_mode     = 420;
    dec_props->zero_copy       = 1;
    return XMA_APP_SUCCESS;
}

/**
 * Sets the decoder properties. Xrm related
 * properties will be set later in xlnx_dec_create_dec_xrm_ctx and
 * xlnx_dec_allocate_xrm_dec_cu
 * @param xma_dec_props The decoder properties to be set
 * @param dec_props The context used to set the decoder properties.
 * @return XMA_APP_SUCCESS on success
 */
int xlnx_dec_create_xma_dec_props(XlnxDecoderProperties* dec_props,
                                  XmaDecoderProperties*  xma_dec_props)
{
    if(xlnx_dec_validate_dec_props(dec_props) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    strcpy(xma_dec_props->hwvendor_string, "MPSoC");
    xma_dec_props->hwdecoder_type        = XMA_MULTI_DECODER_TYPE;
    xma_dec_props->dev_index             = dec_props->device_id;
    xma_dec_props->width                 = dec_props->width;
    xma_dec_props->height                = dec_props->height;
    xma_dec_props->bits_per_pixel        = dec_props->bit_depth;
    xma_dec_props->framerate.numerator   = dec_props->fps;
    xma_dec_props->framerate.denominator = 1;
    xma_dec_props->param_cnt             = MAX_DEC_PARAMS;
    xma_dec_props->params = calloc(1, MAX_DEC_PARAMS * sizeof(XmaParameter));
    dec_fill_custom_xma_params(dec_props, xma_dec_props);
    /* Xrm related xma_dec_props (plugin_lib, ddr_bank_index, cu_index,
    channel_id, dev_index if necessary) will be set in
    xlnx_dec_create_dec_xrm_ctx */
    return XMA_APP_SUCCESS;
}
