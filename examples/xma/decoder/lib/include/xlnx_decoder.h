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
#ifndef _XLNX_DECODER_H_
#define _XLNX_DECODER_H_
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>


#include "xlnx_dec_h264_parser.h"
#include "xlnx_dec_h265_parser.h"

#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"
#include "xlnx_dec_xma_props.h"
#include "xlnx_dec_xrm_interface.h"
#include "xlnx_xvbm_utils.h"


#define XLNX_DEC_APP_MODULE "xlnx_decoder_app"

typedef struct XlnxDecoderInputCtx {
    XlnxStateInfo         input_file_info;
    XmaDataBuffer         xbuffer;
    XlnxDecFrameParseData parse_data;
} XlnxDecoderInputCtx;

typedef struct XlnxDecoderCtx {
    int32_t               pts;
    bool                  is_flush_sent;
    size_t                num_frames_sent;
    size_t                num_frames_received;
    XmaDecoderSession*    xma_dec_session;
    XmaDecoderProperties  dec_xma_props;
    XlnxDecoderProperties dec_props;
    XlnxDecoderXrmCtx     dec_xrm_ctx;
    XlnxDecoderInputCtx   input_ctx;
} XlnxDecoderCtx;

/**
 * xlnx_dec_cleanup_dec_ctx: Cleanup the decoder context - free xma frame,
 * release xrm resources, close input file.
 *
 * @param ctx: The decoder context
 */
void xlnx_dec_cleanup_dec_ctx(XlnxDecoderCtx* ctx);

/**
 * xlnx_dec_fpga_init: Get/allocate xrm resources, xma initialize.
 *
 * @param ctx: The decoder context
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_dec_fpga_init(XlnxDecoderCtx* ctx);

/**
 * xlnx_dec_create_decoder_context: Creates the decoder context. Opens input
 * file and parses the first header from it to get info, copies the dec_params
 * to its own fields.
 * @param input_file_name: The name of the input file to be opened and parsed
 * @param loop_count: How many loops the user specified.
 * @param dec_params: The dec params which have been set by the user. Doesn't
 * have
 * @param info from input file header at this point.
 * @param ctx: A pointer to the decoder context to be created. (Assumes memory
 * already
 * @param allocated)
 * @return XMA_APP_SUCCESS on success XMA_APP_ERROR on error
 */
int32_t xlnx_dec_create_decoder_context(char* input_file_name, long loop_count,
                                        XlnxDecoderProperties dec_params,
                                        XlnxDecoderCtx*       ctx);

/**
 * xlnx_dec_create_xframe: Create the xma frame ctx->channel_ctx.xframe for
 * decoding
 * @param ctx: the decoder context
 * @param xframe: The xma frame to be created/filled
 * @return XMA_APP_SUCCESS on success
 */
int32_t xlnx_dec_create_xframe(XlnxDecoderCtx* ctx, XmaFrame* xframe);

/**
 * xlnx_dec_scan_next_au: Scans the next access unit from the input file into
 * ctx->input_ctx.input_file_info.buffer. This should not be used for initial
 * setup.
 * @param ctx: The decoder context
 * @param input_ctx.input_file_info: Contains the input file and input buffer
 * @param input_ctx.parse_data: Contains the parse data
 * @param offset: Pointer to the offset which is set by the decoder parser.
 * @return XMA_APP_SUCCESS on success XMA_APP_EOS on end of stream
 * XMA_APP_EOF on end of file
 */
int xlnx_dec_scan_next_au(XlnxDecoderCtx* ctx, int* offset);

/**
 * Processes the next input. Parse an access unit from the input file, send to
 * the decoder, and take an xma frame out. Does not move the file buffer
 * forward if send fails.
 *
 * @param dec_ctx The decoder context
 * @param output_xframe The xma frame which stores the received output in a
 * device buffer.
 * @return The result of the receive. XMA_SUCCESS, XMA_TRY_AGAIN, or XMA_ERROR
 */
int xlnx_dec_process_frame(XlnxDecoderCtx* dec_ctx, XmaFrame* output_xframe);

#endif
