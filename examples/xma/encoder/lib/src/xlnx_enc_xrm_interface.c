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

#include "xlnx_enc_xrm_interface.h"

/**
 * xlnx_enc_load_calc: Calculates encoder and lookahead load and updates CU pool
 * properties
 *
 * @param enc_xrm_ctx: Encoder XRM context
 * @param xma_enc_props: Encoder xma properties
 * @param lookahead_enable: Lookahead enable flag
 * @param enc_cu_pool_prop: Encoder CU pool properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_load_calc(XlnxEncoderXrmCtx*    enc_xrm_ctx,
                                  XmaEncoderProperties* xma_enc_props,
                                  xrmCuPoolPropertyV2*  enc_cu_pool_prop)
{

    enc_xrm_ctx->enc_load =
        xlnx_xrm_load_calc(enc_xrm_ctx->xrm_ctx, xma_enc_props, "ENCODER");

    if(enc_xrm_ctx->enc_load <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Encoder XRM load calculation failed\n");
        return XMA_APP_ERROR;
    }

    /* If LA is enabled, calculate the load to reserve the CU*/
    if(xma_enc_props->lookahead_depth > 0) {
        XmaFilterProperties filter_props;

        /* Update the lookahead props that are needed for libxmaPropsTOjson */
        filter_props.input.width  = xma_enc_props->width;
        filter_props.input.height = xma_enc_props->height;
        filter_props.input.framerate.numerator =
            xma_enc_props->framerate.numerator;
        filter_props.input.framerate.denominator =
            xma_enc_props->framerate.denominator;

        enc_xrm_ctx->la_load = xlnx_xrm_load_calc(enc_xrm_ctx->xrm_ctx,
                                                  &filter_props, "LOOKAHEAD");
        if(enc_xrm_ctx->la_load <= 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Lookahead XRM load calculation failed\n");
            return XMA_APP_ERROR;
        }
    }

    xlnx_update_pool_props("ENCODER", enc_xrm_ctx->enc_load, 1,
                           enc_cu_pool_prop);

    if(enc_xrm_ctx->la_load) {
        xlnx_update_pool_props("LOOKAHEAD", enc_xrm_ctx->la_load, 0,
                               enc_cu_pool_prop);
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_device_init: Encoder resource allocation and device initialization
 * function
 *
 * @param enc_xrm_ctx: Encoder XRM context
 * @param xma_enc_props: Encoder xma properties
 * @param lookahead_enable: Lookahead enable flag
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_device_init(XlnxEncoderXrmCtx*    enc_xrm_ctx,
                             XmaEncoderProperties* xma_enc_props)
{
    xrmCuPoolPropertyV2 enc_cu_pool_prop;
    int32_t             ret = XMA_APP_ERROR;

    memset(&enc_cu_pool_prop, 0, sizeof(enc_cu_pool_prop));

    /* Create XRM context */
    enc_xrm_ctx->xrm_ctx = xlnx_xrm_create_context();
    if(enc_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "create local XRM context failed\n");
        return ret;
    }

    /* Calculate encoder load based on encoder properties */
    ret = xlnx_enc_load_calc(enc_xrm_ctx, xma_enc_props, &enc_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Enc load calculation failed %d\n", ret);
        return ret;
    }

    ret = xlnx_xrm_device_init(enc_xrm_ctx->xrm_ctx, &xma_enc_props->dev_index,
                               XLNX_ENC_APP_NUM_DEVS, &enc_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Enc device initialization failed %d\n", ret);
        return ret;
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_cu_alloc: Allocates CU for encoder
 *
 * @param enc_xrm_ctx: Encoder XRM context
 * @param xma_enc_props: Encoder XMA properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_cu_alloc(XlnxEncoderXrmCtx*    enc_xrm_ctx,
                          XmaEncoderProperties* xma_enc_props)
{
    int32_t ret = XMA_APP_ERROR;

    ret = xlnx_xrm_cu_alloc(enc_xrm_ctx->xrm_ctx, "ENCODER",
                            enc_xrm_ctx->enc_load, xma_enc_props->dev_index,
                            enc_xrm_ctx->xrm_reserve_id,
                            &enc_xrm_ctx->encode_cu_list_res);

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib =
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index =
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index =
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].cuId;
    xma_enc_props->channel_id =
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].channelId;

    enc_xrm_ctx->enc_res_in_use = 1;

    return ret;
}

/**
 * xlnx_la_allocate_xrm_cu: Allocates XRM CU for lookahead
 *
 * @param enc_xrm_ctx: Encoder XRM context
 * @param xma_la_props: XMA lookahead properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_cu_alloc(XlnxEncoderXrmCtx*   enc_xrm_ctx,
                         XmaFilterProperties* xma_la_props)
{
    int32_t ret = XMA_APP_ERROR;

    ret = xlnx_xrm_cu_alloc(enc_xrm_ctx->xrm_ctx, "LOOKAHEAD",
                            enc_xrm_ctx->enc_load, xma_la_props->dev_index,
                            enc_xrm_ctx->xrm_reserve_id,
                            &enc_xrm_ctx->lookahead_cu_list_res);

    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "xrm_allocation: failed to allocate lookahead cu\n");
        return XMA_APP_ERROR;
    }

    /* Set XMA plugin SO and device index */
    xma_la_props->plugin_lib =
        enc_xrm_ctx->lookahead_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_la_props->dev_index =
        enc_xrm_ctx->lookahead_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_la_props->ddr_bank_index = -1;
    xma_la_props->cu_index =
        enc_xrm_ctx->lookahead_cu_list_res.cuResources[0].cuId;
    xma_la_props->channel_id =
        enc_xrm_ctx->lookahead_cu_list_res.cuResources[0].channelId;

    enc_xrm_ctx->lookahead_res_inuse = 1;

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_xrm_deinit: Deinitializes XRM resources
 *
 * @param enc_xrm_ctx: Encoder XRM context
 * @return None
 */
void xlnx_enc_xrm_deinit(XlnxEncoderXrmCtx* enc_xrm_ctx)
{

    if(enc_xrm_ctx->enc_res_in_use) {
        xrmCuListReleaseV2(enc_xrm_ctx->xrm_ctx,
                           &enc_xrm_ctx->encode_cu_list_res);
        enc_xrm_ctx->enc_res_in_use = 0;
    }
    if(enc_xrm_ctx->lookahead_res_inuse) {
        xrmCuListReleaseV2(enc_xrm_ctx->xrm_ctx,
                           &enc_xrm_ctx->lookahead_cu_list_res);
        enc_xrm_ctx->lookahead_res_inuse = 0;
    }

    xlnx_xrm_deinit(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->xrm_reserve_id);

    return;
}
