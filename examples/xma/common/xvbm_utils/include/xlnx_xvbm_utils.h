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
#ifndef _XLNX_XVBM_UTILS_H_
#define _XLNX_XVBM_UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xma.h>
#include <xvbm.h>

#include "xlnx_app_utils.h" // Need BUFFER_ALLOC_ALIGN #define

#define XLNX_XVBM_UTILS_MODULE "xlnx_xvbm_utils"

/**
 * Transfer the buffers stored on device into the dest_buffer. Allocates the
 * dest_buffer if it is NULL.
 * @param processed_frame The frame which has a device buffer
 * @param dest_buffer Where to store the extracted buffer. Allocates data  it if
 * point to NULL
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_utils_extract_dev_buf_to_host_buf(const XmaFrame* processed_frame,
                                               void**          dest_buffer);

/**
 * Extract the device buffer from the source xma frame into an xma frame's
 * buffer
 * @param src_xframe The xma frame containing the device buffer and appropriate
 * linesizes
 * @param dest_xframe The xma frame which will contain the host buffer
 * @return XMA_SUCCESS or XMA_ERROR
 */
int32_t xlnx_utils_copy_dev_frame_to_host_frame(const XmaFrame* src_xframe,
                                                XmaFrame*       dest_xframe);

#endif // _XLNX_XVBM_UTILS_H_
