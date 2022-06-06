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

#include "xlnx_xrm_utils.h"

XlnxIPMapping xlnx_ip_mapping[XRM_MAX_NUM_IP] = {
    {"DECODER", "xrmU30DecPlugin", "decoder", "DECODER_MPSOC",
     "kernel_vcu_decoder", 1},
    {"SCALER", "xrmU30ScalPlugin", "scaler", "SCALER_MPSOC", "", 0},
    {"LOOKAHEAD", "xrmU30EncPlugin", "lookahead", "LOOKAHEAD_MPSOC", "", 0},
    {"ENCODER", "xrmU30EncPlugin", "encoder", "ENCODER_MPSOC",
     "kernel_vcu_encoder", 1}};

/**
 * xlnx_xrm_init: Create XRM context
 *
 * @return XRM context or NULL
 */
xrmContext xlnx_xrm_create_context()
{
    /* Create XRM context */
    xrmContext xrm_ctx = xrmCreateContext(XRM_API_VERSION_1);
    if(xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "creation of XRM context failed\n");
        return NULL;
    }
    return xrm_ctx;
}

/**
 * xlnx_xrm_deinit: Deinit XRM
 *
 * @param xrm_ctx: XRM context
 * @param cu_reserve_id: CU reservation index
 */
void xlnx_xrm_deinit(xrmContext xrm_ctx, int32_t reserve_idx)
{
    if(reserve_idx > 0) {
        xrmCuPoolRelinquish(xrm_ctx, reserve_idx);
    }

    xrmDestroyContext(xrm_ctx);
    return;
}

/**
 * xlnx_xma_props_to_json: Converts the xma props to json for the given
 * function name
 *
 * @param props: The xma props, be they encoder, decoder, or scaler props
 * @param func_name: The name of the function - ENCODER, DECODER, LOOKAHEAD,
 * SCALER
 * @param json_job: json props
 * @return XMA_APP_SUCCESS on success
 */
static int32_t xlnx_xma_props_to_json(void* props, char* func_name,
                                      char* json_job)
{
    void* handle;
    /* Loading propstojson library to convert xma properties to json */
    void (*convert_xma_props_to_json)(void* props, char* func_name,
                                      char* json_job);
    handle                    = dlopen(XMA_PROPS_TO_JSON_SO, RTLD_NOW);
    convert_xma_props_to_json = dlsym(handle, "convertXmaPropsToJson");
    (*convert_xma_props_to_json)(props, func_name, json_job);
    dlclose(handle);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_get_mapping_id: Returns the mapping index for the IP
 *
 * @param ip_name: Name of the IP module
 * @return Mapping ID or -1 on failure
 */
static int32_t xlnx_get_mapping_id(char* ip_name)
{
    for(int32_t i = 0; i < XRM_MAX_NUM_IP; i++) {
        if(!strcmp(xlnx_ip_mapping[i].ip_name, ip_name)) {
            return i;
        }
    }
    return XMA_APP_ERROR;
}

/**
 * xlnx_xrm_load_calc: Calculates load for the given module
 *
 * @param xrm_ctx: XRM context
 * @param xma_props: XMA properties
 * @param ip_name: Name of the IP module
 * @return CU load or XMA_APP_ERROR on error
 */
int32_t xlnx_xrm_load_calc(xrmContext xrm_ctx, void* xma_props, char* ip_name)
{
    int32_t            ret;
    int32_t            skip_value;
    int32_t            cu_load    = 0;
    int32_t            func_id    = 0;
    int32_t            mapping_id = -1;
    xrmPluginFuncParam plg_param;

    mapping_id = xlnx_get_mapping_id(ip_name);

    memset(&plg_param, 0, sizeof(xrmPluginFuncParam));
    xlnx_xma_props_to_json(xma_props, ip_name, plg_param.input);

    ret = xrmExecPluginFunc(xrm_ctx, xlnx_ip_mapping[mapping_id].plugin_name,
                            func_id, &plg_param);
    if(ret != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "XRM plugin failed to calculate load for %s: %d\n", ip_name,
                   ret);
        return XMA_APP_ERROR;
    }

    cu_load    = atoi((char*)(strtok(plg_param.output, " ")));
    skip_value = atoi((char*)(strtok(NULL, " ")));
    (void)skip_value; /* To avoid warning */

    if(!strcmp(ip_name, "LOOKAHEAD")) {
        cu_load = atoi((char*)(strtok(NULL, " ")));
    }

    return cu_load;
}

