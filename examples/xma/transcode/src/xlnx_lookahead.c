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

#include "xvbm.h"
#include "xlnx_lookahead.h"

/*-----------------------------------------------------------------------------
xlnx_la_get_num_video_planes: Returns the number of video planes based on input file 
format.

Parameters:
format: Input file format

Return:
Number of video planes
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_get_num_video_planes(XmaFormatType format)
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
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Unsupported format...");
            return -1;
    }
}

/*-----------------------------------------------------------------------------
xlnx_la_free_frame: Releases lookahead XMA frame and side data.

Parameters:
xframe: XMA frame
-----------------------------------------------------------------------------*/
static void xlnx_la_free_frame(XmaFrame *xframe)
{
    XvbmBufferHandle handle;
    if (xframe == NULL) {
        return;
    }

    if (xframe->data[0].buffer) {
        if (xframe->data[0].buffer_type == XMA_DEVICE_BUFFER_TYPE) {
            handle = (XvbmBufferHandle)(xframe->data[0].buffer);
            if (handle) {
                xvbm_buffer_pool_entry_free(handle);
            }
            xframe->data[0].buffer = NULL;
        } else {
            /* Only Zero copy supported */
            assert(0);
        }
    }

    xma_frame_clear_all_side_data(xframe);
    free(xframe);

}

/*-----------------------------------------------------------------------------
xlnx_la_free_res: Releases the lookahead resource and close the session.

Parameters:
xrm_ctx: XRM context
la_ctx: Lookahead context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_free_res(xrmContext *xrm_ctx, XlnxLookaheadCtx *la_ctx)
{

    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "free_res : free_res la_ctx = NULL\n");
        return TRANSCODE_APP_FAILURE;
    }
    if(!xrmCuRelease(xrm_ctx, &la_ctx->lookahead_cu_res)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Unable to release CU resource.\n");
        return TRANSCODE_APP_FAILURE;
    }

    /* Close lookahead session */
    if (la_ctx->filter_session) {
        xma_filter_session_destroy(la_ctx->filter_session);
        la_ctx->filter_session = NULL;
    }

    xlnx_la_free_frame(la_ctx->out_frame);
    la_ctx->out_frame = NULL;

    return TRANSCODE_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_la_allocate_xrm_cu: Allocates XRM CU for lookahead

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_la_props: XMA lookahead properties
lookahead_cu_res: Lookahead CU resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_allocate_xrm_cu(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                                       XmaFilterProperties *xma_la_props,
                                       xrmCuResource *lookahead_cu_res)
{

    /* XRM lookahead allocation */
    xrmCuProperty lookahead_cu_prop;
    char pluginName[XRM_MAX_NAME_LEN];
    int func_id = 0, la_load=0;
    int skip_value = 0;
    int32_t ret = TRANSCODE_APP_FAILURE;
    xrmPluginFuncParam param;
    memset(&param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void *props, char *funcName, char *jsonJob);

    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (xma_la_props, "LOOKAHEAD", param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");
    if (xrmExecPluginFunc(app_xrm_ctx->xrm_ctx, pluginName, func_id,
                &param) != XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
        "lookahead plugin function %d, fail to run the function\n", func_id);
    } else {
        skip_value = atoi((char *)(strtok(param.output, " ")));
        skip_value = atoi((char *)(strtok(NULL, " ")));
        la_load = atoi((char *)(strtok(NULL, " ")));
        /* To silence the warning of skip_value set, but not used */
        (void)skip_value;
    }

    memset(&lookahead_cu_prop, 0, sizeof(xrmCuProperty));
    memset(lookahead_cu_res, 0, sizeof(xrmCuResource));

    strcpy(lookahead_cu_prop.kernelName, "lookahead");
    strcpy(lookahead_cu_prop.kernelAlias, "LOOKAHEAD_MPSOC");
    lookahead_cu_prop.devExcl = false;
    lookahead_cu_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(la_load);

    if(app_xrm_ctx->device_id < 0) {
        lookahead_cu_prop.poolId = app_xrm_ctx->reserve_idx;
        ret = xrmCuAlloc(app_xrm_ctx->xrm_ctx, &lookahead_cu_prop,
                         lookahead_cu_res);
    }
    else {
        ret = xrmCuAllocFromDev(app_xrm_ctx->xrm_ctx, app_xrm_ctx->device_id,
                                &lookahead_cu_prop, lookahead_cu_res);
    }

    if (ret != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "xrm_allocation: fail to allocate lookahead cu \n");
        return ret;
    }
    app_xrm_ctx->lookahead_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_la_props->plugin_lib = lookahead_cu_res->kernelPluginFileName;
    xma_la_props->dev_index = lookahead_cu_res->deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_la_props->ddr_bank_index = -1;
    xma_la_props->cu_index = lookahead_cu_res->cuId;
    xma_la_props->channel_id = lookahead_cu_res->channelId;

    return ret;
}

