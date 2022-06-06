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

#include "xlnx_scaler.h"
#include <stdlib.h>
#include <string.h>

/**
 * xlnx_scal_utils_get_plane_size: Compute the number of bytes in a given plane
 * @param fprops: The frame properties
 * @param plane_id: the current plane id.
 * @return The size of the plane
 */
int32_t xlnx_scal_utils_get_plane_size(XmaFrameProperties* fprops, int plane_id)
{
    int size_in_bytes;
    switch(plane_id) {
        case PLANE_Y:
            size_in_bytes = (fprops->linesize[plane_id] * fprops->height);
            break;

        case PLANE_U:
        case PLANE_V:
            size_in_bytes =
                ((fprops->linesize[plane_id] * fprops->height) >> 1);
            break;

        default:
            size_in_bytes = 0;
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Plane ID Out-of-range\n");
            break;
    }
    return size_in_bytes;
}

/**
 * xlnx_scal_utils_alloc_frame: Allocate an xma frame using the given fprops.
 * @param fprops: The fprops used for allocation
 * @return A pointer to the xma frame.
 */
XmaFrame* xlnx_scal_utils_alloc_frame(XmaFrameProperties* fprops)
{
    XmaFrame* frame;
    int32_t   size = 0;
    frame          = xma_frame_alloc(fprops, true);
    if(!frame) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "unable to allocate xma frame\n");
        exit(XMA_APP_ERROR);
    }
    // xma frame allocated without data buffers
    // allocate data buffer based on plane size
    int plane_id;
    int num_planes = 2;
    for(plane_id = 0; plane_id < num_planes; plane_id++) {
        // verify no buffer was allocated by xma, to prevent mem leak
        if(frame->data[plane_id].buffer) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Buffer Allocation already done\n");
            exit(XMA_APP_ERROR);
        }
        size = xlnx_scal_utils_get_plane_size(fprops, plane_id);
        frame->data[plane_id].buffer_type = XMA_HOST_BUFFER_TYPE;
        if(posix_memalign(&frame->data[plane_id].buffer, BUFFER_ALLOC_ALIGN,
                          size) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Failed to allocate buffer for xma frame.\n");
            exit(XMA_APP_ERROR);
        }
    }
    return frame;
}
