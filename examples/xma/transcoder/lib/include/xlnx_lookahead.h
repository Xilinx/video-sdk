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


#include "xlnx_transcoder_constants.h"
#include "xlnx_transcoder_xma_props.h"
#include "xlnx_transcoder_xrm_interface.h"


typedef struct XlnxLookaheadCtx {
    XmaFilterSession*       filter_session;
    XmaFrame*               out_frame;
    xrmCuListResourceV2     lookahead_cu_list_res;
    XlnxLookaheadProperties la_props;
    size_t                  num_frames_sent;
    uint8_t                 bypass;
} XlnxLookaheadCtx;

int32_t xlnx_la_create(XlnxLookaheadCtx*     la_ctx,
                       XlnxTranscoderXrmCtx* app_xrm_ctx,
                       XmaFilterProperties*  xma_la_props);

int32_t   xlnx_la_close(xrmContext xrm_ctx, XlnxLookaheadCtx* la_ctx,
                        XmaFilterProperties* xma_la_props);
int32_t   xlnx_la_send_recv_frame(XlnxLookaheadCtx* la_ctx, XmaFrame* in_frame,
                                  XmaFrame** out_frame);
XmaFrame* la_get_input_frame(XlnxLookaheadCtx* la_ctx);
int32_t   xlnx_la_release_frame(XlnxLookaheadCtx* la_ctx,
                                XmaFrame*         received_frame);
int32_t   xlnx_la_in_bypass_mode(XlnxLookaheadCtx* la_ctx);

#endif // _XLNX_LOOKAHEAD_H_
