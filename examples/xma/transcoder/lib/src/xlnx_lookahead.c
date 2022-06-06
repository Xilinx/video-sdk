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

#include <stdlib.h>
#include <string.h>

#include "xlnx_lookahead.h"
#include "xvbm.h"


/**
 * xlnx_la_free_frame: Releases lookahead XMA frame and side data.
 *
 * @param xframe: XMA frame
 */
static void xlnx_la_free_frame(XmaFrame* xframe)
{
    XvbmBufferHandle handle;
    if(xframe == NULL) {
        return;
    }

    if(xframe->data[0].buffer) {
        if(xframe->data[0].buffer_type == XMA_DEVICE_BUFFER_TYPE) {
            handle = (XvbmBufferHandle)(xframe->data[0].buffer);
            if(handle) {
                xvbm_buffer_pool_entry_free(handle);
            }
        } else {
            free(xframe->data[0].buffer);
        }
        xframe->data[0].buffer = NULL;
    }

    xma_frame_clear_all_side_data(xframe);
    free(xframe);
}

/**
 * xlnx_la_free_res: Releases the lookahead resource and close the session.
 *
 * @param xrm_ctx: XRM context
 * @param la_ctx: Lookahead context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_la_free_res(xrmContext xrm_ctx, XlnxLookaheadCtx* la_ctx)
{

    if(!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "free_res : free_res la_ctx = NULL\n");
        return XMA_APP_ERROR;
    }

    xrmCuListReleaseV2(xrm_ctx, &la_ctx->lookahead_cu_list_res);

    /* Close lookahead session */
    if(la_ctx->filter_session) {
        xma_filter_session_destroy(la_ctx->filter_session);
        la_ctx->filter_session = NULL;
    }

    xlnx_la_free_frame(la_ctx->out_frame);
    la_ctx->out_frame = NULL;

    return XMA_APP_SUCCESS;
}

/**
 * la_create: Initializes lookahead module and creates session.
 *
 * @param la_ctx: Lookahead context
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_la_props: XMA lookahead properties
 * @return Lookahead context
 */
int32_t xlnx_la_create(XlnxLookaheadCtx*     la_ctx,
                       XlnxTranscoderXrmCtx* app_xrm_ctx,
                       XmaFilterProperties*  xma_la_props)
{

    XlnxLookaheadProperties* la_props = &la_ctx->la_props;

    if(!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "No LA context received\n");
        return XMA_APP_ERROR;
    }
    if((la_props->lookahead_depth == 0) && (la_props->temporal_aq_mode == 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid params: Lookahead = 0, temporal aq=%u\n",
                   la_props->temporal_aq_mode);
        return XMA_APP_ERROR;
    }

    if((la_props->lookahead_depth == 0) && (la_props->spatial_aq_mode == 0)) {
        la_ctx->bypass = 1;
        return XMA_APP_SUCCESS;
    }

    la_ctx->bypass = 0;
    xlnx_la_get_xma_props(&la_ctx->la_props, xma_la_props);
    xlnx_la_allocate_xrm_cu(app_xrm_ctx, xma_la_props,
                            &la_ctx->lookahead_cu_list_res);

    /* Create lookahead session based on the requested properties */
    la_ctx->filter_session = xma_filter_session_create(xma_la_props);
    if(!la_ctx->filter_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Failed to create lookahead session\n");
        return XMA_APP_ERROR;
    }

    la_ctx->out_frame = (XmaFrame*)calloc(1, sizeof(XmaFrame));
    if(la_ctx->out_frame == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Out of memory while allocating la out frame.\n");
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_la_send_frame: Sends input frame to xma plugin.
 *
 * @param la_ctx: Lookahead context
 * @param in_frame: Input frame
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_la_send_frame(XlnxLookaheadCtx* la_ctx, XmaFrame* in_frame)
{

    int32_t rc;
    if(!la_ctx) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xlnx_la_send_frame : XMA_ERROR\n");
        return XMA_APP_ERROR;
    }

    if(in_frame && in_frame->do_not_encode) {
        if(in_frame->data[0].buffer) {
            if(in_frame->data[0].buffer_type == XMA_DEVICE_BUFFER_TYPE) {
                XvbmBufferHandle handle =
                    (XvbmBufferHandle)(in_frame->data[0].buffer);
                if(handle) {
                    xvbm_buffer_pool_entry_free(handle);
                }
            }
        }
        rc = XMA_APP_SUCCESS;
    } else {
        rc = xma_filter_session_send_frame(la_ctx->filter_session, in_frame);
    }
    if(rc <= XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xlnx_la_send_frame : Send frame to LA xma plg Failed!!\n");
        rc = XMA_APP_ERROR;
    } else if(rc == XMA_SUCCESS || XMA_SEND_MORE_DATA) {
        la_ctx->num_frames_sent++;
    }
    return rc;
}

