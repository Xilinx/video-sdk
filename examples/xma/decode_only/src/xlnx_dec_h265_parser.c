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

#include "xlnx_dec_h265_parser.h"

/*-----------------------------------------------------------------------------
decode_hevc_profile_tier_level: Get HEVC profile from the input
Parameters:
get_bits: structure to read from
_sps: SPS structure
Return:
DEC_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t decode_hevc_profile_tier_level(XlnxDecGetBits* get_bits, 
                                              XlnxHEVCSeqParamSet* sps)
{
    /* profile_space */
    xlnx_dec_get_bits_byte(get_bits, 2);
    /* tier_flag */
    xlnx_dec_get_bits_byte(get_bits, 1);
    unsigned char profile_idc = xlnx_dec_get_bits_byte(get_bits, 5);

    /* profile_compatibility_flags */
    xlnx_dec_get_bits_long(get_bits, 32);

    /* progressive_source_flag */
    xlnx_dec_get_bits_byte(get_bits, 1);
    /* interlaced_source_flag */
    xlnx_dec_get_bits_byte(get_bits, 1);
    /* non_packed_constraint_flag */
    xlnx_dec_get_bits_byte(get_bits, 1);
    /* frame_only_constraint_flag */
    xlnx_dec_get_bits_byte(get_bits, 1);

    xlnx_dec_get_bits_long(get_bits, 32);
    xlnx_dec_get_bits_short(get_bits, 12);

    if(sps)
        sps->profile_idc = profile_idc;

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
hevc_scaling_list_data: Decode HEVC scaling list
Parameters:
get_bits: structure to read from
Return:
DEC_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t hevc_scaling_list_data (XlnxDecGetBits* get_bits)
{
    int32_t size_id;
    for(size_id = 0; size_id < 4; size_id++)
    {
        int32_t matrix_id;
        for(matrix_id = 0; matrix_id < 6; matrix_id += ((size_id == 3) ? 
            3 : 1))
        {
            unsigned char scaling_list_pred_mode_flag = xlnx_dec_get_bits_byte(
                                                                get_bits, 1);
            if(!scaling_list_pred_mode_flag)
            {
                /* delta */
                xlnx_dec_get_bits_unsigned_eg(get_bits);
            } else {
                int32_t coef_num = min (64, 1 << (4 + (size_id << 1)));
                if(size_id > 1)
                {
                    /* scaling_list_dc_coef */
                    xlnx_dec_get_bits_unsigned_eg(get_bits);
                }
                int32_t i;
                for(i = 0; i < coef_num; i++)
                {
                    /* scaling_list_delta_coef */
                    xlnx_dec_get_bits_unsigned_eg(get_bits);
                }
            }
        }
    }
    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
hevc_decode_short_term_rps: Parse HEVC rps header
Parameters:
get_bits: structure to read from
XlnxHEVCShortTermRPS: RPS structure
_sps: SPS structure
Return:
DEC_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t hevc_decode_short_term_rps (XlnxDecGetBits* get_bits, 
                                           XlnxHEVCShortTermRPS* rps, 
                                           XlnxHEVCSeqParamSet* sps)
{
    int32_t i;
    int32_t k = 0;
    int32_t k0 = 0;
    int32_t delta_poc;
    unsigned char rps_predict = 0;
    if(rps != sps->st_rps && sps->nb_st_rps)
        rps_predict = xlnx_dec_get_bits_byte(get_bits, 1);

    if(rps_predict)
    {
        XlnxHEVCShortTermRPS* rps_ridx = &sps->st_rps[rps - sps->st_rps - 1];
        unsigned char delta_rps_sign = xlnx_dec_get_bits_byte(get_bits, 1);
        unsigned long abs_delta_rps = xlnx_dec_get_bits_unsigned_eg(get_bits);
        unsigned char use_delta_flag = 0;
        int32_t delta_rps = (1 - (delta_rps_sign << 1)) * abs_delta_rps;
        for(i = 0; i <= rps_ridx->num_delta_pocs; i++)
        {
            int32_t used = rps->used[k] = xlnx_dec_get_bits_byte(get_bits, 1);
            if(!used)
                use_delta_flag = xlnx_dec_get_bits_byte(get_bits, 1);
            if(used || use_delta_flag)
            {
                if(i < rps_ridx->num_delta_pocs)
                    delta_poc = delta_rps + rps_ridx->delta_poc[i];
                else
                    delta_poc = delta_rps;
                rps->delta_poc[k] = delta_poc;
                if(delta_poc < 0)
                    k0++;
                k++;
            }
        }
        rps->num_delta_pocs = k;
        rps->num_negative_pics = k0;
        if(rps->num_delta_pocs != 0)
        {
            int32_t used, tmp;
            for(i = 1; i < rps->num_delta_pocs; i++)
            {
                delta_poc = rps->delta_poc[i];
                used = rps->used[i];
                for(k = i - 1; k >= 0; k--)
                {
                    tmp = rps->delta_poc[k];
                    if(delta_poc < tmp)
                    {
                        rps->delta_poc[k + 1] = tmp;
                        rps->used[k + 1] = rps->used[k];
                        rps->delta_poc[k] = delta_poc;
                        rps->used[k] = used;
                    }
                }
            }
        }

        if((rps->num_negative_pics >> 1) != 0)
        {
            int32_t used;
            k = rps->num_negative_pics - 1;
            for(i = 0; i < rps->num_negative_pics >> 1; i++)
            {
                delta_poc = rps->delta_poc[i];
                used = rps->used[i];
                rps->delta_poc[i] = rps->delta_poc[k];
                rps->used[i] = rps->used[k];
                rps->delta_poc[k] = delta_poc;
                rps->used[k] = used;
                k--;
            }
        }

    } else {
        unsigned long prev;
        rps->num_negative_pics = xlnx_dec_get_bits_unsigned_eg(get_bits);
        unsigned long nb_positive_pics = xlnx_dec_get_bits_unsigned_eg(
                                                                    get_bits);
        rps->num_delta_pocs = rps->num_negative_pics + nb_positive_pics;
        if(rps->num_delta_pocs)
        {
            prev = 0;
            for(i = 0; i < rps->num_negative_pics; i++)
            {
                delta_poc = xlnx_dec_get_bits_unsigned_eg(get_bits) + 1;
                prev -= delta_poc;
                rps->delta_poc[i] = prev;
                rps->used[i] = xlnx_dec_get_bits_byte(get_bits, 1);
            }
            prev = 0;
            for(i = 0; i < nb_positive_pics; i++) {
                delta_poc = xlnx_dec_get_bits_unsigned_eg(get_bits) + 1;
                prev += delta_poc;
                rps->delta_poc[rps->num_negative_pics + i] = prev;
                rps->used[rps->num_negative_pics + i] = xlnx_dec_get_bits_byte(
                                                                get_bits, 1);
            }
        }
    }

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
parse_hevc_sps: Parse HEVC sps
Parameters:
_file: Input file
_buffer: Decoder input buffer
parse_data: structure for parsed data
start_offset: Current offset for the input buffer
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error.
-----------------------------------------------------------------------------*/
static int32_t parse_hevc_sps (int32_t file, XlnxDecBuffer* buffer, 
                               XlnxDecFrameParseData* parse_data, 
                               int32_t start_offset)
{
    int32_t endoffset;
    int32_t ret = xlnx_dec_find_next_start_code(file, buffer, start_offset + 1, 
            &endoffset);
    if(ret == RET_EOF)
        endoffset = buffer->size;
    if(ret == DEC_APP_ERROR)
        return DEC_APP_ERROR;

    XlnxDecBuffer nbuffer;
    ret = xlnx_dec_convert_to_rbsp(buffer, start_offset, endoffset, &nbuffer);
    if(ret == DEC_APP_ERROR)
        return DEC_APP_ERROR;

    unsigned char* pt = nbuffer.data + 5;
    unsigned char* end = nbuffer.data + nbuffer.size;
    XlnxDecGetBits getbits;

    xlnx_dec_init_get_bits(&getbits, pt, end);

    XlnxHEVCSeqParamSet sps;
    memset (&sps, 0, sizeof (sps));

    /* vps_id */
    xlnx_dec_get_bits_byte(&getbits, 4);
    unsigned char max_sub_layers = xlnx_dec_get_bits_byte(&getbits, 3) + 1;
    /* temporal_id_nesting_flag */
    xlnx_dec_get_bits_byte(&getbits, 1);

    decode_hevc_profile_tier_level (&getbits, &sps);

    sps.level_idc = xlnx_dec_get_bits_byte(&getbits, 8);

    unsigned char sub_layer_profile_present_flag[8];
    unsigned char sub_layer_level_present_flag[8];
    int32_t i;
    for(i = 0; i < max_sub_layers - 1; i++)
    {
        sub_layer_profile_present_flag[i] = xlnx_dec_get_bits_byte(&getbits, 1);
        sub_layer_level_present_flag[i] = xlnx_dec_get_bits_byte(&getbits, 1);
    }

    if(max_sub_layers > 1)
    {
        for(i = max_sub_layers - 1; i < 8; i++)
            xlnx_dec_get_bits_byte(&getbits, 2);
    }
    for(i = 0; i < max_sub_layers - 1; i++)
    {
        if(sub_layer_profile_present_flag[i])
            decode_hevc_profile_tier_level(&getbits, NULL);
        if(sub_layer_level_present_flag[i])
        {
            /* sub_layer_ptl_level_idc */
            xlnx_dec_get_bits_byte(&getbits, 8);
        }
    }

    unsigned long sps_id = xlnx_dec_get_bits_unsigned_eg(&getbits);
    parse_data->latest_hevc_sps = sps_id;
    unsigned long chroma_format_idc = xlnx_dec_get_bits_unsigned_eg(&getbits);
    if(chroma_format_idc == 3)
    {
        unsigned char separate_colour_plane_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(separate_colour_plane_flag)
            chroma_format_idc = 0;
    }

    unsigned long coded_width = xlnx_dec_get_bits_unsigned_eg(&getbits);
    unsigned long coded_height = xlnx_dec_get_bits_unsigned_eg(&getbits);
    parse_data->width = coded_width;
    parse_data->height = coded_height;

    unsigned char pic_conformance_flag = xlnx_dec_get_bits_byte(&getbits, 1);
    if(pic_conformance_flag)
    {
        int32_t vert_mult = 1 + (chroma_format_idc < 2);
        int32_t horiz_mult = 1 + (chroma_format_idc < 3);
        unsigned long output_window_left_offset, output_window_right_offset,
        output_window_top_offset, output_window_bottom_offset;
        /* output_window_left_offset */
        output_window_left_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * horiz_mult;
        /* output_window_right_offset */
        output_window_right_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * horiz_mult;
        /* output_window_top_offset */
        output_window_top_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * vert_mult;
        /* output_window_bottom_offset */
        output_window_bottom_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * vert_mult;
        parse_data->width  = coded_width - (output_window_left_offset + 
                             output_window_right_offset);
        parse_data->height = coded_height - (output_window_top_offset + 
                             output_window_bottom_offset);
    }

    /* bit_depth = value + 8 */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    /* bit_depth_chroma = value + 8 */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    unsigned long log2_max_poc_lsb = xlnx_dec_get_bits_unsigned_eg(&getbits) + 4;

    unsigned char sublayer_ordering_info = xlnx_dec_get_bits_byte(&getbits, 1);
    unsigned char start = sublayer_ordering_info ? 0 : max_sub_layers - 1;
    for(i = start; i < max_sub_layers; i++)
    {
        /* temporal_layer_max_dec_pic_buffering = value + 1 */
        xlnx_dec_get_bits_unsigned_eg(&getbits);
        /* temporal_layer_num_reorder_pics */
        xlnx_dec_get_bits_unsigned_eg(&getbits);
        /* temporal_layer_max_latency_increase = value - 1 */
        xlnx_dec_get_bits_unsigned_eg(&getbits);
    }

    /* log2_min_cb_size = value + 3 */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    /* log2_diff_max_min_coding_block_size */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    /* log2_min_tb_size = value + 2 */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    /* log2_diff_max_min_transform_block_size */
    xlnx_dec_get_bits_unsigned_eg(&getbits);

    /* max_transform_hierarchy_depth_inter */
    xlnx_dec_get_bits_unsigned_eg(&getbits);
    /* max_transform_hierarchy_depth_intra */
    xlnx_dec_get_bits_unsigned_eg(&getbits);

    unsigned char scaling_list_enable_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
    if(scaling_list_enable_flag)
    {
        if(xlnx_dec_get_bits_byte(&getbits, 1))
            hevc_scaling_list_data (&getbits);
    }

    /* amp_enabled_flag */
    xlnx_dec_get_bits_byte(&getbits, 1);
    /* sao_enabled */
    xlnx_dec_get_bits_byte(&getbits, 1);
    unsigned char pcm_enabled_flag = xlnx_dec_get_bits_byte(&getbits, 1);
    if(pcm_enabled_flag)
    {
        /* pcm_bit_depth = value + 1 */
        xlnx_dec_get_bits_byte(&getbits, 4);
        /* pcm_bit_depth_chroma = value + 1 */
        xlnx_dec_get_bits_byte(&getbits, 4);
        /* pcm_log2_min_pcm_cb_size = value + 3 */
        xlnx_dec_get_bits_unsigned_eg(&getbits);
        /* pcm_log2_max_pcm_cb_size = pcm_log2_min_pcm_cb_size + 
           xlnx_dec_get_bits_unsigned_eg(&getbits); */
        xlnx_dec_get_bits_unsigned_eg(&getbits);
        /* pcm_loop_filter_disable_flag */
        xlnx_dec_get_bits_byte(&getbits, 1);
    }

    sps.nb_st_rps = xlnx_dec_get_bits_unsigned_eg(&getbits);
    for(i = 0; i < sps.nb_st_rps; i++)
        hevc_decode_short_term_rps (&getbits, &sps.st_rps[i], &sps);

    unsigned char long_term_ref_pics_present_flag = xlnx_dec_get_bits_byte(
                                                                  &getbits, 1);
    if(long_term_ref_pics_present_flag)
    {
        unsigned long num_long_term_ref_pics_sps = xlnx_dec_get_bits_unsigned_eg(
                                                                      &getbits);
        for(i = 0; i < num_long_term_ref_pics_sps; i++)
        {
            /* lt_ref_pic_poc_lsb_sps */
            xlnx_dec_get_bits_byte(&getbits, log2_max_poc_lsb);
            /* used_by_curr_pic_lt_sps_flag */
            xlnx_dec_get_bits_byte(&getbits, 1);
        }
    }

    /* sps_temporal_mvp_enabled_flag */
    xlnx_dec_get_bits_byte(&getbits, 1);
    /* sps_strong_intra_smoothing_enable_flag */ 
    xlnx_dec_get_bits_byte(&getbits, 1);

    unsigned long def_disp_win_left_offset = 0;
    unsigned long def_disp_win_right_offset = 0;
    unsigned long def_disp_win_top_offset = 0;
    unsigned long def_disp_win_bottom_offset = 0;
    unsigned long vui_num_units_in_tick = 0;
    unsigned long vui_time_scale = 0;

    unsigned char vui_present = xlnx_dec_get_bits_byte(&getbits, 1);
    if(vui_present)
    {
        unsigned char sar_present = xlnx_dec_get_bits_byte(&getbits, 1);
        if(sar_present)
        {
            unsigned char sar_idx = xlnx_dec_get_bits_byte(&getbits, 8);
            if(sar_idx == 255)
            {
                /* sar_num */
                xlnx_dec_get_bits_short(&getbits, 16);
                /* sar_den */
                xlnx_dec_get_bits_short(&getbits, 16);
            }
        }

        unsigned char overscan_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(overscan_info_present_flag)
        {
            /* overscan_appropriate_flag */
            xlnx_dec_get_bits_byte(&getbits, 1);
        }

        unsigned char video_signal_type_present_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(video_signal_type_present_flag)
        {
            /* video_format */
            xlnx_dec_get_bits_byte(&getbits, 3);
            /* video_full_range_flag */
            xlnx_dec_get_bits_byte(&getbits, 1);
            unsigned char colour_description_present_flag = 
                xlnx_dec_get_bits_byte(&getbits, 1);
            if(colour_description_present_flag)
            {
                /* colour_primaries */
                xlnx_dec_get_bits_byte(&getbits, 8);
                /* transfer_characteristic */
                xlnx_dec_get_bits_byte(&getbits, 8);
                /* matrix_coeffs */
                xlnx_dec_get_bits_byte(&getbits, 8);
            }
        }

        unsigned char chroma_loc_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(chroma_loc_info_present_flag)
        {
            /* chroma_sample_loc_type_top_field */
            xlnx_dec_get_bits_unsigned_eg(&getbits);
            /* chroma_sample_loc_type_bottom_field */
            xlnx_dec_get_bits_unsigned_eg(&getbits);
        }

        /* neutra_chroma_indication_flag */
        xlnx_dec_get_bits_byte(&getbits, 1);
        /* field_seq_flag */
        xlnx_dec_get_bits_byte(&getbits, 1);
        /* frame_field_info_present_flag */
        xlnx_dec_get_bits_byte(&getbits, 1);

        unsigned char default_display_window_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(default_display_window_flag)
        {
            int32_t vert_mult = 1 + (chroma_format_idc < 2);
            int32_t horiz_mult = 1 + (chroma_format_idc < 3);
            def_disp_win_left_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * horiz_mult;
            def_disp_win_right_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * horiz_mult;
            def_disp_win_top_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * vert_mult;
            def_disp_win_bottom_offset = xlnx_dec_get_bits_unsigned_eg(
                                                        &getbits) * vert_mult;
            parse_data->width = coded_width - (def_disp_win_left_offset + 
                                def_disp_win_right_offset);
            parse_data->height = coded_height - (def_disp_win_top_offset + 
                                 def_disp_win_bottom_offset);
        }

        unsigned char vui_timing_info_present_flag = xlnx_dec_get_bits_byte(
                                                                &getbits, 1);
        if(vui_timing_info_present_flag)
        {
            vui_num_units_in_tick = xlnx_dec_get_bits_long(&getbits, 32);
            vui_time_scale = xlnx_dec_get_bits_long(&getbits, 32);
        }
    }

    if(xlnx_dec_get_bits_eof(&getbits))
    {
        free(nbuffer.data);
        return DEC_APP_ERROR;
    }

    parse_data->fr_num = vui_time_scale;
    parse_data->fr_den = vui_num_units_in_tick;
    unsigned long temp = xlnx_utils_gcd(parse_data->fr_num, parse_data->fr_den);
    parse_data->fr_num /= temp;
    parse_data->fr_den /= temp;

    sps.valid = 1;
    parse_data->hevc_seq_parameter_set[sps_id] = sps;

    free(nbuffer.data);

    return DEC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_next_h265_au: Parse HEVC headers
Parameters:
_file: Input file
_buffer: Decoder input buffer
parse_data: structure for parsed data
ret_offset: offset for the input buffer read
Return:
DEC_APP_SUCCESS on success, DEC_APP_ERROR on error, or 
RET_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_parse_next_h265_au(int32_t file, XlnxDecBuffer* buffer, 
                                    XlnxDecFrameParseData* parse_data, 
                                    int32_t* ret_offset)
{
    int32_t startoffset = 0;
    int32_t hasslice = 0;
    while(true)
    {
        int32_t endoffset = 0;
        int32_t ret = xlnx_dec_find_next_start_code(file, buffer, startoffset, 
                                                    &endoffset);
        if(ret == DEC_APP_ERROR) {
            return DEC_APP_ERROR;
        }
        if(ret == RET_EOF) {
            *ret_offset = buffer->size;
            return RET_EOF;
        }
        if(buffer->size <= endoffset + 6) {
            int32_t ret = xlnx_dec_get_in_buf(file, buffer, 6);
            if(ret == DEC_APP_ERROR)
                return DEC_APP_ERROR;
            if(ret == RET_EOF) {
                *ret_offset = buffer->size;
                return RET_EOF;
            }
        }
        unsigned char nalutype = (buffer->data[endoffset + 3] & 0x7E) >> 1;
        if(nalutype == 33) 
            ret = parse_hevc_sps (file, buffer, parse_data, endoffset);
        if(ret == DEC_APP_ERROR) {
            return DEC_APP_ERROR;
        }
        if((nalutype >= 32 && nalutype <= 35) || nalutype == 39 || 
           (nalutype >= 41 && nalutype <= 44) || (nalutype >= 48 && 
           nalutype <= 55)) {
            if(hasslice) {
                *ret_offset = endoffset;
                return DEC_APP_SUCCESS;
            }
        } else if(nalutype <= 9 || (nalutype >= 16 && nalutype <= 21)) {
            int32_t first_slice_segment_in_pic_flag = buffer->
                                                      data[endoffset + 5] >> 7;
            if(hasslice && first_slice_segment_in_pic_flag) {
                *ret_offset = endoffset;
                return DEC_APP_SUCCESS;
            }
            hasslice = 1;
        }
        startoffset = endoffset + 1;
    }
}