/*-----------------------------------------------------------------------------
la_create: Initializes lookahead module and creates session.

Parameters:
la_ctx: Lookahead context
app_xrm_ctx: Transcoder XRM context
xma_la_props: XMA lookahead properties

Return:
Lookahead context
-----------------------------------------------------------------------------*/
int32_t xlnx_la_create(XlnxLookaheadCtx *la_ctx, 
                       XlnxTranscoderXrmCtx *app_xrm_ctx, 
                       XmaFilterProperties *xma_la_props)
{

    XlnxLookaheadProperties *la_props = &la_ctx->la_props;

    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "No LA context received\n");
        return TRANSCODE_APP_FAILURE;
    }
    if ((la_props->lookahead_depth == 0) && 
        (la_props->temporal_aq_mode == 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid params: Lookahead = 0, temporal aq=%u\n",
                la_props->temporal_aq_mode);
        return TRANSCODE_APP_FAILURE;
    }

    if (((la_props->lookahead_depth == 0) && (la_props->spatial_aq_mode == 0))
    || ((la_props->spatial_aq_mode == 0) && (la_props->temporal_aq_mode == 0) 
    && (la_props->rate_control_mode == 0))) {
        la_ctx->bypass = 1;
        return TRANSCODE_APP_SUCCESS;
    }

    la_ctx->num_planes = xlnx_la_get_num_video_planes(
                              la_ctx->la_props.xma_fmt_type);
    la_ctx->bypass = 0;

    xlnx_la_get_xma_props(&la_ctx->la_props, xma_la_props);
    app_xrm_ctx->lookahead_res_in_use = 0;
    xlnx_la_allocate_xrm_cu(app_xrm_ctx, xma_la_props, 
                            &la_ctx->lookahead_cu_res);

    /* Create lookahead session based on the requested properties */
    la_ctx->filter_session = xma_filter_session_create(xma_la_props);
    if (!la_ctx->filter_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed to create lookahead session\n");
        return TRANSCODE_APP_FAILURE;
    }

    la_ctx->out_frame = (XmaFrame *) calloc(1, sizeof(XmaFrame));
    if (la_ctx->out_frame == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Out of memory while allocating la out frame. \n");
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_la_send_frame: Sends input frame to xma plugin.

Parameters:
la_ctx: Lookahead context
in_frame: Input frame

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_la_send_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame)
{

    int32_t rc;
    if (!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "xlnx_la_send_frame : XMA_ERROR\n");
        return TRANSCODE_APP_FAILURE;
    }

    if (in_frame && in_frame->do_not_encode) {
        if (in_frame->data[0].buffer) {
            if (in_frame->data[0].buffer_type == XMA_DEVICE_BUFFER_TYPE) {
                XvbmBufferHandle handle = 
                            (XvbmBufferHandle)(in_frame->data[0].buffer);
                if (handle) {
                    xvbm_buffer_pool_entry_free(handle);
                }
            }
        }
        rc = TRANSCODE_APP_SUCCESS;
    } else {
        rc = xma_filter_session_send_frame(la_ctx->filter_session,
                in_frame);
    }
    if (rc <= XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "xlnx_la_send_frame : Send frame to LA xma plg Failed!!\n");
        rc = TRANSCODE_APP_FAILURE;
    }
    return rc;
}

/*-----------------------------------------------------------------------------
xlnx_la_send_recv_frame: High level funtion for lookahead send and receive 
frame.

Parameters:
la_ctx: Lookahead context
in_frame: XMA input frame
out_frame: XMA output frame

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_send_recv_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame,
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
    if (la_ctx->out_frame == NULL) {
        return XMA_ERROR;
    }

    ret = xlnx_la_send_frame(la_ctx, in_frame);
    switch (ret) {
        case XMA_SUCCESS:
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, 
                    la_ctx->out_frame);
            if (ret == XMA_TRY_AGAIN) {
                ret = XMA_SEND_MORE_DATA;
            }
            break;
        case XMA_SEND_MORE_DATA:
            break;
        case XMA_TRY_AGAIN:
            /* If output is received, this condition should not be hit */
            ret = xma_filter_session_recv_frame(la_ctx->filter_session, 
                    la_ctx->out_frame);
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
        *out_frame = la_ctx->out_frame;
        la_ctx->out_frame = NULL;
    }

    if(ret <= XMA_ERROR) {
        XvbmBufferHandle handle = 
                        (XvbmBufferHandle)(in_frame->data[0].buffer);
        if (handle) {
            xvbm_buffer_pool_entry_free(handle);
        }
    }
    return ret;

}

/*-----------------------------------------------------------------------------
xlnx_la_release_frame: Release the output frame returned by xma plugin

Parameters:
la_ctx: Lookahead context
received_frame: Output frame

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_release_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *received_frame)
{
    if (la_ctx->bypass) {
        return TRANSCODE_APP_SUCCESS;
    }
    if (!received_frame || la_ctx->out_frame) {
        return TRANSCODE_APP_FAILURE;
    }
    la_ctx->out_frame = received_frame;
    XmaSideDataHandle *side_data = la_ctx->out_frame->side_data;
    memset(la_ctx->out_frame, 0, sizeof(XmaFrame));
    la_ctx->out_frame->side_data = side_data;
    return TRANSCODE_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_la_in_bypass_mode: Returns lookahead bypass mode param

Parameters:
la_ctx: Lookahead context

Return:
lookahead bypass mode value
-----------------------------------------------------------------------------*/
int32_t xlnx_la_in_bypass_mode(XlnxLookaheadCtx *la_ctx)
{
    int32_t ret = 0;
    if (!la_ctx) {
        return XMA_ERROR;
    }
    ret = la_ctx->bypass;
    return ret;
}

/*-----------------------------------------------------------------------------
la_close: Lookahead deinit module.

Parameters:
xrm_ctx: XRM context
la_ctx: Lookahead context
xma_la_props: XMA lookahead properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_la_close(xrmContext *xrm_ctx, XlnxLookaheadCtx *la_ctx, 
                 XmaFilterProperties *xma_la_props)
{
    if (!la_ctx) {
        return TRANSCODE_APP_FAILURE;
    }

    if (la_ctx->bypass == 0) {
        xlnx_la_free_res(xrm_ctx, la_ctx);
    }

    xlnx_la_free_xma_props(xma_la_props);

    return TRANSCODE_APP_SUCCESS;
}
