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
#include "xlnx_scal_xrm_interface.h"

/*------------------------------------------------------------------------------
xlnx_scal_cleanup_xrm_ctx: Release and relinquish the xrm resources which were 
    reserved and allocated. Destroy the xrm context API.
Parameters:
    scaler_xrm_ctx: The xrm scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_cleanup_xrm_ctx(XlnxScaleXrmCtx* scaler_xrm_ctx) 
{
    if(!scaler_xrm_ctx->xrm_ctx) {
        return;
    }
    if(scaler_xrm_ctx->scaler_res_in_use) {
        /* Release the resource (still reserved) */
        xrmCuListRelease(scaler_xrm_ctx->xrm_ctx, 
                         &scaler_xrm_ctx->scaler_cu_list_res);
        
        scaler_xrm_ctx->scaler_res_in_use = 0;
    }
    if(scaler_xrm_ctx->xrm_reserve_id) {
        /* Put the resource back into the pool of available. */
        xrmCuPoolRelinquish(scaler_xrm_ctx->xrm_ctx, 
                            scaler_xrm_ctx->xrm_reserve_id); 
    }
    if(scaler_xrm_ctx->xrm_ctx) {
        xrmDestroyContext(scaler_xrm_ctx->xrm_ctx);
    }
}

/*------------------------------------------------------------------------------
xlnx_scal_cu_alloc_device_id: Allocate the xrm resources based on the requested 
    load for a user specified device id. Set the scaler props plugin name, 
    device index, ddr bank index, cu index, and channel id accordingly.

Parameters:
    scaler_xrm_ctx: The xrm wrapper struct for xrm information
    scaler_xma_props: The xma scaler properties
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_cu_alloc_device_id(XlnxScaleXrmCtx* scaler_xrm_ctx, 
                               XmaScalerProperties* scaler_xma_props) 
{
    scaler_xrm_ctx->scaler_res_in_use = 0;
    
    //XRM scaler allocation
    xrmCuListProperty scaler_cu_list_prop;

    memset(&scaler_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&scaler_xrm_ctx->scaler_cu_list_res, 0, sizeof(xrmCuListResource));

    scaler_cu_list_prop.cuNum = 1;
    strcpy(scaler_cu_list_prop.cuProps[0].kernelName, "scaler");
    strcpy(scaler_cu_list_prop.cuProps[0].kernelAlias, "SCALER_MPSOC");
    scaler_cu_list_prop.cuProps[0].devExcl = false;
    scaler_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(scaler_xrm_ctx->scaler_load);
    scaler_cu_list_prop.cuProps[0].poolId = scaler_xrm_ctx->xrm_reserve_id;

    int32_t ret;
    ret = xrmCuAllocFromDev(scaler_xrm_ctx->xrm_ctx, 
                            scaler_xma_props->dev_index, 
                            &scaler_cu_list_prop.cuProps[0], 
                            &scaler_xrm_ctx->scaler_cu_list_res.cuResources[0]);
    if(ret != XMA_SUCCESS) {
        SCALER_APP_LOG_ERROR("xrm failed to allocate scaler resources on "
                             "device %d\n", scaler_xrm_ctx->xrm_reserve_id);
        return ret;
    }
    
    scaler_xrm_ctx->scaler_res_in_use = 1;

    /* Set XMA plugin SO */
    scaler_xma_props->plugin_lib     = scaler_xrm_ctx->scaler_cu_list_res.
                                       cuResources[0].kernelPluginFileName;
    /* XMA to select the ddr bank based on xclbin meta data */
    scaler_xma_props->ddr_bank_index = -1; 
    scaler_xma_props->cu_index       = scaler_xrm_ctx->scaler_cu_list_res.
                                       cuResources[1].cuId;
    scaler_xma_props->channel_id     = scaler_xrm_ctx->scaler_cu_list_res.
                                       cuResources[1].channelId; 
    /* SW kernel always used 100% */
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_alloc_xrm_scaler_cu: Allocate the xrm cu list based on the requested 
    load and set the scaler props plugin name, device index, ddr bank index, 
    cu index, and channel id.

