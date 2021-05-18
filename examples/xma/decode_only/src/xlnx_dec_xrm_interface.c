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
#include "xlnx_dec_xrm_interface.h"

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_xrm_ctx: Release and relinquish the xrm resources which were 
    reserved and allocated. Destroy the xrm context API.
Parameters:
    dec_xrm_ctx: The xrm decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_xrm_ctx(XlnxDecoderXrmCtx* dec_xrm_ctx)
{
    if(!dec_xrm_ctx->xrm_ctx) {
        return;
    }
    if(dec_xrm_ctx->decode_res_in_use) {
        /* Release the resource (still reserved) */
        xrmCuListRelease(dec_xrm_ctx->xrm_ctx, 
                        &dec_xrm_ctx->decode_cu_list_res);
        
        dec_xrm_ctx->decode_res_in_use = 0;
    }
    if(dec_xrm_ctx->xrm_reserve_id) {
        /* Put the resource back into the pool of available. */
        xrmCuPoolRelinquish(dec_xrm_ctx->xrm_ctx, 
                            dec_xrm_ctx->xrm_reserve_id); 
    }
    if(dec_xrm_ctx->xrm_ctx) {
        xrmDestroyContext(dec_xrm_ctx->xrm_ctx);
    }
}

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
                                    XmaDecoderProperties* dec_props)
{
    dec_xrm_ctx->decode_res_in_use = 0;
    
    /* XRM decoder allocation */
    xrmCuListProperty decode_cu_list_prop;

    memset(&decode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&dec_xrm_ctx->decode_cu_list_res, 0, sizeof(xrmCuListResource));

    decode_cu_list_prop.cuNum = 2;
    strcpy(decode_cu_list_prop.cuProps[0].kernelName, "decoder");
    strcpy(decode_cu_list_prop.cuProps[0].kernelAlias, "DECODER_MPSOC");
    decode_cu_list_prop.cuProps[0].devExcl = false;
    decode_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(
                                        dec_xrm_ctx->dec_load);
    decode_cu_list_prop.cuProps[0].poolId = dec_xrm_ctx->xrm_reserve_id;

    strcpy(decode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_decoder");
    decode_cu_list_prop.cuProps[1].devExcl = false;
    decode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(
                                        XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    decode_cu_list_prop.cuProps[1].poolId = dec_xrm_ctx->xrm_reserve_id;

    int32_t ret;
    int32_t device_id = dec_props->dev_index;
    ret = xrmCuAllocFromDev(dec_xrm_ctx->xrm_ctx, device_id, 
                            &decode_cu_list_prop.cuProps[0], 
                            &dec_xrm_ctx->decode_cu_list_res.cuResources[0]);
    if(ret != XMA_SUCCESS) {
        DECODER_APP_LOG_ERROR("xrm failed to allocate decoder resources on "
                              "device %d\n", dec_xrm_ctx->xrm_reserve_id);
        return ret;
    }
    ret = xrmCuAllocFromDev(dec_xrm_ctx->xrm_ctx, device_id, 
                            &decode_cu_list_prop.cuProps[1], 
                            &dec_xrm_ctx->decode_cu_list_res.cuResources[1]);
    if(ret != XMA_SUCCESS) {
        DECODER_APP_LOG_ERROR("xrm failed to allocate decoder resources on "
                              "device %d\n", dec_xrm_ctx->xrm_reserve_id);
        return ret;
    }
    dec_xrm_ctx->decode_res_in_use = 1;

    /*Set XMA plugin SO and device index */
    dec_props->plugin_lib     = dec_xrm_ctx->decode_cu_list_res.cuResources[0].
                                kernelPluginFileName;
    /* XMA to select the ddr bank based on xclbin meta data */
    dec_props->ddr_bank_index = -1; 
    dec_props->cu_index       = dec_xrm_ctx->decode_cu_list_res.cuResources[1].
                                cuId;
    dec_props->channel_id     = dec_xrm_ctx->decode_cu_list_res.cuResources[1].
                                channelId; // SW kernel always used 100%
    DECODER_APP_LOG_INFO("Device ID: %d\n", dec_props->dev_index);
    return DEC_APP_SUCCESS;
}

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
                                     XmaDecoderProperties* dec_props)
{
    dec_xrm_ctx->decode_res_in_use = 0;
    /* XRM decoder allocation */
    xrmCuListProperty decode_cu_list_prop;
    memset(&decode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&dec_xrm_ctx->decode_cu_list_res, 0, sizeof(xrmCuListResource));

    decode_cu_list_prop.cuNum = 2;
    strcpy(decode_cu_list_prop.cuProps[0].kernelName, "decoder");
    strcpy(decode_cu_list_prop.cuProps[0].kernelAlias, "DECODER_MPSOC");
    decode_cu_list_prop.cuProps[0].devExcl = false;
    decode_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(
                                        dec_xrm_ctx->dec_load);
    decode_cu_list_prop.cuProps[0].poolId = dec_xrm_ctx->xrm_reserve_id;

    strcpy(decode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_decoder");
    decode_cu_list_prop.cuProps[1].devExcl = false;
    decode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(
                                        XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    decode_cu_list_prop.cuProps[1].poolId = dec_xrm_ctx->xrm_reserve_id;

    if(xrmCuListAlloc(dec_xrm_ctx->xrm_ctx, &decode_cu_list_prop, 
                      &dec_xrm_ctx->decode_cu_list_res) != 0) {
        DECODER_APP_LOG_ERROR("xrm_allocation: fail to allocate cu list "
                              "from reserve id %d\n", 
                              dec_xrm_ctx->xrm_reserve_id);
        return DEC_APP_ERROR;
    }
    dec_xrm_ctx->decode_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    dec_props->plugin_lib     = dec_xrm_ctx->decode_cu_list_res.cuResources[0].
                                kernelPluginFileName;
    dec_props->dev_index      = dec_xrm_ctx->decode_cu_list_res.cuResources[0].
                                deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    dec_props->ddr_bank_index = -1; 
    dec_props->cu_index       = dec_xrm_ctx->decode_cu_list_res.cuResources[1].
                                cuId;
    dec_props->channel_id     = dec_xrm_ctx->decode_cu_list_res.cuResources[1].
                                channelId; // SW kernel always used 100%
    DECODER_APP_LOG_INFO("Device ID: %d\n", 
               dec_props->dev_index);
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
dec_fill_pool_props: Updates decoder CU pool properties

Parameters:
    dec_cu_pool_prop: Decoder CU pool properties
    dec_load: Decoder load

Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t dec_fill_pool_props(xrmCuPoolProperty* dec_cu_pool_prop, 
                                   int dec_load)
{
    int32_t cu_num = 0;
    dec_cu_pool_prop->cuListProp.sameDevice = true;
    dec_cu_pool_prop->cuListNum = 1;
    strcpy(dec_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
           "decoder");
    strcpy(dec_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
           "DECODER_MPSOC");
    dec_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
    dec_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                    XRM_PRECISION_1000000_BIT_MASK(dec_load);
    cu_num++;
    strcpy(dec_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
           "kernel_vcu_decoder");
    strcpy(dec_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, "");
    dec_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
    dec_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                    XRM_PRECISION_1000000_BIT_MASK(
                                        XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    cu_num++;
    dec_cu_pool_prop->cuListProp.cuNum = cu_num;
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
dec_load_calc: Calculates load for decoder module

Parameters:
    dec_xrm_ctx: The xrm wrapper struct for xrm information
    dec_props: The xma decoder properties
    dec_load: The decoder load to be set

Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t dec_load_calc(XlnxDecoderXrmCtx* dec_xrm_ctx, 
                             XmaDecoderProperties* dec_props, int* dec_load)
{
    int32_t func_id = 0;
    int32_t ret;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert decoder properties to json */
    handle = dlopen(XMA_PROPS_TO_JSON_SO, RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (dec_props, "DECODER", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30DecPlugin");

    if((ret = xrmExecPluginFunc(dec_xrm_ctx->xrm_ctx, pluginName, func_id, 
                                &plg_param)) != XRM_SUCCESS) {
        DECODER_APP_LOG_ERROR("XRM decoder plugin failed to calculate decoder "
                              "load. %d\n", ret);
        return DEC_APP_ERROR;
    }
    *dec_load = atoi((char*)(strtok(plg_param.output, " ")));
    return DEC_APP_SUCCESS;
}

/*------------------------------------------------------------------------------
dec_reserve_xrm_device_id: Reserve resources on the device and xma initialize
    
Parameters:
    dec_xrm_ctx: The xrm wrapper struct for xrm information
    dec_props: The xma decoder properties
    dec_cu_pool_prop: Used to reserve the resources
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
static int32_t dec_reserve_xrm_device_id(XlnxDecoderXrmCtx* dec_xrm_ctx, 
                                         XmaDecoderProperties* dec_props,
                                         xrmCuPoolProperty* dec_cu_pool_prop)
{
    int device_id = dec_props->dev_index;
    if(device_id != -1) {
        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = device_id;
        xclbin_param.xclbin_name = XCLBIN_PARAM_NAME; 
        if(xma_initialize(&xclbin_param, 1) != XMA_SUCCESS) {
            DECODER_APP_LOG_ERROR("XMA Initialization failed\n");
            xlnx_dec_cleanup_xrm_ctx(dec_xrm_ctx);
            return DEC_APP_ERROR;
        }
        DECODER_APP_LOG_INFO("XMA initialization success \n");
        return DEC_APP_SUCCESS;
    }
    /* Query XRM to get reservation index for the required CU */
    dec_xrm_ctx->xrm_reserve_id = xrmCuPoolReserve(dec_xrm_ctx->xrm_ctx, 
                                                   dec_cu_pool_prop);
    if(dec_xrm_ctx->xrm_reserve_id == 0) {
        DECODER_APP_LOG_ERROR("xrm_cu_pool_reserve: fail to reserve decode "
                              "cu pool\n");
        xlnx_dec_cleanup_xrm_ctx(dec_xrm_ctx);
        return DEC_APP_ERROR;
    }
    xrmCuPoolResource cu_pool_res;
    memset(&cu_pool_res, 0, sizeof(cu_pool_res));
    DECODER_APP_LOG_INFO("Reservation index %d \n", 
                         dec_xrm_ctx->xrm_reserve_id);
    /* Query XRM for the CU pool resource details like xclbinname and device 
     * ID */
    
    if(xrmReservationQuery(dec_xrm_ctx->xrm_ctx, dec_xrm_ctx->xrm_reserve_id, 
       &cu_pool_res) != 0) {
        DECODER_APP_LOG_ERROR("xrm_reservation_query: fail to query reserved "
                              "cu list\n");
        xlnx_dec_cleanup_xrm_ctx(dec_xrm_ctx);
        return DEC_APP_ERROR;
    }
    /* xclbin configuration */
    XmaXclbinParameter xclbin_param;
    xclbin_param.device_id = cu_pool_res.cuResources[0].deviceId;
    xclbin_param.xclbin_name = cu_pool_res.cuResources[0].xclbinFileName; 

    if((xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS) {
        DECODER_APP_LOG_ERROR("XMA Initialization failed\n");
        xlnx_dec_cleanup_xrm_ctx(dec_xrm_ctx);
        return DEC_APP_ERROR;
    }
    DECODER_APP_LOG_INFO("XMA initialization success \n");
    return DEC_APP_SUCCESS;
}

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
                                      XmaDecoderProperties* dec_props)
{
    xrmCuPoolProperty dec_cu_pool_prop;
    int32_t ret = DEC_APP_ERROR;
    int32_t num_cu_pool = 0;

    memset(&dec_cu_pool_prop, 0, sizeof(dec_cu_pool_prop));

    /* Create XRM context */
    dec_xrm_ctx->xrm_ctx = (xrmContext *)xrmCreateContext(XRM_API_VERSION_1);
    if(dec_xrm_ctx->xrm_ctx == NULL) {
        DECODER_APP_LOG_ERROR("Create local XRM context failed\n");
        return DEC_APP_ERROR;
    }

    /* Calculate decoder load based on decoder properties */
    int dec_load;
    ret = dec_load_calc(dec_xrm_ctx, dec_props, &dec_load);
    if(ret != DEC_APP_SUCCESS) {
        DECODER_APP_LOG_ERROR("Decoder load calculation failed %d \n", ret);
        return ret;
    }
    dec_fill_pool_props(&dec_cu_pool_prop, dec_load);
    dec_xrm_ctx->dec_load = dec_load;

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNum(dec_xrm_ctx->xrm_ctx, 
                                             &dec_cu_pool_prop);
    if(num_cu_pool <= 0) {
        DECODER_APP_LOG_ERROR("No resources available for allocation \n");
        return DEC_APP_ERROR;
    }
    DECODER_APP_LOG_INFO("Num CU pools available %d \n", num_cu_pool);

    /* If the device reservation ID is not sent through command line get the 
     * next available device id */
    dec_reserve_xrm_device_id(dec_xrm_ctx, dec_props, &dec_cu_pool_prop);
    if(ret != DEC_APP_SUCCESS) {
        DECODER_APP_LOG_ERROR("xrm_allocation_query: fail to query allocated "
                              "cu list\n");
        return DEC_APP_ERROR;
    }
    return DEC_APP_SUCCESS;
}
