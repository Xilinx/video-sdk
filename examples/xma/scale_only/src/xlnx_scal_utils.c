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

#include <string.h>
#include <stdlib.h>
#include "xlnx_scaler.h"

static const XlnxAbrPixFmtDescription supported_pix_fmt[] = 
{
   {"nv12", XMA_VCU_NV12_FMT_TYPE, 2, 1, 8},
};

/*------------------------------------------------------------------------------
xlnx_scal_utils_get_format_description: Get the format description for the 
    given xma format 
Parameters:
    format: The xma format
Return:
    A XlnxAbrPixFmtDescription description
------------------------------------------------------------------------------*/
XlnxAbrPixFmtDescription xlnx_scal_utils_get_format_description(
                                                           XmaFormatType format) 
{
    int i;
    int numFormats = sizeof(supported_pix_fmt) / 
                     (sizeof(XlnxAbrPixFmtDescription));
    int fmtIndex = ~0;

    for(i=0; i < numFormats; i++) {
        if(format == supported_pix_fmt[i].format) {
            fmtIndex = i;
            break;
        }
    }
    if(fmtIndex > numFormats) {
        SCALER_APP_LOG_ERROR("Pixel Format %d not supported\n", format);
        exit(SCALER_APP_ERROR);
    }
    return supported_pix_fmt[i];
}


/*------------------------------------------------------------------------------
xlnx_scal_utils_get_plane_size: Compute the number of bytes in a given plane
Parameters:
    stride: The frame stride
    height: The frame height
    format: The xma format type
    plane_id: the current plane id.
Return:
    The size of the plane
------------------------------------------------------------------------------*/
int32_t xlnx_scal_utils_get_plane_size(int32_t stride, int32_t height, 
                                 XmaFormatType format, int32_t plane_id) 
{
    int32_t sizeInBytes;
    XlnxAbrPixFmtDescription fmtdesc = xlnx_scal_utils_get_format_description(
                                                                        format);

    switch (plane_id) {
      case PLANE_Y:
        sizeInBytes = (stride * height * fmtdesc.BytesPerPlane);
        break;

      case PLANE_U:
      case PLANE_V:
        sizeInBytes = ((stride * height * fmtdesc.BytesPerPlane) >> 1);
        break;

      default:
        sizeInBytes = 0;
        SCALER_APP_LOG_ERROR("Plane ID Out-of-range");
        break;
    }
    return sizeInBytes;
}

/*------------------------------------------------------------------------------
xlnx_scal_utils_alloc_frame: Allocate an xma frame using the given fprops.
Parameters:
    fprops: The fprops used for allocation
Return:
    A pointer to the xma frame.
------------------------------------------------------------------------------*/
XmaFrame* xlnx_scal_utils_alloc_frame(XmaFrameProperties *fprops) 
{
    XmaFrame* frame;
    int32_t size = 0;

    fprops->linesize[0] = fprops->width;
    fprops->linesize[1] = fprops->linesize[0];
    fprops->linesize[2] = fprops->linesize[0];

    frame = xma_frame_alloc(fprops, true);
    if(!frame) {
        SCALER_APP_LOG_ERROR("unable to allocate xma frame\n");
        exit(SCALER_APP_ERROR);
    }
    //xma frame allocated without data buffers
    //allocate data buffer based on plane size
    int plane_id;
    int num_planes = 2;
    for (plane_id=0; plane_id < num_planes; plane_id++) {
        //verify no buffer was allocated by xma, to prevent mem leak
        if (frame->data[plane_id].buffer) {
            SCALER_APP_LOG_ERROR("Buffer Allocation already done\n");
            exit(SCALER_APP_ERROR);
        }
        size = xlnx_scal_utils_get_plane_size(fprops->width, fprops->height,
                                        fprops->format, plane_id);
        frame->data[plane_id].buffer_type  = XMA_HOST_BUFFER_TYPE;
        frame->data[plane_id].buffer       = malloc(size);
    }
    return frame;
}