/**
 * xlnx_la_send_recv_frame: High level funtion for lookahead send and receive
 * frame.
 *
 * @param la_ctx: Lookahead context
 * @param in_frame: XMA input frame
 * @param out_frame: XMA output frame
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_send_recv_frame(XlnxLookaheadCtx* la_ctx, XmaFrame* in_frame,
                                XmaFrame** out_frame)
{
    int32_t ret = 0;

    if(out_frame == NULL) {
        return XMA_ERROR;
    }

    if(la_ctx->bypass == 1) {
        *out_frame = in_frame;
        return XMA_SUCCESS;
    }
    if(la_ctx->out_frame == NULL) {
        return XMA_ERROR;
    }

    ret = xlnx_la_send_frame(la_ctx, in_frame);
    switch(ret) {
        case XMA_SUCCESS:
            ret = xma_filter_session_recv_frame(la_ctx->filter_session,
                                                la_ctx->out_frame);
            if(ret == XMA_TRY_AGAIN) {
                ret = XMA_SEND_MORE_DATA;
            }
            break;
        case XMA_SEND_MORE_DATA:
            break;
        case XMA_TRY_AGAIN:
            /* If output is received, this condition should not be hit */
            ret = xma_filter_session_recv_frame(la_ctx->filter_session,
                                                la_ctx->out_frame);
            if(ret == XMA_SUCCESS) {
                ret = xlnx_la_send_frame(la_ctx, in_frame);
            }
            break;
        case XMA_ERROR:
        default:
            *out_frame = NULL;
            break;
    }
    if(ret == XMA_SUCCESS) {
        *out_frame        = la_ctx->out_frame;
        la_ctx->out_frame = NULL;
    }

    if(ret <= XMA_ERROR) {
        XvbmBufferHandle handle = (XvbmBufferHandle)(in_frame->data[0].buffer);
        if(handle && in_frame->data[0].buffer_type == XMA_DEVICE_BUFFER_TYPE) {
            xvbm_buffer_pool_entry_free(handle);
        }
    }
    return ret;
}

/**
 * xlnx_la_release_frame: Release the output frame returned by xma plugin
 *
 * @param la_ctx: Lookahead context
 * @param received_frame: Output frame
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_release_frame(XlnxLookaheadCtx* la_ctx,
                              XmaFrame*         received_frame)
{
    if(la_ctx->bypass) {
        return XMA_APP_SUCCESS;
    }
    if(!received_frame || la_ctx->out_frame) {
        return XMA_APP_ERROR;
    }
    la_ctx->out_frame            = received_frame;
    XmaSideDataHandle* side_data = la_ctx->out_frame->side_data;
    memset(la_ctx->out_frame, 0, sizeof(XmaFrame));
    la_ctx->out_frame->side_data = side_data;
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_la_in_bypass_mode: Returns lookahead bypass mode param
 *
 * @param la_ctx: Lookahead context
 * @return lookahead bypass mode value
 */
int32_t xlnx_la_in_bypass_mode(XlnxLookaheadCtx* la_ctx)
{
    int32_t ret = 0;
    if(!la_ctx) {
        return XMA_ERROR;
    }
    ret = la_ctx->bypass;
    return ret;
}

/**
 * la_close: Lookahead deinit module.
 *
 * @param xrm_ctx: XRM context
 * @param la_ctx: Lookahead context
 * @param xma_la_props: XMA lookahead properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_close(xrmContext xrm_ctx, XlnxLookaheadCtx* la_ctx,
                      XmaFilterProperties* xma_la_props)
{
    if(!la_ctx) {
        return XMA_APP_ERROR;
    }

    if(la_ctx->bypass == 0) {
        xlnx_la_free_res(xrm_ctx, la_ctx);
    }

    xlnx_la_free_xma_props(xma_la_props);

    return XMA_APP_SUCCESS;
}
