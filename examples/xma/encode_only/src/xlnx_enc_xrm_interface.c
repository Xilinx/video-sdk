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

#include "xlnx_enc_xrm_interface.h"

/*-----------------------------------------------------------------------------
xlnx_enc_fill_pool_props: Updates encoder CU pool properties

Parameters:
enc_cu_pool_prop: Encoder CU pool properties
enc_xrm_ctx: Encoder XRM context

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_fill_pool_props(xrmCuPoolProperty *enc_cu_pool_prop, 
                                        XlnxEncoderXrmCtx *enc_xrm_ctx)
{
    int32_t cu_num = 0;
    enc_cu_pool_prop->cuListProp.sameDevice = true;
    enc_cu_pool_prop->cuListNum = 1;

    if (enc_xrm_ctx->enc_load > 0) {
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
               "encoder");
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
               "ENCODER_MPSOC");
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                                     XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);
        cu_num++;

        for(int32_t i = 0; i < enc_xrm_ctx->enc_num; i++) {
            strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                   "kernel_vcu_encoder");
            strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                   "");
            enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
            enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
            cu_num++;
        }
    }

    if(enc_xrm_ctx->la_load > 0) {
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "lookahead");
        strcpy(enc_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "LOOKAHEAD_MPSOC");
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        enc_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                                     XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->la_load);
        cu_num++;

    }

    enc_cu_pool_prop->cuListProp.cuNum = cu_num;
    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_la_load_calc: Calculates load for lookahead module

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder XMA properties

Return:
lookahead load
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_load_calc(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                 XmaEncoderProperties *xma_enc_props)
{

    int32_t la_load = 0;
    int32_t skip_value = 0;
    int32_t func_id = 0;
    char pluginName[XRM_MAX_NAME_LEN];
    XmaFilterProperties filter_props;
    xrmPluginFuncParam plg_param;

    /* Update the lookahead props that are needed for libxmaPropsTOjson */
    filter_props.input.width = xma_enc_props->width;
    filter_props.input.height = xma_enc_props->height;
    filter_props.input.framerate.numerator = xma_enc_props->framerate.numerator;
    filter_props.input.framerate.denominator = 
                                 xma_enc_props->framerate.denominator;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert LA properties to json */
    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (&filter_props, "LOOKAHEAD", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(enc_xrm_ctx->xrm_ctx, pluginName, func_id, &plg_param) 
                          != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "XRM LA plugin failed \n");
        return ENC_APP_FAILURE;
    }
    else
    {
        skip_value = atoi((char *)(strtok(plg_param.output, " ")));
        skip_value = atoi((char *)(strtok(NULL, " ")));
        /* To silence the warning of skip_value set, but not used */
        (void)skip_value;
        la_load = atoi((char *)(strtok(NULL, " ")));
    }
    return la_load;

}

