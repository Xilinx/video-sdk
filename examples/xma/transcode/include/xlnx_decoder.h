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

#ifndef _XLNX_DECODER_H_
#define _XLNX_DECODER_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <xma.h>
#include <xrm.h>

#include "xlnx_dec_utils.h"
#include "xlnx_app_utils.h"
#include "xlnx_transcoder_constants.h"
#include "xlnx_transcoder_xrm_interface.h"
#include "xlnx_transcoder_xma_props.h"

#define FLAG_DEC_INPUT_FILE       "i"
#define FLAG_DEC_CODEC_TYPE       "c:v"
#define FLAG_DEC_LOW_LATENCY      "low-latency"
#define FLAG_DEC_LATENCY_LOGGING  "latency_logging"


typedef struct XlnxH264SeqParamSet
{
    unsigned char profile_idc;
    unsigned char level_idc;
    unsigned long pic_width_in_mbs_minus1;
    unsigned long pic_height_in_map_units_minus1;
    unsigned long frame_crop_left_offset;
    unsigned long frame_crop_right_offset;
    unsigned long frame_crop_top_offset;
    unsigned long frame_crop_bottom_offset;
    unsigned char chroma_format_idc;
    unsigned char log2_max_frame_num_minus4;
    unsigned char pic_order_cnt_type;
    unsigned char log2_max_pic_order_cnt_lsb_minus4;
    unsigned char delta_pic_order_always_zero_flag;
    unsigned char frame_mbs_only_flag;
    unsigned char frame_cropping_flag;
    unsigned char valid;
}XlnxH264SeqParamSet;

typedef struct XlnxH264PicParamSet
{
    unsigned char seq_parameter_set_id;
    unsigned char pic_order_present_flag;
    unsigned char valid;
}XlnxH264PicParamSet;

typedef struct XlnxH264SliceHeader
{
    int32_t delta_pic_order_cnt_bottom;
    int32_t delta_pic_order_cnt[2];
    unsigned short frame_num;
    unsigned short idr_pic_id;
    unsigned short pic_order_cnt_lsb;
    unsigned char pic_parameter_set_id;
    unsigned char field_pic_flag;
    unsigned char bottom_field_flag;
    unsigned char nal_ref_idc;
    unsigned char nal_unit_type;
}XlnxH264SliceHeader;

typedef struct XlnxHevcShortRps
{
    uint32_t num_negative_pics;
    int32_t num_delta_pocs;
    int32_t delta_poc[32];
    uint8_t used[32];
}XlnxHevcShortRps;

typedef struct XlnxHevcSeqParamSet
{
    unsigned char profile_idc;
    unsigned char level_idc;
    unsigned char nb_st_rps;
    XlnxHevcShortRps st_rps[64];
    unsigned char valid;
}XlnxHevcSeqParamSet;

typedef struct XlnxDecFrameData
{
    int32_t width;
    int32_t height;
    int32_t fr_num;
    int32_t fr_den;
    unsigned char current_h264_pps;
    unsigned char latest_hevc_sps;
    XlnxH264SeqParamSet h264_seq_parameter_set[32];
    XlnxH264PicParamSet h264_pic_parameter_set[256];
    XlnxH264SliceHeader last_h264_slice_header;
    XlnxHevcSeqParamSet hevc_seq_parameter_set[32];
}XlnxDecFrameData;

typedef struct XlnxDecoderCtx {
    XmaDecoderSession  *dec_session;
    XmaFrameProperties props;
    XmaDataBuffer      *dec_in_buf;
    XmaFrame           *out_frame;
    xrmCuListResource   decode_cu_list_res;
    XlnxDecoderProperties  dec_props;
    XlnxDecFrameData       frame_data;
    XlnxDecBuffer          in_buffer;
    int32_t            first_idr_found;
    int32_t            decode_res_inuse;
    int32_t            in_offset;
    int32_t            in_file;
    bool               flush_sent;
} XlnxDecoderCtx;

int32_t xlnx_dec_parse_args(int32_t argc, char *argv[], XlnxDecoderCtx *dec_ctx, 
        int32_t param_flag);
int32_t xlnx_dec_parse_frame(XlnxDecoderCtx *dec_ctx);

void xlnx_dec_context_init(XlnxDecoderCtx *dec_ctx);
void xlnx_dec_update_props(XlnxDecoderCtx *dec_ctx, 
                           XmaDecoderProperties *xma_dec_props);

int32_t xlnx_dec_parse_hevc_au (int32_t file, XlnxDecBuffer *buffer, 
        XlnxDecFrameData *parsedata, int32_t *retoffset);
int32_t xlnx_dec_parse_h264_au (int32_t file, XlnxDecBuffer *buffer, 
        XlnxDecFrameData *parsedata, int32_t *retoffset);

int32_t xlnx_dec_session(XlnxTranscoderXrmCtx *app_xrm_ctx,
                         XlnxDecoderCtx *dec_ctx, 
                         XmaDecoderProperties *xma_dec_props);

int32_t xlnx_dec_read_frame(XlnxDecoderCtx *dec_ctx);
int32_t xlnx_dec_send_frame(XlnxDecoderCtx *dec_ctx);
int32_t xlnx_dec_recv_frame(XlnxDecoderCtx *dec_ctx);
int32_t xlnx_dec_get_input_size(XlnxDecoderCtx *dec_ctx);
int32_t xlnx_dec_send_flush_frame(XlnxDecoderCtx *dec_ctx);
int32_t xlnx_dec_send_null_frame(XlnxDecoderCtx *dec_ctx);

int32_t xlnx_dec_deinit(xrmContext *xrm_ctx, XlnxDecoderCtx *dec_ctx, 
                        XmaDecoderProperties *xma_dec_props);

#endif //_XLNX_DECODER_H_
