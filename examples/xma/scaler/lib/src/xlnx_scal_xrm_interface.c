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
#include "xlnx_scal_xrm_interface.h"

/**
 * xlnx_scal_cleanup_xrm_ctx: Release and relinquish the xrm resources which
 * were reserved and allocated. Destroy the xrm context API.
 * @param scaler_xrm_ctx: The xrm scaler context
 */
void xlnx_scal_cleanup_xrm_ctx(XlnxScaleXrmCtx* scaler_xrm_ctx)
{
    if(!scaler_xrm_ctx->xrm_ctx) {
        return;
    }
    if(scaler_xrm_ctx->scaler_res_in_use) {
        /* Release the resource (still reserved) */
        xrmCuListReleaseV2(scaler_xrm_ctx->xrm_ctx,
                           &scaler_xrm_ctx->scaler_cu_list_res);

        scaler_xrm_ctx->scaler_res_in_use = 0;
    }

    xlnx_xrm_deinit(scaler_xrm_ctx->xrm_ctx, scaler_xrm_ctx->xrm_reserve_id);
}

/**
 * xlnx_scal_alloc_xrm_cu: Allocate the xrm cu list and set the scaler props
 * plugin name, device index, ddr bank index, cu index, and channel id.
 *
 * @param scaler_xrm_ctx: The xrm wrapper struct for xrm information
 * @param xma_scaler_props: The xma scaler properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_alloc_xrm_cu(XlnxScaleXrmCtx*     scaler_xrm_ctx,
                               XmaScalerProperties* xma_scaler_props)
{
    int32_t ret = XMA_APP_ERROR;
    memset(&scaler_xrm_ctx->scaler_cu_list_res, 0, sizeof(xrmCuListResourceV2));

    ret = xlnx_xrm_cu_alloc(
        scaler_xrm_ctx->xrm_ctx, "SCALER", scaler_xrm_ctx->scaler_load,
        xma_scaler_props->dev_index, scaler_xrm_ctx->xrm_reserve_id,
        &scaler_xrm_ctx->scaler_cu_list_res);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Fail to allocate scaler cu list %d\n", ret);
        return ret;
    }

    scaler_xrm_ctx->scaler_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_scaler_props->plugin_lib =
        scaler_xrm_ctx->scaler_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_scaler_props->dev_index =
        scaler_xrm_ctx->scaler_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_scaler_props->ddr_bank_index = -1;
    xma_scaler_props->cu_index =
        scaler_xrm_ctx->scaler_cu_list_res.cuResources[0].cuId;
    xma_scaler_props->channel_id =
        scaler_xrm_ctx->scaler_cu_list_res.cuResources[0].channelId;

    return ret;
}

/**
 * xlnx_scal_reserve_xrm_resource: Get the scaler load and reserve resources for
 * allocating the cu.
 *
 * @param scaler_xrm_ctx: The xrm wrapper struct for xrm information
 * @param xma_scaler_props: The xma scaler properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_reserve_xrm_resource(XlnxScaleXrmCtx*     scaler_xrm_ctx,
                                       XmaScalerProperties* xma_scaler_props)
{
    xrmCuPoolPropertyV2 sc_cu_pool_prop;
    int32_t             ret             = XMA_APP_ERROR;
    int8_t              num_soft_kernel = 0;

    memset(&sc_cu_pool_prop, 0, sizeof(sc_cu_pool_prop));

    /* Create XRM context */
    scaler_xrm_ctx->xrm_ctx = xlnx_xrm_create_context();
    if(scaler_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "create local XRM context failed\n");
        return ret;
    }

    /* Calculate scaler load based on scaler properties */
    scaler_xrm_ctx->scaler_load =
        xlnx_xrm_load_calc(scaler_xrm_ctx->xrm_ctx, xma_scaler_props, "SCALER");
    if(scaler_xrm_ctx->scaler_load <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Scaler load calculation failed %d\n", ret);
        return ret;
    }

    xlnx_update_pool_props("SCALER", scaler_xrm_ctx->scaler_load,
                           num_soft_kernel, &sc_cu_pool_prop);

    ret = xlnx_xrm_device_init(scaler_xrm_ctx->xrm_ctx,
                               &xma_scaler_props->dev_index,
                               XLNX_SCAL_APP_NUM_DEVS, &sc_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Scaler device initialization failed %d\n", ret);
        return ret;
    }

    return ret;
}
