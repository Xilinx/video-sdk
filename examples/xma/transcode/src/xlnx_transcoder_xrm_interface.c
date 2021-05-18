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

#include "xlnx_transcoder_xrm_interface.h"

/*-----------------------------------------------------------------------------
xlnx_tran_fill_pool_props: Updates transcoder CU pool properties

Parameters:
transcode_cu_pool_prop: Encoder CU pool properties
transcode_Load: Transcoder load

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_fill_pool_props(
        xrmCuPoolProperty *transcode_cu_pool_prop, 
        XlnxTranscoderLoad *transcode_Load)
{
    int32_t cu_num = 0;
    transcode_cu_pool_prop->cuListProp.sameDevice = true;
    transcode_cu_pool_prop->cuListNum = 1;

    if (transcode_Load->dec_load > 0) {
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "decoder");
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "DECODER_MPSOC");
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
            XRM_PRECISION_1000000_BIT_MASK(transcode_Load->dec_load);
        cu_num++;
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "kernel_vcu_decoder");
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias,
                "");
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
        cu_num++;
    }

    if (transcode_Load->scal_load > 0) {
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "scaler");
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "SCALER_MPSOC");
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
            XRM_PRECISION_1000000_BIT_MASK(transcode_Load->scal_load);
        cu_num++;
    }

    if (transcode_Load->enc_load > 0) {
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "encoder");
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "ENCODER_MPSOC");
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
            XRM_PRECISION_1000000_BIT_MASK(transcode_Load->enc_load);
        cu_num++;

        for(int32_t i = 0; i < transcode_Load->enc_num; i++) {
            strcpy(transcode_cu_pool_prop->
                cuListProp.cuProps[cu_num].kernelName, "kernel_vcu_encoder");
            strcpy(transcode_cu_pool_prop->
                cuListProp.cuProps[cu_num].kernelAlias, "");
            transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
            transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
                                     XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
            cu_num++;
        }
    }

    if(transcode_Load->la_load > 0) {
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelName, 
                "lookahead");
        strcpy(transcode_cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, 
                "LOOKAHEAD_MPSOC");
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        transcode_cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad = 
            XRM_PRECISION_1000000_BIT_MASK(transcode_Load->la_load);
        cu_num++;

    }

    transcode_cu_pool_prop->cuListProp.cuNum = cu_num;
    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_tran_convert_xma_props_to_json: Converts the props to json for the given
    function name

Parameters:
    props: The xma props, be they encoder, decoder, or scaler props
    func_name: The name of the function - ENCODER, DECODER, LOOKAHEAD, SCALER
    json_job: json props

Return:
    TRANSCODE_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_convert_xma_props_to_json(void* props, char* func_name,
                                                   char* json_job) {
    void* handle;
    /* Loading propstojson library to convert xma properties to json */
    void (*convert_xma_props_to_json)(void* props, char* func_name, char* json_job);
    handle = dlopen(XMA_PROPS_TO_JSON_SO, RTLD_NOW);
    convert_xma_props_to_json = dlsym(handle, "convertXmaPropsToJson");
    (*convert_xma_props_to_json) (props, func_name, json_job);
    dlclose(handle);
    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_tran_enc_la_load: Calculates load for lookahead module

Parameters:
xrm_ctx: XRM context
enc_ctx: Encoder context

Return:
Lookahead load
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_enc_la_load(xrmContext *xrm_ctx, 
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
    filter_props.input.framerate.denominator = xma_enc_props->framerate.
                                               denominator;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    xlnx_tran_convert_xma_props_to_json(&filter_props, "LOOKAHEAD", plg_param.input);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(xrm_ctx, pluginName, func_id, &plg_param) 
            != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "XRM LA plugin failed \n");
        return TRANSCODE_APP_FAILURE;
    }
    else {
        skip_value = atoi((char *)(strtok(plg_param.output, " ")));
        skip_value = atoi((char *)(strtok(NULL, " ")));
        /* To silence the warning of skip_value set, but not used */
        (void)skip_value;
        la_load = atoi((char *)(strtok(NULL, " ")));
    }
    return la_load;

}

