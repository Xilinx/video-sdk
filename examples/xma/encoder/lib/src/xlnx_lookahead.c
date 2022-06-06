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

/**
 * xlnx_la_create_ctx: Create the lookahead context. Does not init fpga/create
 * session.
 *
 * @param la_props: The lookahead properties used to create the lookahead
 * context
 * @param la_ctx:   The lookahead context to be created.
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_create_ctx(XlnxLookaheadProperties* la_props,
                           XlnxLookaheadCtx*        la_ctx)
{
    la_ctx->la_props = la_props;
    if((la_props->lookahead_depth == 0) && (la_props->spatial_aq_mode == 0)) {
        la_ctx->bypass = 1;
        return XMA_APP_SUCCESS;
    }
    xlnx_la_create_xma_props(la_ctx->la_props, &la_ctx->xma_la_props);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_la_send_frame: Sends input frame to xma plugin.
 *
 * @param la_ctx: Lookahead context
 * @param in_frame: Input frame
 * @return The result of xma_filter_session_send_frame
 */
static int32_t xlnx_la_send_frame(XlnxLookaheadCtx* la_ctx, XmaFrame* in_frame)
{

    int32_t ret = XMA_SUCCESS;
    if(in_frame && in_frame->do_not_encode) {
        return ret;
    }
    ret = xma_filter_session_send_frame(la_ctx->filter_session, in_frame);
    if(ret == XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "xlnx_la_send_frame : Failed to send frame %d!\n",
                   la_ctx->num_frames_sent);
    } else if(ret == XMA_SUCCESS || XMA_SEND_MORE_DATA) {
        la_ctx->num_frames_sent++;
    }
    return ret;
}

/**
 * xlnx_la_process_frame: High level funtion for lookahead send and receive
 * frame.
 *
 * @param la_ctx: Lookahead context
 * @param la_input_xframe: The input xframe containing input buffers
 * @param la_output_xframe: The output xframe to store the lookahead results.
 * (XmaFrame**
 * @param for the bypass use case)
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_process_frame(XlnxLookaheadCtx* la_ctx,
                              XmaFrame*         la_input_xframe,
                              XmaFrame**        la_output_xframe)
{
    int32_t send_ret = 0, recv_ret = 0;

    if(la_ctx->bypass) {
        *la_output_xframe = la_input_xframe;
        return XMA_SUCCESS;
    }
    if(*la_output_xframe == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "xlnx_la_process_frame : output xma frame is NULL!\n");
        return XMA_ERROR;
    }
    send_ret = xlnx_la_send_frame(la_ctx, la_input_xframe);
    if(send_ret == XMA_ERROR || send_ret == XMA_SEND_MORE_DATA) {
        return send_ret;
    }
    recv_ret = xma_filter_session_recv_frame(la_ctx->filter_session,
                                             *la_output_xframe);
    if(recv_ret == XMA_SUCCESS && send_ret == XMA_TRY_AGAIN) {
        return xlnx_la_send_frame(la_ctx, la_input_xframe);
    }
    if(recv_ret == XMA_SUCCESS) {
        la_ctx->num_frames_received++;
    }
    return recv_ret;
}

/**
 * Release the xma frame after it has been received by the lookahead and used
 * in other parts of the pipeline.
 * This MUST be called after an xma frame has been received by the lookahead and
 * is ready to be freed.
 *
 * @param la_ctx the lookahead context
 * @param received_frame The frame to be released/freed.
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_release_frame(XlnxLookaheadCtx* la_ctx,
                              XmaFrame*         received_frame)
{
    if(la_ctx->bypass) {
        return XMA_APP_SUCCESS;
    }
    if(!received_frame) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Attempted to release a NULL xma frame!");
        return XMA_APP_ERROR;
    }
    XmaSideDataHandle* side_data = received_frame->side_data;
    memset(received_frame, 0, sizeof(XmaFrame));
    received_frame->side_data = side_data;
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_la_deinit: Lookahead deinit module.
 *
 * @param la_ctx: Lookahead context
 * @param xma_la_props: XMA lookahead properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_deinit(XlnxLookaheadCtx* la_ctx)
{
    if(!la_ctx) {
        return XMA_APP_ERROR;
    }
    if(la_ctx->bypass == 0) {
        /* Close lookahead session */
        if(la_ctx->filter_session) {
            xma_filter_session_destroy(la_ctx->filter_session);
            la_ctx->filter_session = NULL;
        }
    }
    xlnx_la_free_xma_props(&la_ctx->xma_la_props);
    return XMA_APP_SUCCESS;
}
