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

#include "xlnx_dec_common_parser.h"
#include "xlnx_dec_h264_parser.h"

/*-----------------------------------------------------------------------------
parse_h264_scaling_list: Parse H.264 scaling list
Parameters:
size_of_list: size of the scaling list
get_bits: structure to read from
Return:
DEC_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t parse_h264_scaling_list(int32_t size_of_list, 
                                       XlnxDecGetBits* get_bits)
{
    int32_t last_scale = 8;
    int32_t next_scale = 8;
    for(int32_t i = 0; i < size_of_list; i++)
    {
        if(next_scale != 0)
        {
            int32_t deltascale = xlnx_dec_get_bits_signed_eg(get_bits);
            next_scale = (last_scale + deltascale + 256) % 256;
        }
        int32_t scalinglist = (next_scale == 0) ? last_scale : next_scale;
        last_scale = scalinglist;
    }
    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
parse_h264_sps: Parse H.264 sps
Parameters:
file: Input file
buffer: Decoder input buffer
parse_data: structure for parsed data
start_offset: Current offset for the input buffer
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error.
-----------------------------------------------------------------------------*/
static int32_t parse_h264_sps(int32_t file, XlnxDecBuffer* buffer, 
                              XlnxDecFrameParseData* parse_data, 
                              int32_t start_offset)
{
    int32_t end_offset;
    int32_t ret = xlnx_dec_find_next_start_code(file, buffer, start_offset + 1, 
            &end_offset);
    if(ret == RET_EOF)
        end_offset = buffer->size;
    if(ret == DEC_APP_ERROR) {
        DECODER_APP_LOG_ERROR("Failed in finding next start code in sps "
                              "parsing.\n");
        return DEC_APP_ERROR;
    }

    XlnxDecBuffer nbuffer;
    ret = xlnx_dec_convert_to_rbsp(buffer, start_offset, end_offset, &nbuffer);
    if(ret == DEC_APP_ERROR) {
        DECODER_APP_LOG_ERROR("Failed in converting to rbsp in sps parsing \n");
        return DEC_APP_ERROR;
    }

    unsigned char* pt = nbuffer.data + 4;
    unsigned char* end = nbuffer.data + nbuffer.size;
    XlnxDecGetBits get_bits;
    XlnxH264SeqParamSet sps;
    memset (&sps, 0, sizeof (sps));
    int32_t i;

    xlnx_dec_init_get_bits(&get_bits, pt, end);

    sps.profile_idc = xlnx_dec_get_bits_byte(&get_bits, 8);
    /* constraint_set_flag */
    xlnx_dec_get_bits_byte(&get_bits, 8);
    sps.level_idc = xlnx_dec_get_bits_byte(&get_bits, 8);
    unsigned char seq_parameter_set_id = xlnx_dec_get_bits_unsigned_eg(
                                                                    &get_bits);

    if((sps.profile_idc == 100) ||
            (sps.profile_idc == 110) ||
            (sps.profile_idc == 122) ||
            (sps.profile_idc == 144))
    {
        sps.chroma_format_idc = xlnx_dec_get_bits_unsigned_eg(&get_bits);
        if(sps.chroma_format_idc == 3)
        {
            /* residual_colour_transform_flag */
            xlnx_dec_get_bits_byte(&get_bits, 1);
        }
        /* bit_depth_luma_minus8 */
        xlnx_dec_get_bits_unsigned_eg(&get_bits);
        /* bit_depth_chroma_minus8 */
        xlnx_dec_get_bits_unsigned_eg(&get_bits);
        /* qpprime_y_zero_transform_bypass_flag */
        xlnx_dec_get_bits_byte(&get_bits, 1);
        unsigned char seq_scaling_matrix_present_flag = xlnx_dec_get_bits_byte(
                                                                  &get_bits, 1);

        unsigned char seq_scaling_list_present_flag[8];
        if(seq_scaling_matrix_present_flag)
        {
            for(i = 0; i < 8; i++)
            {
                /* seq_scaling_list_present_flag[8] */
                seq_scaling_list_present_flag[i] = xlnx_dec_get_bits_byte(
                                                                 &get_bits, 1);
                if(seq_scaling_list_present_flag[i]) {
                    if(i < 6)
                        parse_h264_scaling_list(16, &get_bits);
                    else
                        parse_h264_scaling_list(64, &get_bits);
                }
            }
        }
    } else
        sps.chroma_format_idc = 1;

    sps.log2_max_frame_num_minus4 = xlnx_dec_get_bits_unsigned_eg(&get_bits);
    sps.pic_order_cnt_type = xlnx_dec_get_bits_unsigned_eg(&get_bits);

    if(sps.pic_order_cnt_type == 0)
        sps.log2_max_pic_order_cnt_lsb_minus4 = xlnx_dec_get_bits_unsigned_eg(
                                                                    &get_bits);
    else if(sps.pic_order_cnt_type == 1)
    {
        sps.delta_pic_order_always_zero_flag = xlnx_dec_get_bits_byte(
                                                                 &get_bits, 1);
        /* offset_for_non_ref_pic */
        xlnx_dec_get_bits_signed_eg(&get_bits);
        /* offset_for_top_to_bottom_field */
        xlnx_dec_get_bits_signed_eg(&get_bits);
        unsigned char num_ref_frames_in_pic_order_cnt_cycle = 
            xlnx_dec_get_bits_unsigned_eg(&get_bits);
        for(i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
            xlnx_dec_get_bits_signed_eg(&get_bits);
    }

    /* num_ref_frames */
    xlnx_dec_get_bits_unsigned_eg(&get_bits);
    /* gaps_in_frame_num_value_allowed_flag */
    xlnx_dec_get_bits_byte(&get_bits, 1);
    sps.pic_width_in_mbs_minus1 = xlnx_dec_get_bits_unsigned_eg(&get_bits);
    sps.pic_height_in_map_units_minus1 = xlnx_dec_get_bits_unsigned_eg(
                                                                    &get_bits);
    sps.frame_mbs_only_flag = xlnx_dec_get_bits_byte(&get_bits, 1);

    if(!sps.frame_mbs_only_flag)
    {
        /* mb_adaptive_frame_field_flag */
        xlnx_dec_get_bits_byte(&get_bits, 1);
    }

    /* direct_8x8_inference_flag */
    xlnx_dec_get_bits_byte(&get_bits, 1);
    sps.frame_cropping_flag = xlnx_dec_get_bits_byte(&get_bits, 1);
    if(sps.frame_cropping_flag)
    {
        sps.frame_crop_left_offset = xlnx_dec_get_bits_unsigned_eg(&get_bits);
        sps.frame_crop_right_offset = xlnx_dec_get_bits_unsigned_eg(&get_bits);
        sps.frame_crop_top_offset = xlnx_dec_get_bits_unsigned_eg(&get_bits);
        sps.frame_crop_bottom_offset = xlnx_dec_get_bits_unsigned_eg(&get_bits);
    }

    unsigned char timing_info_present_flag = 0;
    unsigned long num_units_in_tick = 0;
    unsigned long time_scale = 0;

    unsigned char vui_parameters_present_flag = xlnx_dec_get_bits_byte(
                                                                 &get_bits, 1);
    if(vui_parameters_present_flag)
    {
        unsigned char aspect_ratio_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &get_bits, 1);
        if(aspect_ratio_info_present_flag)
        {
            unsigned char aspect_ratio_idc = xlnx_dec_get_bits_byte(
                                                                 &get_bits, 8);
            if(aspect_ratio_idc == 255)
            {
                /* sar_width */
                xlnx_dec_get_bits_short(&get_bits, 16);
                /* sar_height */
                xlnx_dec_get_bits_short(&get_bits, 16);
            }
        }

        unsigned char overscan_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &get_bits, 1);
        if(overscan_info_present_flag)
        {
            /* overscan_appropriate_flag */
            xlnx_dec_get_bits_byte(&get_bits, 1);
        }

        unsigned char video_signal_type_present_flag = xlnx_dec_get_bits_byte(
                                                                &get_bits, 1);
        if(video_signal_type_present_flag)
        {
            /* video_format */
            xlnx_dec_get_bits_byte(&get_bits, 3);
            /* video_full_range_flag */
            xlnx_dec_get_bits_byte(&get_bits, 1);
            unsigned char colour_description_present_flag = 
                xlnx_dec_get_bits_byte(&get_bits, 1);
            if(colour_description_present_flag)
            {
                /* colour_primaries */
                xlnx_dec_get_bits_byte(&get_bits, 8);
                /* transfer_characteristics */
                xlnx_dec_get_bits_byte(&get_bits, 8);
                /* matrix_coefficients */
                xlnx_dec_get_bits_byte(&get_bits, 8);
            }
        }

        unsigned char chroma_loc_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &get_bits, 1);
        if(chroma_loc_info_present_flag)
        {
            /* chroma_sample_loc_type_top_field */
            xlnx_dec_get_bits_unsigned_eg(&get_bits);
            /* chroma_sample_loc_type_bottom_field */
            xlnx_dec_get_bits_unsigned_eg(&get_bits);
        }

        timing_info_present_flag = xlnx_dec_get_bits_byte(&get_bits, 1);
        if(timing_info_present_flag)
        {
            num_units_in_tick = xlnx_dec_get_bits_long(&get_bits, 32);
            time_scale = xlnx_dec_get_bits_long(&get_bits, 32);
            /* fixed_frame_rate_flag */
            xlnx_dec_get_bits_byte(&get_bits, 1);
        }
    }

    if(xlnx_dec_get_bits_eof(&get_bits))
    {
        free(nbuffer.data);
        return DEC_APP_ERROR;
    }

    parse_data->height = ((2 - sps.frame_mbs_only_flag) * 
            (sps.pic_height_in_map_units_minus1 + 1)) * 16;
    parse_data->width = (sps.pic_width_in_mbs_minus1 + 1) * 16;
    if(sps.frame_cropping_flag)
    {
        int32_t cropunitx;
        int32_t cropunity;
        if(sps.chroma_format_idc == 0)
        {
            // mono
            cropunitx = 1;
            cropunity = 2 - sps.frame_mbs_only_flag;
        } else if(sps.chroma_format_idc == 1)
        {
            // 4:2:0
            cropunitx = 2;
            cropunity = 2 * (2 - sps.frame_mbs_only_flag);
        } else if(sps.chroma_format_idc == 2)
        {
            // 4:2:2
            cropunitx = 2;
            cropunity = 2 - sps.frame_mbs_only_flag;
        } else if(sps.chroma_format_idc == 3)
        {
            // 4:4:4
            cropunitx = 1;
            cropunity = 2 - sps.frame_mbs_only_flag;
        }
        parse_data->width -= cropunitx * (sps.frame_crop_left_offset + 
                sps.frame_crop_right_offset);
        parse_data->height -= cropunity * (sps.frame_crop_top_offset + 
                sps.frame_crop_bottom_offset);
    }

    if(timing_info_present_flag)
    {
        parse_data->fr_num = time_scale;
        parse_data->fr_den = num_units_in_tick * 2;
        unsigned long temp = xlnx_utils_gcd(parse_data->fr_num, 
                                            parse_data->fr_den);
        parse_data->fr_num /= temp;
        parse_data->fr_den /= temp;
    }

    sps.valid = 1;
    parse_data->h264_seq_parameter_set[seq_parameter_set_id] = sps;

    free(nbuffer.data);

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
parse_h264_pps: Parse H.264 pps
Parameters:
file: Input file
buffer: Decoder input buffer
parse_data: structure for parsed data
start_offset: Current offset for the input buffer
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error.
-----------------------------------------------------------------------------*/
static int32_t parse_h264_pps (int32_t file, XlnxDecBuffer* buffer, 
        XlnxDecFrameParseData* parse_data, int32_t start_offset)
{
    int32_t end_offset;
    int32_t ret = xlnx_dec_find_next_start_code(file, buffer, start_offset + 1, 
            &end_offset);
    if(ret == RET_EOF)
        end_offset = buffer->size;
    if(ret == DEC_APP_ERROR)
        return DEC_APP_ERROR;

    XlnxDecBuffer nbuffer;
    ret = xlnx_dec_convert_to_rbsp(buffer, start_offset, end_offset, &nbuffer);
    if(ret == DEC_APP_ERROR)
        return DEC_APP_ERROR;

    unsigned char* pt = nbuffer.data + 4;
    unsigned char* end = nbuffer.data + nbuffer.size;
    XlnxDecGetBits get_bits;
    XlnxH264PicParamSet pps;
    memset (&pps, 0, sizeof (pps));

    xlnx_dec_init_get_bits(&get_bits, pt, end);

    unsigned char pic_parameter_set_id = xlnx_dec_get_bits_unsigned_eg(
                                                                    &get_bits);
    pps.seq_parameter_set_id = xlnx_dec_get_bits_unsigned_eg(&get_bits);
    /* entropy_coding_mode_flag */
    xlnx_dec_get_bits_byte(&get_bits, 1);
    pps.pic_order_present_flag = xlnx_dec_get_bits_byte(&get_bits, 1);

    if(xlnx_dec_get_bits_eof(&get_bits))
    {
        free(nbuffer.data);
        return DEC_APP_ERROR;
    }

    pps.valid = 1;
    parse_data->h264_pic_parameter_set[pic_parameter_set_id] = pps;

    free(nbuffer.data);

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
parse_h264_slice_header: Parse H.264 slice header
Parameters:
file: Input file
buffer: Decoder input buffer
parse_data: structure for parsed data
slice_header: Slice header structure
start_offset: Current offset for the input buffer
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error, or 
RET_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
static int32_t parse_h264_slice_header (int32_t file, XlnxDecBuffer* buffer, 
                                        XlnxDecFrameParseData* parse_data, 
                                        XlnxH264SliceHeader* slice_header, 
                                        int32_t start_offset)
{
    int32_t end_offset;
    int32_t ret = xlnx_dec_find_next_start_code(file, buffer, start_offset + 1, 
            &end_offset);
    if(ret == RET_EOF) {
        DECODER_APP_LOG_INFO("Got EOF in slice header %d \n", __LINE__);
        end_offset = buffer->size;
    }
    if(ret == DEC_APP_ERROR) {
        DECODER_APP_LOG_ERROR("Got Error in slice header %d \n", __LINE__);
        return DEC_APP_ERROR;
    }

    XlnxDecBuffer nbuffer;
    ret = xlnx_dec_convert_to_rbsp(buffer, start_offset, end_offset, &nbuffer);
    if(ret == DEC_APP_ERROR) {
        DECODER_APP_LOG_ERROR("Got Error in slice header %d \n", __LINE__);
        return DEC_APP_ERROR;
    }

    unsigned char* pt = nbuffer.data + 3;
    unsigned char* end = nbuffer.data + nbuffer.size;
    XlnxDecGetBits get_bits;

    xlnx_dec_init_get_bits(&get_bits, pt, end);

    xlnx_dec_get_bits_byte(&get_bits, 1); // skip forbidden zero bit
    slice_header->nal_ref_idc = xlnx_dec_get_bits_byte(&get_bits, 2);
    slice_header->nal_unit_type = xlnx_dec_get_bits_byte(&get_bits, 5);

    /* first_mb_in_slice */
    xlnx_dec_get_bits_unsigned_eg(&get_bits);
    /* slice_type */
    xlnx_dec_get_bits_unsigned_eg(&get_bits);
    slice_header->pic_parameter_set_id = xlnx_dec_get_bits_unsigned_eg(
                                                                    &get_bits);
    parse_data->current_h264_pps = slice_header->pic_parameter_set_id;

    XlnxH264PicParamSet pps = parse_data->h264_pic_parameter_set[
        slice_header->pic_parameter_set_id];
    if(!pps.valid)
    {
        free(nbuffer.data);
        DECODER_APP_LOG_ERROR("Got Error in slice header %d \n", __LINE__);
        return DEC_APP_ERROR;
    }
    XlnxH264SeqParamSet sps = parse_data->h264_seq_parameter_set[
        pps.seq_parameter_set_id];
    if(!sps.valid)
    {
        free(nbuffer.data);
        DECODER_APP_LOG_ERROR("Got Error in slice header %d \n", __LINE__);
        return DEC_APP_ERROR;
    }

    slice_header->frame_num = xlnx_dec_get_bits_short(&get_bits, 
            sps.log2_max_frame_num_minus4 + 4);

    if(!sps.frame_mbs_only_flag)
    {
        slice_header->field_pic_flag = xlnx_dec_get_bits_byte(&get_bits, 1);
        if(slice_header->field_pic_flag)
            slice_header->bottom_field_flag = xlnx_dec_get_bits_byte(
                                                                &get_bits, 1);
    }

    if(slice_header->nal_unit_type == 5)
        slice_header->idr_pic_id = xlnx_dec_get_bits_unsigned_eg(&get_bits);

    if(sps.pic_order_cnt_type == 0)
    {
        slice_header->pic_order_cnt_lsb = xlnx_dec_get_bits_short(&get_bits, 
                sps.log2_max_pic_order_cnt_lsb_minus4 + 4);

        if(pps.pic_order_present_flag && !slice_header->field_pic_flag)
            slice_header->delta_pic_order_cnt_bottom = 
                xlnx_dec_get_bits_signed_eg(&get_bits);
    }
    if(sps.pic_order_cnt_type == 1 && !sps.delta_pic_order_always_zero_flag)
    {
        slice_header->delta_pic_order_cnt[0] = xlnx_dec_get_bits_signed_eg(
                                                                    &get_bits);
        if(pps.pic_order_present_flag && !slice_header->field_pic_flag)
            slice_header->delta_pic_order_cnt[1] = xlnx_dec_get_bits_signed_eg(
                                                                    &get_bits);
    }

    if(xlnx_dec_get_bits_eof(&get_bits))
    {
        free(nbuffer.data);
        DECODER_APP_LOG_ERROR("Got error in slice header parsing %d \n", 
                              __LINE__);
        return DEC_APP_ERROR;
    }

    free(nbuffer.data);

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_next_h264_au: Parse H.264 headers
Parameters:
file: Input file
buffer: Decoder input buffer
parse_data: structure for parsed data
ret_offset: offset for the input buffer read
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error, or 
RET_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_parse_next_h264_au (int32_t file, XlnxDecBuffer *buffer, 
        XlnxDecFrameParseData *parse_data, int32_t *ret_offset)
{
    int32_t startoffset = 0;
    int32_t hasslice = 0;
    int32_t ret = DEC_APP_ERROR;

    while(true)
    {
        int32_t end_offset = 0;
        ret = xlnx_dec_find_next_start_code(file, buffer, startoffset, 
                                            &end_offset);
        if(ret == DEC_APP_ERROR) {
            DECODER_APP_LOG_ERROR("Error in finding H.264 start code \n");
            return DEC_APP_ERROR;
        }

        if(ret == RET_EOF)
        {
            *ret_offset = buffer->size;
            DECODER_APP_LOG_INFO("Got EOF in decoder parser %d \n", __LINE__);
            return RET_EOF;
        }
        if(buffer->size <= end_offset + 4)
        {
            int32_t ret = xlnx_dec_get_in_buf(file, buffer, 4);
            if(ret == DEC_APP_ERROR) {
                DECODER_APP_LOG_ERROR("Error in decoder input buffer fill \n");
                return DEC_APP_ERROR;
            }
            if(ret == RET_EOF)
            {
                *ret_offset = buffer->size;
                DECODER_APP_LOG_INFO("Got EOF in decoder parser %d \n", 
                                     __LINE__);
                return RET_EOF;
            }
        }
        unsigned char nalutype = buffer->data[end_offset + 3] & 0x1F;
        if(nalutype == 7) 
            ret = parse_h264_sps (file, buffer, parse_data, end_offset);
        if(nalutype == 8) 
            ret = parse_h264_pps (file, buffer, parse_data, end_offset);
        XlnxH264SliceHeader sliceheader;
        memset (&sliceheader, 0, sizeof (sliceheader));
        if((1 <= nalutype) && (nalutype <= 5)) {
            ret = parse_h264_slice_header (file, buffer, parse_data, 
                    &sliceheader, end_offset);
            if(ret == DEC_APP_ERROR) {
                DECODER_APP_LOG_ERROR("Error parsing H.264 slice header \n");
                return ret;
            }
        }

        if((hasslice == 1) && ((nalutype == 6) || (nalutype == 7) || 
                    (nalutype == 8) || (nalutype == 9) || (nalutype == 14) || 
                    (nalutype == 15) || (nalutype == 16) || (nalutype == 17) || 
                    (nalutype == 18)))
        {
            *ret_offset = end_offset;
            return DEC_APP_SUCCESS;
        }
        if((1 <= nalutype) && (nalutype <= 5))
        {
            if(!hasslice)
            {
                hasslice = 1;
                startoffset = end_offset + 1;
                parse_data->last_h264_slice_header = sliceheader;
                continue;
            }

            if(!parse_data->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].valid)
            {
                startoffset = end_offset + 1;
                continue;
            }
            if(!parse_data->h264_seq_parameter_set [
                    parse_data->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].
                    seq_parameter_set_id].valid)
            {
                startoffset = end_offset + 1;
                continue;
            }
            XlnxH264SeqParamSet sps = parse_data->h264_seq_parameter_set [
                parse_data->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].
                        seq_parameter_set_id];

            unsigned char isnewpic = 0;
            if(parse_data->last_h264_slice_header.frame_num != 
                    sliceheader.frame_num) 
                isnewpic = 1;
            if(parse_data->last_h264_slice_header.pic_parameter_set_id != 
                    sliceheader.pic_parameter_set_id) 
                isnewpic = 1;
            if(parse_data->last_h264_slice_header.field_pic_flag != 
                    sliceheader.field_pic_flag) 
                isnewpic = 1;
            if((sps.frame_mbs_only_flag) && 
                    (parse_data->last_h264_slice_header.field_pic_flag) && 
                    (sliceheader.field_pic_flag) && 
                    (parse_data->last_h264_slice_header.bottom_field_flag != 
                     sliceheader.bottom_field_flag)) 
                isnewpic = 1;
            if((parse_data->last_h264_slice_header.nal_ref_idc != 
                        sliceheader.nal_ref_idc) && 
                    ((parse_data->last_h264_slice_header.nal_ref_idc == 0) || 
                     (sliceheader.nal_ref_idc == 0))) 
                isnewpic = 1;
            if((sps.pic_order_cnt_type == 0) && 
                    ((parse_data->last_h264_slice_header.pic_order_cnt_lsb != 
                      sliceheader.pic_order_cnt_lsb) || 
                     (parse_data->last_h264_slice_header.delta_pic_order_cnt_bottom != 
                      sliceheader.delta_pic_order_cnt_bottom))) 
                isnewpic = 1;
            if((sps.pic_order_cnt_type == 1) && 
                    ((parse_data->last_h264_slice_header.delta_pic_order_cnt[0] != 
                      sliceheader.delta_pic_order_cnt[0]) || 
                     (parse_data->last_h264_slice_header.delta_pic_order_cnt[1] != 
                      sliceheader.delta_pic_order_cnt[1]))) 
                isnewpic = 1;
            if((parse_data->last_h264_slice_header.nal_unit_type != 
                        sliceheader.nal_unit_type) && 
                    ((parse_data->last_h264_slice_header.nal_unit_type == 5) || 
                     (sliceheader.nal_unit_type == 5))) 
                isnewpic = 1;
            if((parse_data->last_h264_slice_header.nal_unit_type == 5) && 
                    (sliceheader.nal_unit_type == 5) && 
                    (parse_data->last_h264_slice_header.idr_pic_id != 
                     sliceheader.idr_pic_id)) 
                isnewpic = 1;

            parse_data->last_h264_slice_header = sliceheader;
            if(isnewpic)
            {
                *ret_offset = end_offset;
                return DEC_APP_SUCCESS;
            }
        }
        startoffset = end_offset + 1;
    }
    return ret;
}
