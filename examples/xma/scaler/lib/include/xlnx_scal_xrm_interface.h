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
#ifndef _XLNX_SCAL_XRM_INTERFACE_H_
#define _XLNX_SCAL_XRM_INTERFACE_H_

#include <dlfcn.h>
#include <string.h>

#include "xlnx_app_utils.h"
#include "xlnx_scal_common.h"
#include "xlnx_xrm_utils.h"

#include <xma.h>
#include <xmaplugin.h>
#include <xrm.h>

#define XLNX_SCAL_APP_NUM_DEVS 1

typedef struct XlnxScaleXrmCtx {
    int                 xrm_reserve_id;
    int                 scaler_load;
    int                 scaler_res_in_use;
    xrmContext          xrm_ctx;
    xrmCuListResourceV2 scaler_cu_list_res;
} XlnxScaleXrmCtx;

/**
 * xlnx_scal_cleanup_xrm_ctx: Release and relinquish the xrm resources which
 * were reserved and allocated. Destroy the xrm context API.
 * @param scaler_xrm_ctx: The xrm scaler context
 */
void xlnx_scal_cleanup_xrm_ctx(XlnxScaleXrmCtx* scaler_xrm_ctx);

/**
 * xlnx_scal_cu_alloc_device_id: Allocate the xrm resources based on the
 * requested load for a user specified device id. Set the scaler props plugin
 * name, device index, ddr bank index, cu index, and channel id accordingly.
 *
 * @param scaler_xrm_ctx: The xrm wrapper struct for xrm information
 * @param scaler_xma_props: The xma scaler properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_cu_alloc_device_id(XlnxScaleXrmCtx*     scaler_xrm_ctx,
                                     XmaScalerProperties* scaler_xma_props);

/**
 * xlnx_scal_alloc_xrm_cu: Allocate the xrm cu list based on the requested
 * load and set the scaler props plugin name, device index, ddr bank index,
 * cu index, and channel id.
 *
 * @param scaler_xrm_ctx: The xrm wrapper struct for xrm information
 * @param scaler_xma_props: The xma scaler properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_alloc_xrm_cu(XlnxScaleXrmCtx*     scaler_xrm_ctx,
                               XmaScalerProperties* scaler_xma_props);

/**
 * xlnx_scal_reserve_xrm_resource: Get the scaler load and reserve resources for
 * allocating the cu.
 *
 * @param scaler_xrm_ctx: The xrm wrapper struct for xrm information
 * @param scaler_xma_props: The xma scaler properties
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_scal_reserve_xrm_resource(XlnxScaleXrmCtx*     scaler_xrm_ctx,
                                       XmaScalerProperties* scaler_xma_props);

#endif
