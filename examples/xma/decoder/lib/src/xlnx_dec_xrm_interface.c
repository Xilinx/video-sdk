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
#include "xlnx_dec_xrm_interface.h"

/**
 * xlnx_dec_cleanup_xrm_ctx: Release and relinquish the xrm resources which were
 * reserved and allocated. Destroy the xrm context API.
 * @param dec_xrm_ctx: The xrm decoder context
 */
void xlnx_dec_cleanup_xrm_ctx(XlnxDecoderXrmCtx* dec_xrm_ctx)
{
    if(!dec_xrm_ctx->xrm_ctx) {
        return;
    }
    if(dec_xrm_ctx->decode_res_in_use) {
        /* Release the resource (still reserved) */
        xrmCuListReleaseV2(dec_xrm_ctx->xrm_ctx,
                           &dec_xrm_ctx->decode_cu_list_res);

        dec_xrm_ctx->decode_res_in_use = 0;
    }

    xlnx_xrm_deinit(dec_xrm_ctx->xrm_ctx, dec_xrm_ctx->xrm_reserve_id);
}

/**
 * xlnx_dec_alloc_xrm_cu: Allocate the xrm cu list and set the decoder props
 * plugin name, device index, ddr bank index, cu index, and channel id.
 *
 * @param dec_xrm_ctx: The xrm wrapper struct for xrm information
 * @param xma_dec_props: The xma decoder properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_dec_alloc_xrm_cu(XlnxDecoderXrmCtx*    dec_xrm_ctx,
                              XmaDecoderProperties* xma_dec_props)
{
    int32_t ret = XMA_APP_ERROR;
    memset(&dec_xrm_ctx->decode_cu_list_res, 0, sizeof(xrmCuListResourceV2));

    ret = xlnx_xrm_cu_alloc(dec_xrm_ctx->xrm_ctx, "DECODER",
                            dec_xrm_ctx->dec_load, xma_dec_props->dev_index,
                            dec_xrm_ctx->xrm_reserve_id,
                            &dec_xrm_ctx->decode_cu_list_res);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Fail to allocate decoder cu list %d\n", ret);
        return ret;
    }

    dec_xrm_ctx->decode_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_dec_props->plugin_lib =
        dec_xrm_ctx->decode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_dec_props->dev_index =
        dec_xrm_ctx->decode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_dec_props->ddr_bank_index = -1;
    xma_dec_props->cu_index =
        dec_xrm_ctx->decode_cu_list_res.cuResources[1].cuId;
    xma_dec_props->channel_id = dec_xrm_ctx->decode_cu_list_res.cuResources[1]
                                    .channelId; // SW kernel always used 100%
    xma_logmsg(XMA_INFO_LOG, XLNX_DEC_APP_MODULE, "Device ID: %d\n",
               xma_dec_props->dev_index);
    return ret;
}

/**
 * xlnx_dec_reserve_xrm_resource: Get the decoder load and reserve resources for
 * xlnx_dec_allocate_xrm_dec_cu.
 * @param dec_xrm_ctx: The xrm wrapper struct for xrm information
 * @param xma_dec_props: The xma decoder properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_dec_reserve_xrm_resource(XlnxDecoderXrmCtx*    dec_xrm_ctx,
                                      XmaDecoderProperties* xma_dec_props)
{
    xrmCuPoolPropertyV2 dec_cu_pool_prop;
    int32_t             ret             = XMA_APP_ERROR;
    int8_t              num_soft_kernel = 1;

    memset(&dec_cu_pool_prop, 0, sizeof(dec_cu_pool_prop));

    /* Create XRM context */
    dec_xrm_ctx->xrm_ctx = xlnx_xrm_create_context();
    if(dec_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Create local XRM context failed\n");
        return XMA_APP_ERROR;
    }

    /* Calculate decoder load based on decoder properties */
    dec_xrm_ctx->dec_load =
        xlnx_xrm_load_calc(dec_xrm_ctx->xrm_ctx, xma_dec_props, "DECODER");
    if(dec_xrm_ctx->dec_load <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Decoder load calculation failed %d\n", ret);
        return ret;
    }

    xlnx_update_pool_props("DECODER", dec_xrm_ctx->dec_load, num_soft_kernel,
                           &dec_cu_pool_prop);

    ret = xlnx_xrm_device_init(dec_xrm_ctx->xrm_ctx, &xma_dec_props->dev_index,
                               XLNX_DEC_APP_NUM_DEVS, &dec_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Decoder device initialization failed %d\n", ret);
        return ret;
    }

    return XMA_APP_SUCCESS;
}
