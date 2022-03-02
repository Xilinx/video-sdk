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

#ifndef _XLNX_DEC_COMMON_PARSER_H_
#define _XLNX_DEC_COMMON_PARSER_H_

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <xma.h>
#include "xlnx_app_utils.h"
#include "xlnx_dec_common.h"

typedef void *xlnx_lookahead_t;

typedef struct XlnxDecBuffer
{
    unsigned char* data;
    uint32_t size;
    uint32_t allocated;
} XlnxDecBuffer;

typedef struct XlnxDecGetBits
{
    const unsigned char* start;
    const unsigned char* end;
    unsigned int offset_bytes;
    unsigned char offset_bits;
}XlnxDecGetBits;

struct XlnxTimingInfo
{
    long long next_frame_pts;
    long long pts_delta;
};

struct XlnxStateInfo
{
    int in_file;
    long loop;
    XlnxDecBuffer input_buffer;
};

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
} XlnxH264SeqParamSet;

typedef struct XlnxH264PicParamSet
{
    unsigned char seq_parameter_set_id;
    unsigned char pic_order_present_flag;
    unsigned char valid;
} XlnxH264PicParamSet;

typedef struct XlnxH264SliceHeader
{
    int delta_pic_order_cnt_bottom;
    int delta_pic_order_cnt[2];
    unsigned short frame_num;
    unsigned short idr_pic_id;
    unsigned short pic_order_cnt_lsb;
    unsigned char pic_parameter_set_id;
    unsigned char field_pic_flag;
    unsigned char bottom_field_flag;
    unsigned char nal_ref_idc;
    unsigned char nal_unit_type;
} XlnxH264SliceHeader;

typedef struct XlnxHEVCShortTermRPS
{
    unsigned int num_negative_pics;
    int num_delta_pocs;
    int32_t delta_poc[32];
    uint8_t used[32];
} XlnxHEVCShortTermRPS;

typedef struct XlnxHEVCSeqParamSet
{
    unsigned char profile_idc;
    unsigned char level_idc;
    unsigned char nb_st_rps;
    XlnxHEVCShortTermRPS st_rps[64];
    unsigned char valid;
} XlnxHEVCSeqParamSet;

typedef struct XlnxDecFrameParseData
{
    int width;
    int height;
    int fr_num;
    int fr_den;
    unsigned char current_h264_pps;
    unsigned char latest_hevc_sps;
    XlnxH264SeqParamSet h264_seq_parameter_set[32];
    XlnxH264PicParamSet h264_pic_parameter_set[256];
    XlnxH264SliceHeader last_h264_slice_header;
    XlnxHEVCSeqParamSet hevc_seq_parameter_set[32];
} XlnxDecFrameParseData;

typedef struct {
    xlnx_lookahead_t la;
    int end_found;
} XlnxLaInfo;


void xlnx_dec_init_get_bits(XlnxDecGetBits* get_bits, 
                            const unsigned char* start, 
                            const unsigned char* end);

int xlnx_dec_get_bits_eof(XlnxDecGetBits* get_bits);
unsigned char xlnx_dec_get_bits_byte(XlnxDecGetBits* get_bits, 
                                     unsigned char bits);
unsigned short xlnx_dec_get_bits_short(XlnxDecGetBits* get_bits, 
                                       unsigned char bits);
unsigned long xlnx_dec_get_bits_long(XlnxDecGetBits* get_bits, 
                                     unsigned char bits);
unsigned long xlnx_dec_get_bits_unsigned_eg(XlnxDecGetBits* get_bits);
signed long xlnx_dec_get_bits_signed_eg(XlnxDecGetBits* get_bits);

int32_t xlnx_dec_get_in_buf(int32_t file, XlnxDecBuffer *buffer, 
                            const uint32_t min_size);

int xlnx_dec_find_next_start_code(int file, XlnxDecBuffer* buffer, int offset, 
                                  int* retoffset);

int xlnx_dec_convert_to_rbsp(XlnxDecBuffer* buffer, int startoffset, 
                             int endoffset, XlnxDecBuffer* newbuffer);

int xlnx_dec_init_parse_data(XlnxDecFrameParseData* parsedata);

#endif // _XLNX_DEC_COMMON_PARSER_H_
