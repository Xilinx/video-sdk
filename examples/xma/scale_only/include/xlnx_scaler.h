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

#ifndef _XLNX_ABR_SCALER_H_
#define _XLNX_ABR_SCALER_H_

#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>

#include <xma.h>
#include <xmaplugin.h>

#include "xlnx_scal_arg_parse.h"
#include "xlnx_scal_common.h"
#include "xlnx_scal_xma_props.h"
#include "xlnx_scal_xrm_interface.h"

typedef enum {
    PLANE_Y = 0,
    PLANE_U,
    PLANE_V
} PlaneId;

typedef struct XlnxAbrPixFmtDescription 
{
    char*           FormatStr;
    XmaFormatType   format;
    uint32_t        numPlanes;
    uint32_t        BytesPerPlane;
    uint8_t         bitdepth;
} XlnxAbrPixFmtDescription;

typedef struct XlnxScalerInputCtx 
{
    FILE*     in_fp;
    XmaFrame* xframe;
    int32_t   loops_remaining;
} XlnxScalerInputCtx;

/* 
Cannot make an array of XlnxScalerOutputCtx because 
xma_scaler_session_recv_frame_list expects XmaFrame**
*/
typedef struct XlnxScalerOutputCtx 
{
    /* total num used, num of full rate, num half rate */
    size_t          num_frames_to_scale;
    FILE*           out_fp[MAX_SCALER_OUTPUTS];
    XmaFrame*       xframe_list[MAX_SCALER_OUTPUTS];
} XlnxScalerOutputCtx;

typedef struct XlnxScalerCtx 
{
    int                     pts;
    size_t                  num_frames_scaled;
    int                     num_sessions;
    XmaScalerSession*       session[2];
    XlnxAppTimeTracker      timer;
    XlnxScalerInputCtx      input_ctx;
    XlnxScalerOutputCtx     output_ctx;
    XmaScalerProperties     abr_xma_props[2]; // All rate and full rate sessions 
    XlnxScalerProps         abr_params;
    XlnxScaleXrmCtx         scaler_xrm_ctx;
} XlnxScalerCtx;

#define abr_utils_get_num_video_planes(desc)    (desc.numPlanes)
#define abr_utils_get_bits_per_pixel(desc)      (desc.bitdepth)


/*------------------------------------------------------------------------------
xlnx_scal_cleanup_ctx: Cleanup the context - free resources, close files.

Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_cleanup_ctx(XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_fpga_init: Get/allocate xrm resources, xma initialize.

Parameters:
    ctx: The scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_fpga_init(XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_create_context: Creates the context based on user arguments. It 
    parses the header of the input file to get relevant codec info. This does 
    not create the xma session. Nor does it initialize the fpga.
Parameters:
    arguments: The argument struct containing scaler param, input, output file 
    info
    ctx: A pointer to the scaler context
Return:
    SCALER_APP_SUCCESS on success
------------------------------------------------------------------------------*/
int32_t xlnx_scal_create_context(XlnxScalArguments arguments, 
                                 XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_create_scaler_sessions: Creates the scaler sessions using the scaler
    props. One session for all rates, another for full rate only (If there are
    half rate sessions specified.)
Parameters:
    ctx: A pointer to the scaler context
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_create_scaler_sessions(XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_get_input_frame: Read an input frame into the xframe buffer
Parameters:
    ctx: A pointer to the scaler context
        in_fp: ctx->input_ctx.in_fp
        xframe: ctx->input_ctx.xframe
        props: ctx->arb_xma_props
Return:
    SCALER_APP_SUCCESS on success
    SCALER_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_scal_get_input_frame(XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_print_segment_performance: Print the performance since the last 
    segment.
Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_print_segment_performance(XlnxScalerCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_scal_print_total_performance: Print the total performance since tracking
    began.
Parameters:
    ctx: The scaler context
------------------------------------------------------------------------------*/
void xlnx_scal_print_total_performance(XlnxScalerCtx* ctx);

/* 
From xlnx_scaler_utils.c
*/

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
int32_t xlnx_scal_utils_get_plane_size(int32_t       stride,
                                       int32_t       height,
                                       XmaFormatType format,
                                       int32_t       plane_id);

/*------------------------------------------------------------------------------
xlnx_scal_utils_alloc_frame: Allocate an xma frame using the given fprops.
Parameters:
    fprops: The fprops used for allocation
Return:
    A pointer to the xma frame.
------------------------------------------------------------------------------*/
XmaFrame* xlnx_scal_utils_alloc_frame(XmaFrameProperties* fprops);

/*------------------------------------------------------------------------------
xlnx_scal_utils_get_format_description: Get the format description for the 
    given xma format 
Parameters:
    format: The xma format
Return:
    A XlnxAbrPixFmtDescription description
------------------------------------------------------------------------------*/
XlnxAbrPixFmtDescription xlnx_scal_utils_get_format_description(
                                                          XmaFormatType format);

#endif