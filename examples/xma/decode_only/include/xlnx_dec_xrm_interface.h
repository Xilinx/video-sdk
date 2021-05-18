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
#ifndef _XLNX_DEC_XRM_INTERFACE_H_
#define _XLNX_DEC_XRM_INTERFACE_H_

#include <string.h>
#include <dlfcn.h>

#include <xmaplugin.h>
#include <xrm.h>

#include "xlnx_dec_common.h"
#include "xlnx_app_utils.h"

#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))

typedef struct XlnxDecoderXrmCtx
{
    int                       xrm_reserve_id;
    int                       dec_load;
    int                       decode_res_in_use;
    xrmContext*               xrm_ctx;
    xrmCuListResource         decode_cu_list_res;
} XlnxDecoderXrmCtx;

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_xrm_ctx: Release and relinquish the xrm resources which were 
    reserved and allocated. Destroy the xrm context API.
Parameters:
    dec_xrm_ctx: The xrm decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_xrm_ctx(XlnxDecoderXrmCtx* dec_xrm_ctx);

/*------------------------------------------------------------------------------
xlnx_dec_cu_alloc_device_id: Allocate the xrm cu list based on the requested 
    load for a user specified device id. Set the decoder props plugin name, 
    device index, ddr bank index, cu index, and channel id accordingly.

Parameters:
    dec_xrm_ctx: The xrm wrapper struct for xrm information
    dec_props: The xma decoder properties
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_dec_cu_alloc_device_id(XlnxDecoderXrmCtx* dec_xrm_ctx, 
                                    XmaDecoderProperties* dec_props);

/*------------------------------------------------------------------------------
xlnx_dec_allocate_xrm_dec_cu: Allocate the xrm cu list based on the requested 
    load and set the decoder props plugin name, device index, ddr bank index, 
    cu index, and channel id.

Parameters:
    dec_xrm_ctx: The xrm wrapper struct for xrm information
    dec_props: The xma decoder properties
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_dec_allocate_xrm_dec_cu(XlnxDecoderXrmCtx* dec_xrm_ctx, 
                                     XmaDecoderProperties* dec_props);

/*------------------------------------------------------------------------------
xlnx_dec_reserve_xrm_resource: Get the decoder load and reserve resources for
    xlnx_dec_allocate_xrm_dec_cu.
    
Parameters:
    dec_xrm_ctx: The xrm wrapper struct for xrm information
    dec_props: The xma decoder properties
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_dec_reserve_xrm_resource(XlnxDecoderXrmCtx* dec_xrm_ctx, 
                                      XmaDecoderProperties* dec_props);

#endif