/*-----------------------------------------------------------------------------
xlnx_enc_load_calc: Calculates encoder and lookahead load and updates CU pool 
                    properties

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder xma properties
lookahead_enable: Lookahead enable flag
enc_cu_pool_prop: Encoder CU pool properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_load_calc(XlnxEncoderXrmCtx *enc_xrm_ctx,
                                  XmaEncoderProperties *xma_enc_props, 
                                  int32_t lookahead_enable,
                                  xrmCuPoolProperty *enc_cu_pool_prop)
{

    int32_t func_id = 0;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    /* Loading propstojson library to convert encoder properties to json */
    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (xma_enc_props, "ENCODER", plg_param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(enc_xrm_ctx->xrm_ctx, pluginName, func_id, 
                          &plg_param) != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "XRM encoder plugin failed \n");
        return ENC_APP_FAILURE;
    }
    else {
        enc_xrm_ctx->enc_load = atoi((char*)(strtok(plg_param.output, " ")));
        enc_xrm_ctx->enc_num = atoi((char*)(strtok(NULL, " ")));
    }

    /* If LA is enabled, calculate the load to reserve the CU*/
    if(lookahead_enable) {
        enc_xrm_ctx->la_load = xlnx_la_load_calc(enc_xrm_ctx, xma_enc_props);
        if(enc_xrm_ctx->la_load <= 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Lookahead XRM load calculation failed \n");
            return ENC_APP_FAILURE;
        }
    }
    xlnx_enc_fill_pool_props(enc_cu_pool_prop, enc_xrm_ctx);

    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_enc_device_init: Encoder resource allocation and device initialization 
                      function

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder xma properties
lookahead_enable: Lookahead enable flag

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_device_init(XlnxEncoderXrmCtx *enc_xrm_ctx,
                        XmaEncoderProperties *xma_enc_props,
                        int32_t lookahead_enable)
{

    xrmCuPoolProperty enc_cu_pool_prop;
    int32_t ret = ENC_APP_FAILURE;
    int32_t num_cu_pool = 0;

    memset(&enc_cu_pool_prop, 0, sizeof(enc_cu_pool_prop));

    /* Create XRM context */
    enc_xrm_ctx->xrm_ctx = (xrmContext *)xrmCreateContext(XRM_API_VERSION_1);
    if (enc_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "create local XRM context failed\n");
        return ret;
    }

    /* Calculate encoder load based on encoder properties */
    ret = xlnx_enc_load_calc(enc_xrm_ctx, xma_enc_props, lookahead_enable, 
                             &enc_cu_pool_prop);
    if(ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Enc load calculation failed %d \n", ret);
        return ret;
    }

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNum(enc_xrm_ctx->xrm_ctx, 
            &enc_cu_pool_prop);
    if(num_cu_pool <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "No resources available for allocation \n");
        return ENC_APP_FAILURE;
    }

    /* If the device reservation ID is not sent through command line, get the
       next available device id */
    if(enc_xrm_ctx->device_id < 0) {

        /* Query XRM to get reservation index for the required CU */
        enc_xrm_ctx->enc_res_idx = xrmCuPoolReserve(enc_xrm_ctx->xrm_ctx, 
                &enc_cu_pool_prop);
        if (enc_xrm_ctx->enc_res_idx == 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Failed to reserve encode cu pool\n");
            return ENC_APP_FAILURE;
        }
        xrmCuPoolResource enc_cu_pool_res;
        memset(&enc_cu_pool_res, 0, sizeof(enc_cu_pool_res));

        /* Query XRM for the CU pool resource details like xclbinname and 
           device ID */
        ret = xrmReservationQuery(enc_xrm_ctx->xrm_ctx, 
              enc_xrm_ctx->enc_res_idx, &enc_cu_pool_res);
        if (ret != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Failed to query reserved  cu list\n");
            return ENC_APP_FAILURE;
        }

        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = enc_cu_pool_res.cuResources[0].deviceId;
        xclbin_param.xclbin_name = 
                                 enc_cu_pool_res.cuResources[0].xclbinFileName; 

        if ((ret = xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "XMA Initialization failed\n");
            return ENC_APP_FAILURE;
        }
        xma_logmsg(XMA_NOTICE_LOG, XLNX_ENC_APP_MODULE, 
                "XMA initialization success \n");
    }
    else {
        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = enc_xrm_ctx->device_id;
        xclbin_param.xclbin_name = "/opt/xilinx/xcdr/xclbins/transcode.xclbin"; 

        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
            "Device ID %d selected to run encoder \n", enc_xrm_ctx->device_id);
        if ((ret = xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                    "XMA Initialization failed\n");
            return ENC_APP_FAILURE;
        }
        else {
            xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                    "XMA initialization success \n");
        }

    }

    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_enc_xrm_deinit: Deinitializes XRM resources

Parameters:
enc_xrm_ctx: Encoder XRM context

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_enc_xrm_deinit(XlnxEncoderXrmCtx *enc_xrm_ctx)
{

    if(enc_xrm_ctx->enc_res_in_use) {
        xrmCuListRelease(enc_xrm_ctx->xrm_ctx, 
                         &enc_xrm_ctx->encode_cu_list_res);
    }

    if((enc_xrm_ctx->device_id < 0) && (enc_xrm_ctx->enc_res_idx >= 0)) {
        xrmCuPoolRelinquish(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->enc_res_idx);
    }

    if (enc_xrm_ctx->lookahead_res_inuse ==1) {
        xrmCuRelease(enc_xrm_ctx->xrm_ctx, &enc_xrm_ctx->lookahead_cu_res);
    }

    xrmDestroyContext(enc_xrm_ctx->xrm_ctx);
    return;
}