/*-----------------------------------------------------------------------------
xlnx_tran_enc_load_calc: Calculates load for encoder module

Parameters:
xrm_ctx: XRM context
xma_enc_props: XMA encoder properties
enc_num: Number of encoder soft kernels needed

Return:
Encoder load
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_enc_load_calc(xrmContext *xrm_ctx, 
        XmaEncoderProperties *xma_enc_props, 
        int32_t *enc_num)
{

    int32_t enc_load = 0;
    int32_t func_id = 0;
    int32_t ret;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    xlnx_tran_convert_xma_props_to_json(xma_enc_props, "ENCODER", plg_param.input);

    strcpy(pluginName, "xrmU30EncPlugin");

    if ((ret = xrmExecPluginFunc(xrm_ctx, pluginName, func_id, &plg_param)) 
            != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "XRM encoder plugin failed %d \n", ret);
        return TRANSCODE_APP_FAILURE;
    }
    else {
        enc_load = atoi((char*)(strtok(plg_param.output, " ")));
        *enc_num += atoi((char*)(strtok(NULL, " ")));
    }
    return enc_load;
}

/*-----------------------------------------------------------------------------
xlnx_tran_scal_load_calc: Calculates load for scaler module

Parameters:
xrm_ctx: XRM context
xma_scal_props: XMA scaler properties

Return:
Scaler load
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_scal_load_calc(xrmContext *xrm_ctx, 
        XmaScalerProperties *xma_scal_props)
{

    int32_t scal_load = 0;
    int32_t func_id = 0;
    int32_t ret;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    xlnx_tran_convert_xma_props_to_json(xma_scal_props, "SCALER", plg_param.input);

    strcpy(pluginName, "xrmU30ScalPlugin");

    if ((ret = xrmExecPluginFunc(xrm_ctx, pluginName, func_id, &plg_param)) 
            != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "XRM scaler plugin failed %d \n", ret);
        return TRANSCODE_APP_FAILURE;
    }
    else {
        scal_load = atoi((char*)(strtok(plg_param.output, " ")));
    }
    return scal_load;
}

/*-----------------------------------------------------------------------------
xlnx_tran_dec_load_calc: Calculates load for decoder module

Parameters:
xrm_ctx: XRM context
xma_dec_props: XMA decoder properties

Return:
Decoder load
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_dec_load_calc(xrmContext *xrm_ctx, 
        XmaDecoderProperties *xma_dec_props)
{

    int32_t dec_load = 0;
    int32_t func_id = 0;
    int32_t ret;
    char pluginName[XRM_MAX_NAME_LEN];
    xrmPluginFuncParam plg_param;
    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    xlnx_tran_convert_xma_props_to_json(xma_dec_props, "DECODER", 
                                        plg_param.input);
    strcpy(pluginName, "xrmU30DecPlugin");

    if ((ret = xrmExecPluginFunc(xrm_ctx, pluginName, func_id, &plg_param)) 
            != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "XRM decoder plugin failed %d \n", ret);
        return TRANSCODE_APP_FAILURE;
    }
    else {
        dec_load = atoi((char*)(strtok(plg_param.output, " ")));
    }
    return dec_load;
}

/*-----------------------------------------------------------------------------
xlnx_tran_load_calc: Calculates transcoder load and updates CU pool properties

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_dec_props: Decoder XMA properties
xma_scal_props: Scaler XMA properties
xma_enc_props: Encoder XMA properties
transcode_cu_pool_prop: Transcoder CU pool properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_load_calc(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                                   XmaDecoderProperties *xma_dec_props,
                                   XmaScalerProperties  *xma_scal_props,
                                   XmaEncoderProperties *xma_enc_props, 
                                   xrmCuPoolProperty *transcode_cu_pool_prop)
{
    XlnxTranscoderLoad *transcode_load = &app_xrm_ctx->transcode_load;
    transcode_load->dec_load  = xlnx_tran_dec_load_calc(app_xrm_ctx->xrm_ctx, 
                                                        xma_dec_props);
    if(app_xrm_ctx->num_scal_out) {
        transcode_load->scal_load = xlnx_tran_scal_load_calc(app_xrm_ctx->
                                 xrm_ctx, xma_scal_props);

        if(app_xrm_ctx->num_fullrate_out > 0) {
            xma_scal_props->num_outputs = app_xrm_ctx->num_fullrate_out;
            transcode_load->scal_load += xlnx_tran_scal_load_calc(app_xrm_ctx->
                                 xrm_ctx, xma_scal_props);
        }
    }
    for(int32_t i = 0; i < app_xrm_ctx->num_enc_channels; i++) {
        transcode_load->enc_load += xlnx_tran_enc_load_calc(app_xrm_ctx->
                xrm_ctx, &xma_enc_props[i], &transcode_load->enc_num);

        /* If LA is enabled, calculate the load to reserve the CU*/
        if(app_xrm_ctx->la_enable) {
            transcode_load->la_load += xlnx_tran_enc_la_load(app_xrm_ctx->
                                                 xrm_ctx, &xma_enc_props[i]);
        }
    }
    xlnx_tran_fill_pool_props(transcode_cu_pool_prop, transcode_load);
    return TRANSCODE_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_tran_xma_init: Transcoder resource allocation and device 
