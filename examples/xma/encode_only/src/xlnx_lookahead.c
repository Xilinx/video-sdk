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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "xlnx_lookahead.h"

/*-----------------------------------------------------------------------------
xlnx_la_get_num_planes: Returns the number of video planes based on input file 
                        format.

Parameters:
format: Input file format

Return:
Number of video planes
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_get_num_planes(XmaFormatType format)
{
    /* multi scaler supports max 2 planes till v2019.1 */
    switch (format) {
        case XMA_RGB888_FMT_TYPE: /* BGR */
            return 1;
        case XMA_YUV420_FMT_TYPE: /* NV12 */
            return 2;
        case XMA_VCU_NV12_FMT_TYPE: /* VCU_NV12 */
            return 1;
        default:
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                       "Unsupported format...");
            return -1;
    }
}

/*-----------------------------------------------------------------------------
xlnx_la_allocate_xrm_cu: Allocates XRM CU for lookahead

Parameters:
la_ctx: Lookahead context
xrm_reserve_id: XRM reserve id
xma_la_props: XMA lookahead properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_allocate_xrm_cu(XlnxLookaheadCtx *la_ctx, 
                                       XlnxEncoderXrmCtx *enc_xrm_ctx,
                                       XmaFilterProperties *xma_la_props)
{

    int32_t ret = ENC_APP_FAILURE;
    /* XRM lookahead allocation */
    xrmCuProperty lookahead_cu_prop;

    memset(&lookahead_cu_prop, 0, sizeof(xrmCuProperty));
    memset(&enc_xrm_ctx->lookahead_cu_res, 0, sizeof(xrmCuResource));

    strcpy(lookahead_cu_prop.kernelName, "lookahead");
    strcpy(lookahead_cu_prop.kernelAlias, "LOOKAHEAD_MPSOC");
    lookahead_cu_prop.devExcl = false;
    lookahead_cu_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->la_load);

    if(enc_xrm_ctx->device_id < 0) {
        lookahead_cu_prop.poolId = enc_xrm_ctx->enc_res_idx;
        ret = xrmCuAlloc(enc_xrm_ctx->xrm_ctx, &lookahead_cu_prop,
                         &enc_xrm_ctx->lookahead_cu_res);
    }
    else {
        ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx,  enc_xrm_ctx->device_id,
                          &lookahead_cu_prop, &enc_xrm_ctx->lookahead_cu_res);
    }

    if (ret != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "xrm_allocation: fail to allocate lookahead cu \n");
        return ret;
    } else {
        enc_xrm_ctx->lookahead_res_inuse = 1;
    }

    /* Set XMA plugin SO and device index */
    xma_la_props->plugin_lib = 
                         enc_xrm_ctx->lookahead_cu_res.kernelPluginFileName;
    xma_la_props->dev_index = enc_xrm_ctx->lookahead_cu_res.deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_la_props->ddr_bank_index = -1;
    xma_la_props->cu_index = enc_xrm_ctx->lookahead_cu_res.cuId;
    xma_la_props->channel_id = enc_xrm_ctx->lookahead_cu_res.channelId;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_la_send_frame: Sends input frame to xma plugin.

Parameters:
la_ctx: Lookahead context
in_frame: Input frame

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_send_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame)
{

    int32_t rc;
    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "xlnx_la_send_frame : XMA_ERROR\n");
        return ENC_APP_FAILURE;
    }

    if (in_frame && in_frame->do_not_encode) {
        rc = ENC_APP_SUCCESS;
    } else {
        rc = xma_filter_session_send_frame(la_ctx->filter_session,
                in_frame);
    }
    if (rc <= XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "xlnx_la_send_frame : Send frame to LA xma plg Failed!!\n");
        rc = ENC_APP_FAILURE;
    }
    return rc;
}

