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

#ifndef _XLNX_DEC_COMMON_PARSER_H_
#define _XLNX_DEC_COMMON_PARSER_H_

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"
#include <xma.h>


typedef void* xlnx_lookahead_t;

typedef struct XlnxDecBuffer {
    uint8_t* data;
    uint32_t size;
    uint32_t allocated;
} XlnxDecBuffer;

typedef struct XlnxDecGetBits {
    const uint8_t* start;
    const uint8_t* end;
    uint32_t       offset_bytes;
    uint8_t        offset_bits;
} XlnxDecGetBits;

struct XlnxTimingInfo {
    long long next_frame_pts;
    long long pts_delta;
};

typedef struct XlnxStateInfo {
    int           in_file;
    long          loop;
    XlnxDecBuffer input_buffer;
} XlnxStateInfo;

typedef struct XlnxH264SeqParamSet {
    uint8_t  profile_idc;
    uint8_t  level_idc;
    uint32_t pic_width_in_mbs_minus1;
    uint32_t pic_height_in_map_units_minus1;
    uint32_t frame_crop_left_offset;
    uint32_t frame_crop_right_offset;
    uint32_t frame_crop_top_offset;
    uint32_t frame_crop_bottom_offset;
    uint8_t  chroma_format_idc;
    uint8_t  log2_max_frame_num_minus4;
    uint8_t  pic_order_cnt_type;
    uint8_t  log2_max_pic_order_cnt_lsb_minus4;
    uint8_t  delta_pic_order_always_zero_flag;
    uint8_t  frame_mbs_only_flag;
    uint8_t  frame_cropping_flag;
    uint8_t  valid;
} XlnxH264SeqParamSet;

typedef struct XlnxH264PicParamSet {
    uint8_t seq_param_set_id;
    uint8_t pic_order_present_flag;
    uint8_t valid;
} XlnxH264PicParamSet;

typedef struct XlnxH264SliceHeader {
    int      delta_pic_order_cnt_bottom;
    int      delta_pic_order_cnt[2];
    uint16_t frame_num;
    uint16_t idr_pic_id;
    uint16_t pic_order_cnt_lsb;
    uint8_t  pic_parameter_set_id;
    uint8_t  field_pic_flag;
    uint8_t  bottom_field_flag;
    uint8_t  nal_ref_idc;
    uint8_t  nal_unit_type;
} XlnxH264SliceHeader;

typedef struct XlnxHEVCShortTermRPS {
    uint32_t num_negative_pics;
    int      num_delta_pocs;
    int32_t  delta_poc[32];
    uint8_t  used[32];
} XlnxHEVCShortTermRPS;

typedef struct XlnxHEVCSeqParamSet {
    uint8_t              profile_idc;
    uint8_t              level_idc;
    uint8_t              nb_st_rps;
    XlnxHEVCShortTermRPS st_rps[64];
    uint8_t              valid;
} XlnxHEVCSeqParamSet;

typedef struct XlnxDecFrameParseData {
    int                 width;
    int                 height;
    int                 fr_num;
    int                 fr_den;
    int                 luma_bit_depth;
    uint8_t             current_h264_pps;
    uint8_t             latest_hevc_sps;
    XlnxH264SeqParamSet h264_seq_parameter_set[32];
    XlnxH264PicParamSet h264_pic_parameter_set[256];
    XlnxH264SliceHeader last_h264_slice_header;
    XlnxHEVCSeqParamSet hevc_seq_parameter_set[32];
} XlnxDecFrameParseData;

typedef struct {
    xlnx_lookahead_t la;
    int              end_found;
} XlnxLaInfo;

void xlnx_dec_init_get_bits(XlnxDecGetBits* get_bits, const uint8_t* start,
                            const uint8_t* end);

int         xlnx_dec_get_bits_eof(XlnxDecGetBits* get_bits);
uint8_t     xlnx_dec_get_bits_byte(XlnxDecGetBits* get_bits, uint8_t bits);
uint16_t    xlnx_dec_get_bits_short(XlnxDecGetBits* get_bits, uint8_t bits);
uint32_t    xlnx_dec_get_bits_long(XlnxDecGetBits* get_bits, uint8_t bits);
uint32_t    xlnx_dec_get_bits_unsigned_eg(XlnxDecGetBits* get_bits);
signed long xlnx_dec_get_bits_signed_eg(XlnxDecGetBits* get_bits);

int32_t xlnx_dec_get_in_buf(int32_t file, XlnxDecBuffer* buffer,
                            const uint32_t min_size);

int xlnx_dec_find_next_start_code(int file, XlnxDecBuffer* buffer, int offset,
                                  int* retoffset);

int xlnx_dec_convert_to_rbsp(XlnxDecBuffer* buffer, int startoffset,
                             int endoffset, XlnxDecBuffer* newbuffer);

int xlnx_dec_init_parse_data(XlnxDecFrameParseData* parsedata);

#endif // _XLNX_DEC_COMMON_PARSER_H_