Parameters:
    scaler_xrm_ctx: The xrm wrapper struct for xrm information
    scaler_xma_props: The xma scaler properties
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_alloc_xrm_scaler_cu(XlnxScaleXrmCtx* scaler_xrm_ctx, 
                                   XmaScalerProperties* scaler_xma_props) 
{
    scaler_xrm_ctx->scaler_res_in_use = 0;
    
    //XRM scaler allocation
    xrmCuListProperty scaler_cu_list_prop;

    memset(&scaler_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&scaler_xrm_ctx->scaler_cu_list_res, 0, sizeof(xrmCuListResource));

    scaler_cu_list_prop.cuNum = 1;
    strcpy(scaler_cu_list_prop.cuProps[0].kernelName, "scaler");
    strcpy(scaler_cu_list_prop.cuProps[0].kernelAlias, "SCALER_MPSOC");
    scaler_cu_list_prop.cuProps[0].devExcl = false;
    scaler_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(scaler_xrm_ctx->scaler_load);
    scaler_cu_list_prop.cuProps[0].poolId = scaler_xrm_ctx->xrm_reserve_id;

    if(xrmCuListAlloc(scaler_xrm_ctx->xrm_ctx, &scaler_cu_list_prop, 
                      &scaler_xrm_ctx->scaler_cu_list_res) != 0) {
        SCALER_APP_LOG_ERROR("xrm_allocation: fail to allocate cu list from "
                             "reserve id %d\n", scaler_xrm_ctx->xrm_reserve_id);
        return SCALER_APP_ERROR;
    }
    scaler_xrm_ctx->scaler_res_in_use = 1;
    /* Set XMA plugin SO and device index */
    scaler_xma_props->plugin_lib  = scaler_xrm_ctx->scaler_cu_list_res.
                                    cuResources[0].kernelPluginFileName;
    if(scaler_xma_props->dev_index == -1) {
        scaler_xma_props->dev_index = scaler_xrm_ctx->scaler_cu_list_res.
                                      cuResources[0].deviceId;
    }
    /* XMA to select the ddr bank based on xclbin meta data */
    scaler_xma_props->ddr_bank_index = -1; 
    scaler_xma_props->cu_index       = scaler_xrm_ctx->scaler_cu_list_res.
                                       cuResources[1].cuId;
    scaler_xma_props->channel_id     = scaler_xrm_ctx->scaler_cu_list_res.
                                       cuResources[1].channelId; 
    /* SW kernel always used 100% */
    return SCALER_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
abr_scaler_fill_pool_props: Updates scaler CU pool properties

parameters:
    sc_cu_pool_prop: Scaler CU pool properties
    scc_load: Scaler load

return:
    XMA_SCALER_APP_SUCCESS or XMA_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t abr_scaler_fill_pool_props(xrmCuPoolProperty* sc_cu_pool_prop, 
                                   int sc_load) 
{
    int32_t cu_num = 0;
    sc_cu_pool_prop->cuListProp.sameDevice = true;
    sc_cu_pool_prop->cuListNum = 1;
    strcpy(sc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, "scaler");
    strcpy(sc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
           "SCALER_MPSOC");
    sc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
    sc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = XRM_PRECISION_1000000_BIT_MASK(sc_load);
    cu_num++;
    sc_cu_pool_prop->cuListProp.cuNum = cu_num;
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_scaler_load_calc: Calculates load for scaler module

Parameters:
        xrm_ctx: The xrm context
        scale_props: Scaler properties
        sc_load: Pointer to scaler load to be filled
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_scaler_load_calc(xrmContext* xrm_ctx, 
                                          XmaScalerProperties* scale_props, 
                                          int* sc_load) 
{
    int32_t func_id = 0;
    int32_t ret;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert scaler properties to json */
    handle = dlopen(XMA_PROPS_TO_JSON_SO, RTLD_NOW);

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (scale_props, "SCALER", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30ScalPlugin");

    if((ret = xrmExecPluginFunc(xrm_ctx, pluginName, func_id, &plg_param)) != 
        XRM_SUCCESS) {
        SCALER_APP_LOG_ERROR("XRM scaler plugin failed to calculate scaler "
                             "load. %d\n", ret);
        return SCALER_APP_ERROR;
    }
    *sc_load = atoi((char*)(strtok(plg_param.output, " ")));
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_reserve_xrm_device_id: Perform the xma initialize and reserve

Parameters:
    scaler_xrm_ctx: The xrm wrapper struct for xrm information
    scaler_xma_props: The xma scaler properties
    sc_cu_pool_prop: The pool props for reservation
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t xlnx_scal_reserve_xrm_device_id(XlnxScaleXrmCtx* scaler_xrm_ctx, 
                                         XmaScalerProperties* scaler_xma_props, 
                                         xrmCuPoolProperty* sc_cu_pool_prop) 
{
    int device_id = scaler_xma_props->dev_index;
    if(device_id != -1) {
        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = device_id;
        xclbin_param.xclbin_name = XCLBIN_PARAM_NAME; 

        SCALER_APP_INFO_LOG("Device ID %d selected to run encoder \n", 
                            device_id);
        if(xma_initialize(&xclbin_param, 1) != XMA_SUCCESS) {
            SCALER_APP_LOG_ERROR("XMA Initialization failed\n");
            xlnx_scal_cleanup_xrm_ctx(scaler_xrm_ctx);
            return SCALER_APP_ERROR;
        }
        SCALER_APP_INFO_LOG("XMA initialization success \n");
        return SCALER_APP_SUCCESS;
    }
    /* Query XRM to get reservation index for the required CU */
    scaler_xrm_ctx->xrm_reserve_id = xrmCuPoolReserve(scaler_xrm_ctx->xrm_ctx, 
                                                      sc_cu_pool_prop);
    if(scaler_xrm_ctx->xrm_reserve_id == 0) {
        SCALER_APP_LOG_ERROR("xrm_cu_pool_reserve: fail to reserve scaler cu "
                             "pool\n");
        xrmDestroyContext(scaler_xrm_ctx->xrm_ctx);
        return SCALER_APP_ERROR;
    }
    xrmCuPoolResource cu_pool_res;
    memset(&cu_pool_res, 0, sizeof(cu_pool_res));
    SCALER_APP_INFO_LOG("Reservation index %d \n", 
                        scaler_xrm_ctx->xrm_reserve_id);
    /* Query XRM for the CU pool resource details like xclbinname and 
    device ID */
    
    if(xrmReservationQuery(scaler_xrm_ctx->xrm_ctx, scaler_xrm_ctx->
                           xrm_reserve_id, &cu_pool_res) != 0) {
        SCALER_APP_LOG_ERROR("xrm_reservation_query: fail to query reserved  "
                             "cu list\n");
        xrmCuPoolRelinquish(scaler_xrm_ctx->xrm_ctx, 
                            scaler_xrm_ctx->xrm_reserve_id);
        xrmDestroyContext(scaler_xrm_ctx->xrm_ctx);
        return SCALER_APP_ERROR;
    }
    /* xclbin configuration */
    XmaXclbinParameter xclbin_param;
    xclbin_param.device_id = cu_pool_res.cuResources[0].deviceId;
    xclbin_param.xclbin_name = cu_pool_res.cuResources[0].xclbinFileName; 

    if((xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS) {
        SCALER_APP_LOG_ERROR("XMA Initialization failed\n");
        xrmCuPoolRelinquish(scaler_xrm_ctx->xrm_ctx, 
                            scaler_xrm_ctx->xrm_reserve_id);
        xrmDestroyContext(scaler_xrm_ctx->xrm_ctx);
        return SCALER_APP_ERROR;
    }
    SCALER_APP_INFO_LOG("XMA initialization success \n");
    return SCALER_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
xlnx_scal_reserve_xrm_resource: Get the scaler load and reserve resources for
    allocating the cu.
    
Parameters:
    scaler_xrm_ctx: The xrm wrapper struct for xrm information
    scaler_xma_props: The xma scaler properties
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_reserve_xrm_resource(XlnxScaleXrmCtx* scaler_xrm_ctx, 
                                 XmaScalerProperties* scaler_xma_props) 
{
    xrmCuPoolProperty sc_cu_pool_prop;
    int32_t ret = SCALER_APP_ERROR;
    int32_t num_cu_pool = 0;

    memset(&sc_cu_pool_prop, 0, sizeof(sc_cu_pool_prop));

    /* Create XRM context */
    scaler_xrm_ctx->xrm_ctx = (xrmContext *)xrmCreateContext(XRM_API_VERSION_1);
    if (scaler_xrm_ctx->xrm_ctx == NULL) {
        SCALER_APP_LOG_ERROR("create local XRM context failed\n");
        return ret;
    }

    /* Calculate scaler load based on scaler properties */
    int sc_load;
    ret = xlnx_scal_scaler_load_calc(scaler_xrm_ctx->xrm_ctx, scaler_xma_props, 
                                     &sc_load);
    if(ret != SCALER_APP_SUCCESS) {
        SCALER_APP_LOG_ERROR("Scaler load calculation failed %d \n", ret);
        return ret;
    }
    scaler_xrm_ctx->scaler_load += sc_load;
    abr_scaler_fill_pool_props(&sc_cu_pool_prop, 
                               scaler_xrm_ctx->scaler_load);

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNum(scaler_xrm_ctx->xrm_ctx, 
                                             &sc_cu_pool_prop);
    if(num_cu_pool <= 0) {
        SCALER_APP_LOG_ERROR("No resources available for allocation \n");
        return SCALER_APP_ERROR;
    }
    SCALER_APP_INFO_LOG("Num CU pools available %d \n", num_cu_pool);

    /* If the device reservation ID is not sent through command line and  
     * RESERVE_ID environment variable is not set , get the next available 
     * device id */
    ret = xlnx_scal_reserve_xrm_device_id(scaler_xrm_ctx, scaler_xma_props, 
                                          &sc_cu_pool_prop);
    if (ret != SCALER_APP_SUCCESS) {
        SCALER_APP_LOG_ERROR("xrm_allocation_query: fail to query allocated "
                             "cu list\n");
    }
    return ret;
}
