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

#ifndef _XLNX_TRANSCODER_H_
#define _XLNX_TRANSCODER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xvbm.h"
#include <xma.h>
#include <xrm.h>

#include "xlnx_decoder.h"
#include "xlnx_encoder.h"
#include "xlnx_lookahead.h"
#include "xlnx_scaler.h"
#include "xlnx_transcoder_constants.h"

typedef struct XlnxTranscoderCtx {

    XlnxTranscoderXrmCtx app_xrm_ctx;
    XlnxDecoderCtx       dec_ctx;
    XlnxScalerCtx        scal_ctx;
    XlnxEncoderCtx       enc_ctx[TRANSCODE_MAX_ABR_CHANNELS];
    XlnxAppTimeTracker   app_timer;
    XvbmBufferHandle     dec_pool_handle;
    XmaFrame             xma_app_frame[TRANSCODE_MAX_ABR_CHANNELS + 1];
    XmaDataBuffer        xma_out_buffer[TRANSCODE_MAX_ABR_CHANNELS];
    size_t               num_frames;
    size_t               in_frame_cnt;
    size_t               out_frame_cnt;
    int32_t              loop_count;
    int32_t              num_enc_channels;
    int32_t              curr_sess_channels;
    int32_t              num_scal_out;
    int32_t              num_scal_fullrate;
    int32_t              non_scal_channels;
    int32_t              enc_chan_idx;
    int32_t              eos_count;
    int32_t              flush_mode;
    int32_t              transcoder_state;
    uint32_t             la_in_index;
    uint32_t             dec_out_index;
    int32_t              dec_idx_arr[DEC_MAX_OUT_BUFFERS];
} XlnxTranscoderCtx;

int32_t xlnx_tran_device_init(XlnxTranscoderCtx*        transcode_ctx,
                              XlnxTranscoderProperties* transcode_props);

int32_t xlnx_tran_session_create(XlnxTranscoderCtx*        transcode_ctx,
                                 XlnxTranscoderProperties* transcode_props);

int32_t xlnx_tran_frame_process(XlnxTranscoderCtx* transcode_ctx,
                                int32_t*           transcode_stop);

#endif // _XLNX_TRANSCODER_H_
