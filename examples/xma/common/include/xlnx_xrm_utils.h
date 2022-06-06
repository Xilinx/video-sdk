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

#ifndef _XLNX_XRM_UTILS_H_
#define _XLNX_XRM_UTILS_H_

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <xlnx_app_utils.h>
#include <xma.h>
#include <xrm.h>

#define XMA_PROPS_TO_JSON_SO "/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so"
#define XCLBIN_PARAM_NAME    "/opt/xilinx/xcdr/xclbins/transcode.xclbin"

#define XRM_PRECISION_1000000_BIT_MASK(load) ((load << 8))

#define XLNX_MAX_DEVICE_COUNT 128
#define XLNX_MAX_DEVS_PER_CMD 2

#define XRM_MAX_NUM_IP               4
#define XLNX_MAX_IP_NAME_SIZE        32
#define XLNX_MAX_IP_PLUGIN_NAME_SIZE 32
#define XLNX_MAX_KERNEL_NAME_SIZE    32

typedef struct {
    char    ip_name[XLNX_MAX_IP_NAME_SIZE];
    char    plugin_name[XLNX_MAX_IP_PLUGIN_NAME_SIZE];
    char    kernel_name[XLNX_MAX_KERNEL_NAME_SIZE];
    char    kernel_alias[XLNX_MAX_KERNEL_NAME_SIZE];
    char    soft_kernel_name[XLNX_MAX_KERNEL_NAME_SIZE];
    uint8_t soft_kernel;
} XlnxIPMapping;

xrmContext xlnx_xrm_create_context();

void xlnx_xrm_deinit(xrmContext xrm_ctx, int32_t reserve_idx);

int32_t xlnx_xrm_load_calc(xrmContext xrm_ctx, void* xma_props, char* ip_name);

int32_t xlnx_update_pool_props(char* ip_name, int32_t cu_load,
                               int8_t               num_soft_kernel,
                               xrmCuPoolPropertyV2* cu_pool_prop);

int32_t xlnx_xrm_device_init(xrmContext xrm_ctx, int32_t* device_id,
                             int32_t              num_devices,
                             xrmCuPoolPropertyV2* cu_pool_prop);

int32_t xlnx_xrm_cu_alloc(xrmContext xrm_ctx, char* ip_name, int32_t cu_load,
                          int32_t device_id, int32_t cu_reserve_id,
                          xrmCuListResourceV2* xrm_cu_list_res);

#endif // _XLNX_XRM_UTILS_H_
