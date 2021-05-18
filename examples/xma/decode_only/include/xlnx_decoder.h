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
#ifndef _INCLUDED_XMDECODER_H_
#define _INCLUDED_XMDECODER_H_
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <signal.h>

#include <xmaplugin.h>
#include <xrm.h>
#include <xma.h>
#include <xvbm.h> 

#include "xlnx_dec_h264_parser.h"
#include "xlnx_dec_h265_parser.h"

#include "xlnx_dec_arg_parse.h"
#include "xlnx_dec_xma_props.h"
#include "xlnx_dec_xrm_interface.h"
#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"

#define XLNX_DEC_APP_MODULE "xlnx_decoder_app"

typedef struct XlnxDecoderInputCtx
{
    struct XlnxStateInfo  input_file_info;
    XmaDataBuffer         xbuffer;
    XlnxDecFrameParseData parse_data;
} XlnxDecoderInputCtx;

typedef struct XlnxDecoderChannelCtx
{
    FILE*           out_fp;
    size_t          num_frames_to_decode;
    XmaFrame*       xframe;
} XlnxDecoderChannelCtx;

typedef struct XlnxDecoderCtx
{
    int32_t                   pts;
    bool                      is_flush_sent;
    size_t                    num_frames_sent;
    size_t                    num_frames_decoded;
    XmaDecoderSession*        xma_dec_session;
    XmaDecoderProperties      dec_xma_props;
    XlnxDecoderProperties     dec_params;
    XlnxDecoderInputCtx       input_ctx;
    XlnxDecoderChannelCtx     channel_ctx;
    XlnxDecoderXrmCtx         dec_xrm_ctx;
    XlnxAppTimeTracker        timer;
} XlnxDecoderCtx;

/*------------------------------------------------------------------------------
xlnx_dec_cleanup_ctx: Cleanup the context - free resources, close files.

Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_cleanup_ctx(XlnxDecoderCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_dec_fpga_init: Get/allocate xrm resources, xma initialize.

Parameters:
    ctx: The decoder context
Return:
    DEC_APP_SUCCESS on success
    DEC_APP_ERROR on error
------------------------------------------------------------------------------*/
int32_t xlnx_dec_fpga_init(XlnxDecoderCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_dec_create_decoder_context: Creates the context based on user arguments. It 
    parses the header of the input file to get relevant codec info. This does 
    not create the xma session. Nor does it initialize the fpga.
Parameters:
    arguments: The argument struct containing decoder param, input, output file 
    info
    ctx: A pointer to the decoder context
Return:
    DEC_APP_SUCCESS on success
------------------------------------------------------------------------------*/
int32_t xlnx_dec_create_decoder_context(XlnxDecoderArguments arguments, 
                                   XlnxDecoderCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_dec_scan_next_au: Scans the next access unit from the input file into 
    ctx->input_ctx.input_file_info.buffer. This should not be used for initial 
    setup.
Parameters:
    ctx: The decoder context
        input_ctx.input_file_info: Contains the input file and input buffer
        input_ctx.parse_data: Contains the parse data
    offset: Pointer to the offset which is set by the decoder parser.
Return:
    RET_SUCCESS on success
    RET_EOS on end of stream
    RET_EOF on end of file
------------------------------------------------------------------------------*/
int xlnx_dec_scan_next_au(XlnxDecoderCtx* ctx, int* offset);

/*------------------------------------------------------------------------------
xlnx_dec_get_buffer_from_fpga: Transfer the buffers stored on fpga into the 
    host.
Parameters:
    ctx: The decoder context
    buffer_size: A pointer to the size of the buffer.
Return:
    A pointer to the host buffer of size * buffer_size
------------------------------------------------------------------------------*/
uint8_t* xlnx_dec_get_buffer_from_fpga(XlnxDecoderCtx* ctx, size_t*
                                       buffer_size);

/*------------------------------------------------------------------------------
xlnx_dec_send_null_data: Send null data to flush the remaining frames out of 
    the fpga
Parameters:
    ctx: The decoder context
Return:
    XMA_SUCCESS on success
    XMA_ERROR on error
------------------------------------------------------------------------------*/
int xlnx_dec_send_null_data(XlnxDecoderCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_dec_send_data: sends data to the decoder for processing
Parameters:
    ctx: ctx for the decoder.
        session: Used to send data to xma plugin
        input_buffer: Contains the data to send 
    end: amount of data to send
Return:
    RET_SUCCESS on success, otherwise RET_ERROR
------------------------------------------------------------------------------*/
int xlnx_dec_send_data(XlnxDecoderCtx* ctx, int end);

/*------------------------------------------------------------------------------
xlnx_dec_print_segment_performance: Print the performance since the last 
    segment.
Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_print_segment_performance(XlnxDecoderCtx* ctx);

/*------------------------------------------------------------------------------
xlnx_dec_print_total_performance: Print the total performance of the decoder.
Parameters:
    ctx: The decoder context
------------------------------------------------------------------------------*/
void xlnx_dec_print_total_performance(XlnxDecoderCtx* ctx);

#endif