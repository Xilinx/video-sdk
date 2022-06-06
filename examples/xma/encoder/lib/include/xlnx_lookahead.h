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

#ifndef _XLNX_LOOKAHEAD_H_
#define _XLNX_LOOKAHEAD_H_

#include <dlfcn.h>
#include <inttypes.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_enc_constants.h"
#include "xlnx_enc_xma_props.h"
#include "xlnx_enc_xrm_interface.h"


typedef struct {
    size_t                   num_frames_sent;
    size_t                   num_frames_received;
    XlnxLookaheadProperties* la_props;
    XmaFilterProperties      xma_la_props;
    XmaFilterSession*        filter_session;
    bool                     bypass;
} XlnxLookaheadCtx;

int32_t xlnx_la_create_ctx(XlnxLookaheadProperties* la_props,
                           XlnxLookaheadCtx*        la_ctx);

int32_t xlnx_la_deinit(XlnxLookaheadCtx* la_ctx);

int32_t xlnx_la_process_frame(XlnxLookaheadCtx* la_ctx,
                              XmaFrame*         la_input_xframe,
                              XmaFrame**        la_output_xframe);

int32_t xlnx_la_release_frame(XlnxLookaheadCtx* la_ctx,
                              XmaFrame*         received_frame);

#endif // _XLNX_LOOKAHEAD_H_
