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

#ifndef _XLNX_LOOKAHEAD_H_
#define _XLNX_LOOKAHEAD_H_

#include <inttypes.h>
#include <dlfcn.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_enc_xrm_interface.h"
#include "xlnx_enc_xma_props.h"
#include "xlnx_enc_constants.h"

typedef struct
{
    XmaFilterSession   *filter_session;
    XmaFrame           *xma_la_frame;
    XlnxLookaheadProperties la_props;
    int32_t            num_planes;
    uint8_t            bypass;
} XlnxLookaheadCtx;

int32_t xlnx_la_create(XlnxLookaheadCtx *la_ctx, 
	        XlnxEncoderXrmCtx *enc_xrm_ctx, XmaFilterProperties *xma_la_props);

int32_t xlnx_la_deinit(XlnxLookaheadCtx *la_ctx, 
	                   XmaFilterProperties *xma_la_props);

int32_t xlnx_la_process_frame(XlnxLookaheadCtx *la_ctx, XmaFrame *in_frame,
                             XmaFrame **out_frame);

int32_t xlnx_la_release_frame(XlnxLookaheadCtx *la_ctx, 
	                          XmaFrame *received_frame);

int32_t xlnx_la_get_bypass_mode(XlnxLookaheadCtx *la_ctx);

#endif // _XLNX_LOOKAHEAD_H_