/*-----------------------------------------------------------------------------
xlnx_la_create: Initializes lookahead module and creates session.

Parameters:
la_ctx: Lookahead context
enc_xrm_ctx: Encoder XRM context
xma_la_props: XMA lookahead properties

Return:
Lookahead context
-----------------------------------------------------------------------------*/
int32_t xlnx_la_create(XlnxLookaheadCtx *la_ctx, XlnxEncoderXrmCtx *enc_xrm_ctx, 
                      XmaFilterProperties *xma_la_props)
{

    XlnxLookaheadProperties *la_props = &la_ctx->la_props;

    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "No LA context received\n");
        return ENC_APP_FAILURE;
    }
    if ((la_props->lookahead_depth == 0) && 
        (la_props->temporal_aq_mode == 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Invalid params: Lookahead = 0, temporal aq=%u\n",
                la_props->temporal_aq_mode);
        return ENC_APP_SUCCESS;
    }

    if (((la_props->lookahead_depth == 0) && (la_props->spatial_aq_mode == 0))
    || ((la_props->spatial_aq_mode == 0) && (la_props->temporal_aq_mode == 0) 
    && (la_props->rate_control_mode == 0))) {
        la_ctx->bypass = 1;
        return ENC_APP_SUCCESS;
    }

    la_ctx->num_planes = xlnx_la_get_num_planes(la_ctx->la_props.xma_fmt_type);
    la_ctx->bypass = 0;

    xlnx_la_get_xma_props(&la_ctx->la_props, xma_la_props);

    enc_xrm_ctx->lookahead_res_inuse = 0;
    xlnx_la_allocate_xrm_cu(la_ctx, enc_xrm_ctx, xma_la_props);

    /* Create lookahead session based on the requested properties */
    la_ctx->filter_session = xma_filter_session_create(xma_la_props);
    if (!la_ctx->filter_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Failed to create lookahead session\n");
        return ENC_APP_FAILURE;
    }

    la_ctx->xma_la_frame = (XmaFrame *) calloc(1, sizeof(XmaFrame));
    if (la_ctx->xma_la_frame == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Out of memory while allocating la out frame. \n");
        return ENC_APP_FAILURE;
    }

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_la_process_frame: High level funtion for lookahead send and receive 
                       frame.

Parameters:
la_ctx: Lookahead context
in_frame: XMA input frame
out_frame: XMA output frame

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_process_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame,
                              XmaFrame **out_frame)
{
    int32_t ret = 0;

    if (out_frame == NULL) {
        return XMA_ERROR;
    }
    if (la_ctx->bypass == 1) {
        *out_frame = in_frame;
        return XMA_SUCCESS;
    }
    if (la_ctx->xma_la_frame == NULL) {
        return XMA_ERROR;
    }

    ret = xlnx_la_send_frame(la_ctx, in_frame);
    switch (ret) {
        case XMA_SUCCESS:
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, 
                    la_ctx->xma_la_frame);
            if (ret == XMA_TRY_AGAIN) {
                ret = XMA_SEND_MORE_DATA;
            }
            if(ret != ENC_APP_SUCCESS)
                break;
        case XMA_SEND_MORE_DATA:
            break;
        case XMA_TRY_AGAIN:
            /* If the user is receiving output, this condition should 
               not be hit */
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, 
                    la_ctx->xma_la_frame);
            if (ret == XMA_SUCCESS) {
                ret = xlnx_la_send_frame(la_ctx, in_frame);
            }
            break;
        case XMA_ERROR:
        default:
            *out_frame = NULL;
            break;
    }
    if (ret == XMA_SUCCESS) {
        *out_frame = la_ctx->xma_la_frame;
        la_ctx->xma_la_frame = NULL;
    }
    return ret;

}

/*-----------------------------------------------------------------------------
xlnx_la_release_frame: Release the output frame returned by xma plugin

Parameters:
la_ctx: Encoder context
received_frame: Output frame

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_release_frame(XlnxLookaheadCtx *la_ctx, 
                              XmaFrame *received_frame)
{
    if (la_ctx->bypass) {
        return ENC_APP_SUCCESS;
    }

    if (!received_frame || la_ctx->xma_la_frame) {
        return ENC_APP_FAILURE;
    }
    la_ctx->xma_la_frame = received_frame;
    XmaSideDataHandle *side_data = la_ctx->xma_la_frame->side_data;
    memset(la_ctx->xma_la_frame, 0, sizeof(XmaFrame));
    la_ctx->xma_la_frame->side_data = side_data;
    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_la_get_bypass_mode: Returns lookahead bypass mode param

Parameters:
la_ctx: Lookahead context

Return:
lookahead bypass mode value
-----------------------------------------------------------------------------*/
int32_t xlnx_la_get_bypass_mode(XlnxLookaheadCtx *la_ctx)
{
    if (!la_ctx) {
        return ENC_APP_FAILURE;
    }
    return la_ctx->bypass;
}

/*-----------------------------------------------------------------------------
xlnx_la_deinit: Lookahead deinit module.

Parameters:
la_ctx: Lookahead context
xma_la_props: XMA lookahead properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_deinit(XlnxLookaheadCtx *la_ctx, 
                       XmaFilterProperties *xma_la_props)
{
    if (!la_ctx) {
        return ENC_APP_FAILURE;
    }

    if (la_ctx->bypass == 0) {

        /* Close lookahead session */
        if (la_ctx->filter_session) {
            xma_filter_session_destroy(la_ctx->filter_session);
            la_ctx->filter_session = NULL;
        }

        if (la_ctx->xma_la_frame != NULL) {
            xma_frame_clear_all_side_data(la_ctx->xma_la_frame);
            free(la_ctx->xma_la_frame);
            la_ctx->xma_la_frame = NULL;
        }

    }

    xlnx_la_free_xma_props(xma_la_props);
    return ENC_APP_SUCCESS;
}
