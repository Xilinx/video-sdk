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

#include "xlnx_encoder.h"

int32_t xlnx_enc_create_enc_ctx(XlnxEncoderProperties* enc_props,
                                XlnxEncoderCtx*        enc_ctx)
{
    enc_ctx->enc_props = enc_props;
    if(xlnx_enc_create_xma_props(enc_ctx->enc_props, &enc_ctx->xma_enc_props) !=
       XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    XmaDataBuffer* xma_buffer = &enc_ctx->output_xma_buffer;
    /* Allocate enough data to safely recv */
    xma_buffer->alloc_size =
        (3 * enc_ctx->enc_props->width * enc_ctx->enc_props->height) >> 1;
    if(posix_memalign(&xma_buffer->data.buffer, BUFFER_ALLOC_ALIGN,
                      xma_buffer->alloc_size) != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_XMA_APP_MODULE,
                   "Encoder failed to allocate data buffer for recv!\n");
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Send the input xframe to the xma encoder plugin
 *
 * @param enc_ctx      The encoder context
 * @param input_xframe The input xma frame containing raw frame in nv12 format
 * @return XMA_SUCCESS, XMA_ERROR, XMA_SEND_MORE_DATA, or
 * XMA_EOS on end of stream.
 */
static int32_t xlnx_enc_send_frame(XlnxEncoderCtx* enc_ctx,
                                   XmaFrame*       input_xframe)
{
    int ret = INT32_MIN;

    if(input_xframe->data[0].buffer) {
        ret = xma_enc_session_send_frame(enc_ctx->enc_session, input_xframe);
    } else if(!enc_ctx->flush_sent) {
        input_xframe->is_last_frame = 1;
        input_xframe->pts           = -1;
        ret = xma_enc_session_send_frame(enc_ctx->enc_session, input_xframe);
        if(ret != XMA_FLUSH_AGAIN) {
            enc_ctx->flush_sent = true;
        }
    }

    if(ret == XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Encoder failed to send frame %d\n",
                   enc_ctx->num_frames_sent);
    } else if(ret == XMA_SUCCESS || ret == XMA_SEND_MORE_DATA) {
        enc_ctx->num_frames_sent++;
    } else if(ret == INT32_MIN) { // It made it through without sending
        ret = XMA_SUCCESS;
    }
    return ret;
}

/**
 * Processes an input xma frame throug the encoder. Begins flushing when the
 * input xma frame datap[0].buffer is NULL.
 *
 * @param enc_ctx      The encoder context
 * @param input_xframe The input xma frame containing raw frame in nv12 format
 * @param recv_size    Stores how large the output encoded frame was
 * @return Returns the result of the call to receive. XMA_SUCCESS,
 * XMA_ERROR, XMA_SEND_MORE_DATA, or XMA_EOS on end of
 * stream.
 */
int32_t xlnx_enc_process_frame(XlnxEncoderCtx* enc_ctx, XmaFrame* input_xframe,
                               int32_t* recv_size)
{
    int ret = xlnx_enc_send_frame(enc_ctx, input_xframe);
    if(ret == XMA_ERROR || ret == XMA_SEND_MORE_DATA) {
        return ret;
    }
    ret = xma_enc_session_recv_data(enc_ctx->enc_session,
                                    &(enc_ctx->output_xma_buffer), recv_size);
    if(ret == XMA_SUCCESS) {
        enc_ctx->num_frames_received++;
    } else if(ret == XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Failed to receive frame %d\n",
                   enc_ctx->num_frames_received);
    }
    return ret;
}

/**
 * xlnx_enc_deinit: Encoder deinitialize
 *
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @return None
 */
void xlnx_enc_deinit(XlnxEncoderCtx* enc_ctx)
{
    if(enc_ctx->enc_session != NULL) {
        xma_enc_session_destroy(enc_ctx->enc_session);
    }
    xlnx_enc_xrm_deinit(&enc_ctx->enc_xrm_ctx);
    xlnx_enc_free_xma_props(&enc_ctx->xma_enc_props);
    if(enc_ctx->output_xma_buffer.data.buffer) {
        free(enc_ctx->output_xma_buffer.data.buffer);
    }
    if(enc_ctx->dynamic_idr->len_idr_arr) {
        free(enc_ctx->dynamic_idr->idr_arr);
    }
    return;
}
