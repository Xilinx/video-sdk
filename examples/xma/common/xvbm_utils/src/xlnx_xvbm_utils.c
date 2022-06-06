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

#include "xlnx_xvbm_utils.h"

/**
 * Transfer the buffers stored on device into the dest_buffer. Allocates the
 * dest_buffer if it is NULL.
 * @param processed_frame The frame which has a device buffer
 * @param dest_buffer Where to store the extracted buffer. Allocates data  it if
 * point to NULL
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_utils_extract_dev_buf_to_host_buf(const XmaFrame* processed_frame,
                                               void**          dest_buffer)
{
    uint16_t total_bytes_in_line, buff_height;
    total_bytes_in_line = processed_frame->frame_props.linesize[0];
    buff_height         = processed_frame->frame_props.linesize[1];
    size_t buff_size    = total_bytes_in_line * buff_height +      // Y plane
                       ((total_bytes_in_line * buff_height) >> 1); // UV plane

    if(!*dest_buffer) {
        if(posix_memalign(dest_buffer, BUFFER_ALLOC_ALIGN, buff_size) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_XVBM_UTILS_MODULE,
                       "Failed to allocate buffer for device transfer!\n");
            return XMA_APP_ERROR;
        }
    }
    if(xvbm_buffer_read(processed_frame->data[0].buffer, (*dest_buffer),
                        (buff_size), 0)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XVBM_UTILS_MODULE,
                   "Failed to extract buffer from device!\n");
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Extract the device buffer from the source xma frame into an xma frame's
 * buffer
 * @param src_xframe The xma frame containing the device buffer and appropriate
 * linesizes
 * @param dest_xframe The xma frame which will contain the host buffer
 * @return XMA_SUCCESS or XMA_ERROR
 */
int32_t xlnx_utils_copy_dev_frame_to_host_frame(const XmaFrame* src_xframe,
                                                XmaFrame*       dest_xframe)
{
    if(src_xframe->frame_props.linesize[0] > 0) {
        dest_xframe->frame_props.linesize[0] =
            src_xframe->frame_props.linesize[0];
        dest_xframe->frame_props.linesize[1] =
            src_xframe->frame_props.linesize[1];
    }
    XmaBufferRef* dest_data = &dest_xframe->data[0];
    xlnx_utils_extract_dev_buf_to_host_buf(src_xframe, &dest_data[0].buffer);
    if(dest_data[0].buffer == NULL) {
        return XMA_ERROR;
    }
    dest_data[0].buffer_type = XMA_HOST_BUFFER_TYPE;
    dest_data[0].refcount    = 1;
    /* y size/uv start = linesize[0] * linesize[1] bytes. linesize[1] == aligned
     * height. */
    dest_data[1].buffer =
        dest_data[0].buffer + (src_xframe->frame_props.linesize[0] *
                               src_xframe->frame_props.linesize[1]);
    dest_data[1].is_clone    = true;
    dest_data[1].buffer_type = XMA_HOST_BUFFER_TYPE;
    return XMA_SUCCESS;
}