initialization function

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_dec_props: Decoder XMA properties
xma_scal_props: Scaler XMA properties
xma_enc_props: Encoder XMA properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_tran_xma_init(XlnxTranscoderXrmCtx *app_xrm_ctx,
                           XmaDecoderProperties *xma_dec_props,
                           XmaScalerProperties  *xma_scal_props,
                           XmaEncoderProperties *xma_enc_props)
{

    xrmCuPoolProperty transcode_cu_pool_prop;
    int32_t ret = TRANSCODE_APP_FAILURE;
    int32_t num_cu_pool = 0;

    memset(&transcode_cu_pool_prop, 0, sizeof(transcode_cu_pool_prop));

    /* Create XRM context */
    app_xrm_ctx->xrm_ctx = (xrmContext *)xrmCreateContext(XRM_API_VERSION_1);
    if (app_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "create local XRM context failed\n");
        return ret;
    }

    /* Calculate encoder load based on encoder properties */
    ret = xlnx_tran_load_calc(app_xrm_ctx, xma_dec_props, xma_scal_props, 
                              xma_enc_props, &transcode_cu_pool_prop);
    if(ret != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Transcoder load calculation failed %d \n", ret);
        xrmDestroyContext(app_xrm_ctx->xrm_ctx);
        return ret;
    }

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNum(app_xrm_ctx->xrm_ctx, 
            &transcode_cu_pool_prop);
    if(num_cu_pool <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "No resources available for allocation \n");
        return TRANSCODE_APP_FAILURE;
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Num CU pools available %d \n", num_cu_pool);

    /* If the device reservation ID is not sent through command line, get the 
       next available device id */
    if(app_xrm_ctx->device_id == -1) {

        /* Query XRM to get reservation index for the required CU */
        app_xrm_ctx->reserve_idx = xrmCuPoolReserve(
                app_xrm_ctx->xrm_ctx, &transcode_cu_pool_prop);
        if (app_xrm_ctx->reserve_idx == 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "xrm_cu_pool_reserve: fail to reserve encode cu pool\n");
            xrmDestroyContext(app_xrm_ctx->xrm_ctx);
            return TRANSCODE_APP_FAILURE;
        }
        xrmCuPoolResource transcode_cu_pool_res;
        memset(&transcode_cu_pool_res, 0, sizeof(transcode_cu_pool_res));
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Reservation index %d \n", app_xrm_ctx->reserve_idx);
        /* Query XRM for the CU resource details like xclbin name and 
            device ID */
        ret = xrmReservationQuery(app_xrm_ctx->xrm_ctx, 
                app_xrm_ctx->reserve_idx, &transcode_cu_pool_res);
        if (ret != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
            "xrm_reservation_query: fail to query reserved  cu list\n");
            xrmCuPoolRelinquish(app_xrm_ctx->xrm_ctx, 
                    app_xrm_ctx->reserve_idx);
            xrmDestroyContext(app_xrm_ctx->xrm_ctx);
            return TRANSCODE_APP_FAILURE;
        }
        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = transcode_cu_pool_res.cuResources[0].deviceId;
        xclbin_param.xclbin_name = transcode_cu_pool_res.cuResources[0].
                                   xclbinFileName; 

        if ((ret = xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "XMA Initialization failed\n");
            xrmCuPoolRelinquish(app_xrm_ctx->xrm_ctx, 
                                app_xrm_ctx->reserve_idx);
            xrmDestroyContext(app_xrm_ctx->xrm_ctx);
            return TRANSCODE_APP_FAILURE;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "XMA initialization success \n");
    } else {

        /* xclbin configuration */
        XmaXclbinParameter xclbin_param;
        xclbin_param.device_id = app_xrm_ctx->device_id;
        xclbin_param.xclbin_name = XCLBIN_PARAM_NAME; 
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Device ID %d selected to run transcoder \n", 
            app_xrm_ctx->device_id);
        if ((ret = xma_initialize(&xclbin_param, 1)) != XMA_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "XMA Initialization failed\n");
            xrmDestroyContext(app_xrm_ctx->xrm_ctx);
            return TRANSCODE_APP_FAILURE;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "XMA initialization success \n");
    }

    if (ret != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xrm_allocation_query: fail to query allocated cu list\n");
        xrmDestroyContext(app_xrm_ctx->xrm_ctx);
        return TRANSCODE_APP_FAILURE;
    }
    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_tran_xrm_deinit: Deinitializes XRM resources

Parameters:
app_xrm_ctx: Transcoder XRM context

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_tran_xrm_deinit(XlnxTranscoderXrmCtx *app_xrm_ctx)
{

    if((app_xrm_ctx->device_id < 0) && (app_xrm_ctx->reserve_idx >= 0)) {
        xrmCuPoolRelinquish(app_xrm_ctx->xrm_ctx, app_xrm_ctx->reserve_idx);
    }

    xrmDestroyContext(app_xrm_ctx->xrm_ctx);
    return;
}