/**
 * xlnx_update_pool_props: Updates CU pool properties
 *
 * @param ip_name: Name of the IP module
 * @param cu_load: CU load for the module
 * @param num_soft_kernel: Number of soft kernels
 * @param cu_pool_prop: CU pool properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_update_pool_props(char* ip_name, int32_t cu_load,
                               int8_t               num_soft_kernel,
                               xrmCuPoolPropertyV2* cu_pool_prop)
{
    int32_t cu_num          = cu_pool_prop->cuListProp.cuNum;
    int32_t mapping_id      = -1;
    cu_pool_prop->cuListNum = 1;
    mapping_id              = xlnx_get_mapping_id(ip_name);

    if(cu_load > 0) {
        strcpy(cu_pool_prop->cuListProp.cuProps[cu_num].kernelName,
               xlnx_ip_mapping[mapping_id].kernel_name);
        strcpy(cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias,
               xlnx_ip_mapping[mapping_id].kernel_alias);
        cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
        cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad =
            XRM_PRECISION_1000000_BIT_MASK(cu_load);
        cu_num++;

        for(int32_t i = 0; i < num_soft_kernel; i++) {
            strcpy(cu_pool_prop->cuListProp.cuProps[cu_num].kernelName,
                   xlnx_ip_mapping[mapping_id].soft_kernel_name);
            strcpy(cu_pool_prop->cuListProp.cuProps[cu_num].kernelAlias, "");
            cu_pool_prop->cuListProp.cuProps[cu_num].devExcl = false;
            cu_pool_prop->cuListProp.cuProps[cu_num].requestLoad =
                XRM_PRECISION_1000000_BIT_MASK(
                    XRM_MAX_CU_LOAD_GRANULARITY_1000000);
            cu_num++;
        }
    }

    cu_pool_prop->cuListProp.cuNum = cu_num;
    return XMA_APP_SUCCESS;
}

/**
 * Allocates resources and initializes device(s)
 * @param xrm_ctx The xrm context
 * @param device_ids An array of unique device ids to be initialized
 * @param num_devices The size of the array
 * @param cu_pool_prop The CU pool properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_xrm_device_init(xrmContext xrm_ctx, int32_t* device_ids,
                             int32_t              num_devices,
                             xrmCuPoolPropertyV2* cu_pool_prop)
{
    /* xclbin configuration */
    XmaXclbinParameter xclbin_param[num_devices];
    int32_t            ret         = XMA_APP_ERROR;
    int32_t            num_cu_pool = 0;

    /* Check the number of pools available for the given encoder load */
    num_cu_pool = xrmCheckCuPoolAvailableNumV2(xrm_ctx, cu_pool_prop);
    if(num_cu_pool <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "No resources available for allocation\n");
        return XMA_APP_ERROR;
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_XMA_APP_MODULE, "Num CU pools available %d\n",
               num_cu_pool);
    for(int i = 0; i < num_devices; i++) {
        xclbin_param[i].device_id   = device_ids[i];
        xclbin_param[i].xclbin_name = XCLBIN_PARAM_NAME;
    }

    if((ret = xma_initialize(xclbin_param, num_devices)) != XMA_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "XMA Initialization failed\n");
        return XMA_APP_ERROR;
    }
    xma_logmsg(XMA_INFO_LOG, XLNX_XMA_APP_MODULE,
               "XMA initialization success\n");

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_xrm_cu_alloc: Allocates the xrm cu list based on the requested load for
 * a user specified device id or reservation index.
 *
 * @param xrm_ctx: XRM context
 * @param ip_name: Name of the IP module
 * @param cu_load: CU load for the module
 * @param device_id: Device index
 * @param cu_reserve_id: CU reservation index
 * @param xrm_cu_list_res: XRM CU list resource structure
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_xrm_cu_alloc(xrmContext xrm_ctx, char* ip_name, int32_t cu_load,
                          int32_t device_id, int32_t cu_reserve_id,
                          xrmCuListResourceV2* xrm_cu_list_res)
{
    int32_t             ret        = XMA_APP_ERROR;
    int32_t             mapping_id = xlnx_get_mapping_id(ip_name);
    xrmCuListPropertyV2 xrm_cu_list_prop;
    memset(&xrm_cu_list_prop, 0, sizeof(xrmCuListPropertyV2));
    memset(xrm_cu_list_res, 0, sizeof(xrmCuListResourceV2));

    uint64_t dev_info = 0;
    if(device_id != DEFAULT_DEVICE_ID) {
        dev_info =
            (device_id << XRM_DEVICE_INFO_DEVICE_INDEX_SHIFT) |
            ((uint64_t)XRM_DEVICE_INFO_CONSTRAINT_TYPE_HARDWARE_DEVICE_INDEX
             << XRM_DEVICE_INFO_CONSTRAINT_TYPE_SHIFT);
    }
    xrm_cu_list_prop.cuNum = 1;
    strcpy(xrm_cu_list_prop.cuProps[0].kernelName,
           xlnx_ip_mapping[mapping_id].kernel_name);
    strcpy(xrm_cu_list_prop.cuProps[0].kernelAlias,
           xlnx_ip_mapping[mapping_id].kernel_alias);
    xrm_cu_list_prop.cuProps[0].devExcl = false;
    xrm_cu_list_prop.cuProps[0].requestLoad =
        XRM_PRECISION_1000000_BIT_MASK(cu_load);
    xrm_cu_list_prop.cuProps[0].deviceInfo = dev_info;
    xrm_cu_list_prop.cuProps[0].poolId     = cu_reserve_id;

    if(xlnx_ip_mapping[mapping_id].soft_kernel) {
        xrm_cu_list_prop.cuNum++;
        strcpy(xrm_cu_list_prop.cuProps[1].kernelName,
               xlnx_ip_mapping[mapping_id].soft_kernel_name);
        xrm_cu_list_prop.cuProps[1].devExcl = false;
        xrm_cu_list_prop.cuProps[1].requestLoad =
            XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
        xrm_cu_list_prop.cuProps[1].deviceInfo = dev_info;
        xrm_cu_list_prop.cuProps[1].poolId     = cu_reserve_id;
    }
    if((ret = xrmCuListAllocV2(xrm_ctx, &xrm_cu_list_prop, xrm_cu_list_res)) !=
       0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "xrm_allocation: fail to allocate cu list from reserve "
                   "id %d, error %d\n",
                   cu_reserve_id, ret);
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}
