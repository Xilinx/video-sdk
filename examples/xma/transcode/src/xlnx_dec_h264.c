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

#include "xlnx_decoder.h"
#include "xlnx_transcoder.h"

/*-----------------------------------------------------------------------------
xlnx_dec_parse_h264_scaling_list: Parse H.264 scaling list
Parameters:
sizeoflist: size of the scaling list
getbits: structure to read from
Return:
TRANSCODE_APP_SUCCESS on success
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_parse_h264_scaling_list (int32_t sizeoflist, 
        XlnxDecGetBits* getbits)
{
    int32_t lastscale = 8;
    int32_t nextscale = 8;
    for (int32_t i = 0; i < sizeoflist; i++)
    {
        if (nextscale != 0)
        {
            int32_t deltascale = xlnx_dec_get_bits_signed_eg (getbits);
            nextscale = (lastscale + deltascale + 256) % 256;
        }
        int32_t scalinglist = (nextscale == 0) ? lastscale : nextscale;
        lastscale = scalinglist;
    }
    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_h264_sps: Parse H.264 sps
Parameters:
file: Input file
dec_buffer: Decoder input buffer
parsedata: structure for parsed data
startoffset: Current offset for the input buffer
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on error.
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_parse_h264_sps (int32_t file, XlnxDecBuffer* 
        dec_buffer, XlnxDecFrameData* parsedata, 
        int32_t startoffset)
{
    int32_t endoffset;
    int32_t ret = xlnx_dec_find_next_start_code (file, dec_buffer, startoffset + 1, 
            &endoffset);
    if (ret == DEC_INPUT_EOF)
        endoffset = dec_buffer->size;
    if (ret == TRANSCODE_APP_FAILURE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed in finding next start code in sps parsing \n");
        return TRANSCODE_APP_FAILURE;
    }

    XlnxDecBuffer buffer;
    ret = xlnx_dec_convert_to_rbsp (dec_buffer, startoffset, endoffset, &buffer);
    if (ret == TRANSCODE_APP_FAILURE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed in converting to rbsp in sps parsing \n");
        return TRANSCODE_APP_FAILURE;
    }

    unsigned char* pt = buffer.data + 4;
    unsigned char* end = buffer.data + buffer.size;
    XlnxDecGetBits getbits;
    XlnxH264SeqParamSet sps;
    memset (&sps, 0, sizeof (sps));
    int32_t i;

    xlnx_dec_init_get_bits (&getbits, pt, end);

    sps.profile_idc = xlnx_dec_get_bits_byte (&getbits, 8);
    /* constraint_set_flag */
    xlnx_dec_get_bits_byte (&getbits, 8);
    sps.level_idc = xlnx_dec_get_bits_byte (&getbits, 8);
    unsigned char seq_parameter_set_id = xlnx_dec_get_bits_unsigned_eg (&getbits);

    if ((sps.profile_idc == 100) ||
            (sps.profile_idc == 110) ||
            (sps.profile_idc == 122) ||
            (sps.profile_idc == 144))
    {
        sps.chroma_format_idc = xlnx_dec_get_bits_unsigned_eg (&getbits);
        if (sps.chroma_format_idc == 3)
        {
            /* residual_colour_transform_flag */
            xlnx_dec_get_bits_byte (&getbits, 1);
        }
        /* bit_depth_luma_minus8 */
        xlnx_dec_get_bits_unsigned_eg (&getbits);
        /* bit_depth_chroma_minus8 */
        xlnx_dec_get_bits_unsigned_eg (&getbits);
        /* qpprime_y_zero_transform_bypass_flag */
        xlnx_dec_get_bits_byte (&getbits, 1);
        unsigned char seq_scaling_matrix_present_flag = 
                                                   xlnx_dec_get_bits_byte (&getbits, 1);

        unsigned char seq_scaling_list_present_flag[8];
        if (seq_scaling_matrix_present_flag)
        {
            for (i = 0; i < 8; i++)
            {
                /* seq_scaling_list_present_flag[8] */
                seq_scaling_list_present_flag[i] = xlnx_dec_get_bits_byte (&getbits, 1);
                if(seq_scaling_list_present_flag[i]) {
                    if (i < 6)
                        xlnx_dec_parse_h264_scaling_list (16, &getbits);
                    else
                        xlnx_dec_parse_h264_scaling_list (64, &getbits);
                }
            }
        }
    } else
        sps.chroma_format_idc = 1;

    sps.log2_max_frame_num_minus4 = xlnx_dec_get_bits_unsigned_eg (&getbits);
    sps.pic_order_cnt_type = xlnx_dec_get_bits_unsigned_eg (&getbits);

    if (sps.pic_order_cnt_type == 0) {
        sps.log2_max_pic_order_cnt_lsb_minus4 = 
                                           xlnx_dec_get_bits_unsigned_eg (&getbits);
    }
    else if (sps.pic_order_cnt_type == 1)
    {
        sps.delta_pic_order_always_zero_flag = xlnx_dec_get_bits_byte (&getbits, 1);
        /* offset_for_non_ref_pic */
        xlnx_dec_get_bits_signed_eg (&getbits);
        /* offset_for_top_to_bottom_field */
        xlnx_dec_get_bits_signed_eg (&getbits);
        unsigned char num_ref_frames_in_pic_order_cnt_cycle = 
            xlnx_dec_get_bits_unsigned_eg (&getbits);
        for (i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
            xlnx_dec_get_bits_signed_eg (&getbits);
    }

    /* num_ref_frames */
    xlnx_dec_get_bits_unsigned_eg (&getbits);
    /* gaps_in_frame_num_value_allowed_flag */
    xlnx_dec_get_bits_byte (&getbits, 1);
    sps.pic_width_in_mbs_minus1 = xlnx_dec_get_bits_unsigned_eg (&getbits);
    sps.pic_height_in_map_units_minus1 = xlnx_dec_get_bits_unsigned_eg (&getbits);
    sps.frame_mbs_only_flag = xlnx_dec_get_bits_byte (&getbits, 1);

    if (!sps.frame_mbs_only_flag)
    {
        /* mb_adaptive_frame_field_flag */
        xlnx_dec_get_bits_byte (&getbits, 1);
    }

    /* direct_8x8_inference_flag */
    xlnx_dec_get_bits_byte (&getbits, 1);
    sps.frame_cropping_flag = xlnx_dec_get_bits_byte (&getbits, 1);
    if (sps.frame_cropping_flag)
    {
        sps.frame_crop_left_offset = xlnx_dec_get_bits_unsigned_eg (&getbits);
        sps.frame_crop_right_offset = xlnx_dec_get_bits_unsigned_eg (&getbits);
        sps.frame_crop_top_offset = xlnx_dec_get_bits_unsigned_eg (&getbits);
        sps.frame_crop_bottom_offset = xlnx_dec_get_bits_unsigned_eg (&getbits);
    }

    unsigned char timing_info_present_flag = 0;
    unsigned long num_units_in_tick = 0;
    unsigned long time_scale = 0;

    unsigned char vui_parameters_present_flag = xlnx_dec_get_bits_byte (&getbits, 1);
    if (vui_parameters_present_flag)
    {
        unsigned char aspect_ratio_info_present_flag = 
                                                xlnx_dec_get_bits_byte (&getbits, 1);
        if (aspect_ratio_info_present_flag)
        {
            unsigned char aspect_ratio_idc = xlnx_dec_get_bits_byte (&getbits, 8);
            if (aspect_ratio_idc == 255)
            {
                /* sar_width */
                xlnx_dec_get_bits_short (&getbits, 16);
                /* sar_height */
                xlnx_dec_get_bits_short (&getbits, 16);
            }
        }

        unsigned char overscan_info_present_flag = xlnx_dec_get_bits_byte (&getbits, 1);
        if (overscan_info_present_flag)
        {
            /* overscan_appropriate_flag */
            xlnx_dec_get_bits_byte (&getbits, 1);
        }

        unsigned char video_signal_type_present_flag = 
                                                xlnx_dec_get_bits_byte (&getbits, 1);
        if (video_signal_type_present_flag)
        {
            /* video_format */
            xlnx_dec_get_bits_byte (&getbits, 3);
            /* video_full_range_flag */
            xlnx_dec_get_bits_byte (&getbits, 1);
            unsigned char colour_description_present_flag = 
                xlnx_dec_get_bits_byte (&getbits, 1);
            if (colour_description_present_flag)
            {
                /* colour_primaries */
                xlnx_dec_get_bits_byte (&getbits, 8);
                /* transfer_characteristics */
                xlnx_dec_get_bits_byte (&getbits, 8);
                /* matrix_coefficients */
                xlnx_dec_get_bits_byte (&getbits, 8);
            }
        }

        unsigned char chroma_loc_info_present_flag = 
                                            xlnx_dec_get_bits_byte (&getbits, 1);
        if (chroma_loc_info_present_flag)
        {
            /* chroma_sample_loc_type_top_field */
            xlnx_dec_get_bits_unsigned_eg (&getbits);
            /* chroma_sample_loc_type_bottom_field */
            xlnx_dec_get_bits_unsigned_eg (&getbits);
        }

        timing_info_present_flag = xlnx_dec_get_bits_byte (&getbits, 1);
        if (timing_info_present_flag)
        {
            num_units_in_tick = xlnx_dec_get_bits_long (&getbits, 32);
            time_scale = xlnx_dec_get_bits_long (&getbits, 32);
            /* fixed_frame_rate_flag */
            xlnx_dec_get_bits_byte (&getbits, 1);
        }
    }

    if (xlnx_dec_get_bits_eof (&getbits))
    {
        free (buffer.data);
        return TRANSCODE_APP_FAILURE;
    }

    parsedata->height = ((2 - sps.frame_mbs_only_flag) * 
            (sps.pic_height_in_map_units_minus1 + 1)) * 16;
    parsedata->width = (sps.pic_width_in_mbs_minus1 + 1) * 16;
    if (sps.frame_cropping_flag)
    {
        int32_t crop_unitx;
        int32_t crop_unity;
        if (sps.chroma_format_idc == 0)
        {
            // mono
            crop_unitx = 1;
            crop_unity = 2 - sps.frame_mbs_only_flag;
        } else if (sps.chroma_format_idc == 1)
        {
            // 4:2:0
            crop_unitx = 2;
            crop_unity = 2 * (2 - sps.frame_mbs_only_flag);
        } else if (sps.chroma_format_idc == 2)
        {
            // 4:2:2
            crop_unitx = 2;
            crop_unity = 2 - sps.frame_mbs_only_flag;
        } else if (sps.chroma_format_idc == 3)
        {
            // 4:4:4
            crop_unitx = 1;
            crop_unity = 2 - sps.frame_mbs_only_flag;
        }
        parsedata->width -= crop_unitx * (sps.frame_crop_left_offset + 
                sps.frame_crop_right_offset);
        parsedata->height -= crop_unity * (sps.frame_crop_top_offset + 
                sps.frame_crop_bottom_offset);
    }

    if (timing_info_present_flag)
    {
        parsedata->fr_num = time_scale;
        parsedata->fr_den = num_units_in_tick * 2;
        unsigned long temp = xlnx_utils_gcd (parsedata->fr_num, parsedata->fr_den);
        parsedata->fr_num /= temp;
        parsedata->fr_den /= temp;
    }

    sps.valid = 1;
    parsedata->h264_seq_parameter_set[seq_parameter_set_id] = sps;

    free (buffer.data);

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_h264_pps: Parse H.264 pps
Parameters:
file: Input file
dec_buffer: Decoder input buffer
parsedata: structure for parsed data
startoffset: Current offset for the input buffer
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on error.
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_parse_h264_pps (int32_t file, XlnxDecBuffer* dec_buffer, 
        XlnxDecFrameData* parsedata, int32_t startoffset)
{
    int32_t endoffset;
    int32_t ret = xlnx_dec_find_next_start_code (file, dec_buffer, startoffset + 1, 
            &endoffset);
    if (ret == DEC_INPUT_EOF)
        endoffset = dec_buffer->size;
    if (ret == TRANSCODE_APP_FAILURE)
        return TRANSCODE_APP_FAILURE;

    XlnxDecBuffer buffer;
    ret = xlnx_dec_convert_to_rbsp (dec_buffer, startoffset, endoffset, &buffer);
    if (ret == TRANSCODE_APP_FAILURE)
        return TRANSCODE_APP_FAILURE;

    unsigned char* pt = buffer.data + 4;
    unsigned char* end = buffer.data + buffer.size;
    XlnxDecGetBits getbits;
    XlnxH264PicParamSet pps;
    memset (&pps, 0, sizeof (pps));

    xlnx_dec_init_get_bits (&getbits, pt, end);

    unsigned char pic_parameter_set_id = xlnx_dec_get_bits_unsigned_eg (&getbits);
    pps.seq_parameter_set_id = xlnx_dec_get_bits_unsigned_eg (&getbits);
    /* entropy_coding_mode_flag */
    xlnx_dec_get_bits_byte (&getbits, 1);
    pps.pic_order_present_flag = xlnx_dec_get_bits_byte (&getbits, 1);

    if (xlnx_dec_get_bits_eof (&getbits))
    {
        free (buffer.data);
        return TRANSCODE_APP_FAILURE;
    }

    pps.valid = 1;
    parsedata->h264_pic_parameter_set[pic_parameter_set_id] = pps;

    free (buffer.data);

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_h264_slice_header: Parse H.264 slice header
Parameters:
file: Input file
dec_buffer: Decoder input buffer
parsedata: structure for parsed data
sliceheader: Slice header structure
startoffset: Current offset for the input buffer
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on error, or 
DEC_INPUT_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
static int32_t xlnx_dec_parse_h264_slice_header (int32_t file, XlnxDecBuffer* dec_buffer, 
        XlnxDecFrameData* parsedata, 
        XlnxH264SliceHeader *sliceheader, 
        int32_t startoffset)
{
    int32_t endoffset;
    int32_t ret = xlnx_dec_find_next_start_code (file, dec_buffer, startoffset + 1, 
            &endoffset);
    if (ret == DEC_INPUT_EOF) {
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Got EOF in slice header %d \n", __LINE__);
        endoffset = dec_buffer->size;
    }
    if (ret == TRANSCODE_APP_FAILURE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Got Error in slice header %d \n", __LINE__);
        return TRANSCODE_APP_FAILURE;
    }

    XlnxDecBuffer buffer;
    ret = xlnx_dec_convert_to_rbsp (dec_buffer, startoffset, endoffset, &buffer);
    if (ret == TRANSCODE_APP_FAILURE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Got Error in slice header %d \n", __LINE__);
        return TRANSCODE_APP_FAILURE;
    }

    unsigned char* pt = buffer.data + 3;
    unsigned char* end = buffer.data + buffer.size;
    XlnxDecGetBits getbits;

    xlnx_dec_init_get_bits (&getbits, pt, end);

    xlnx_dec_get_bits_byte (&getbits, 1); // skip forbidden zero bit
    sliceheader->nal_ref_idc = xlnx_dec_get_bits_byte (&getbits, 2);
    sliceheader->nal_unit_type = xlnx_dec_get_bits_byte (&getbits, 5);

    /* first_mb_in_slice */
    xlnx_dec_get_bits_unsigned_eg (&getbits);
    /* slice_type */
    xlnx_dec_get_bits_unsigned_eg (&getbits);
    sliceheader->pic_parameter_set_id = xlnx_dec_get_bits_unsigned_eg (&getbits);
    parsedata->current_h264_pps = sliceheader->pic_parameter_set_id;

    XlnxH264PicParamSet pps = parsedata->h264_pic_parameter_set[
        sliceheader->pic_parameter_set_id];
    if (!pps.valid)
    {
        free (buffer.data);
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Got Error in slice header %d \n", __LINE__);
        return TRANSCODE_APP_FAILURE;
    }
    XlnxH264SeqParamSet sps = parsedata->h264_seq_parameter_set[
        pps.seq_parameter_set_id];
    if (!sps.valid)
    {
        free (buffer.data);
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Got Error in slice header %d \n", __LINE__);
        return TRANSCODE_APP_FAILURE;
    }

    sliceheader->frame_num = xlnx_dec_get_bits_short (&getbits, 
            sps.log2_max_frame_num_minus4 + 4);

    if (!sps.frame_mbs_only_flag)
    {
        sliceheader->field_pic_flag = xlnx_dec_get_bits_byte (&getbits, 1);
        if (sliceheader->field_pic_flag)
            sliceheader->bottom_field_flag = xlnx_dec_get_bits_byte (&getbits, 1);
    }

    if (sliceheader->nal_unit_type == 5)
        sliceheader->idr_pic_id = xlnx_dec_get_bits_unsigned_eg (&getbits);

    if (sps.pic_order_cnt_type == 0)
    {
        sliceheader->pic_order_cnt_lsb = xlnx_dec_get_bits_short (&getbits, 
                sps.log2_max_pic_order_cnt_lsb_minus4 + 4);

        if (pps.pic_order_present_flag && !sliceheader->field_pic_flag)
            sliceheader->delta_pic_order_cnt_bottom = 
                xlnx_dec_get_bits_signed_eg (&getbits);
    }
    if (sps.pic_order_cnt_type == 1 && !sps.delta_pic_order_always_zero_flag)
    {
        sliceheader->delta_pic_order_cnt[0] = xlnx_dec_get_bits_signed_eg (&getbits);
        if (pps.pic_order_present_flag && !sliceheader->field_pic_flag) {
            sliceheader->delta_pic_order_cnt[1] = 
                                            xlnx_dec_get_bits_signed_eg (&getbits);
        }
    }

    if (xlnx_dec_get_bits_eof (&getbits))
    {
        free (buffer.data);
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Got error in slice header parsing %d \n", __LINE__);
        return TRANSCODE_APP_FAILURE;
    }

    free (buffer.data);

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_parse_h264_au: Parse H.264 headers
Parameters:
file: Input file
buffer: Decoder input buffer
parsedata: structure for parsed data
retoffset: offset for the input buffer read
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on error, or 
DEC_INPUT_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_parse_h264_au (int32_t file, XlnxDecBuffer *dec_buffer, 
        XlnxDecFrameData *parsedata, int32_t *retoffset)
{
    int32_t startoffset = 0;
    int32_t hasslice = 0;
    int32_t ret = TRANSCODE_APP_FAILURE;

    while (true)
    {
        int32_t endoffset = 0;
        ret = xlnx_dec_find_next_start_code (file, dec_buffer, startoffset, &endoffset);
        if (ret == TRANSCODE_APP_FAILURE) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Error in finding H.264 start code \n");
            return TRANSCODE_APP_FAILURE;
        }

        if (ret == DEC_INPUT_EOF)
        {
            *retoffset = dec_buffer->size;
            xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Got EOF in decoder parser %d \n", __LINE__);
            return DEC_INPUT_EOF;
        }
        if (dec_buffer->size <= endoffset + 4)
        {
            int32_t ret = xlnx_dec_get_in_buf (file, dec_buffer, 4);
            if (ret == TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                        "Error in decoder input buffer fill \n");
                return TRANSCODE_APP_FAILURE;
            }
            if (ret == DEC_INPUT_EOF)
            {
                *retoffset = dec_buffer->size;
                xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                        "Got EOF in decoder parser %d \n", __LINE__);
                return DEC_INPUT_EOF;
            }
        }
        unsigned char nalutype = dec_buffer->data[endoffset + 3] & 0x1F;
        if (nalutype == 7) 
            ret = xlnx_dec_parse_h264_sps (file, dec_buffer, parsedata, endoffset);
        if (nalutype == 8) 
            ret = xlnx_dec_parse_h264_pps (file, dec_buffer, parsedata, endoffset);
        XlnxH264SliceHeader sliceheader;
        memset (&sliceheader, 0, sizeof (sliceheader));
        if ((1 <= nalutype) && (nalutype <= 5)) {
            ret = xlnx_dec_parse_h264_slice_header (file, dec_buffer, parsedata, 
                    &sliceheader, endoffset);
            if (ret == TRANSCODE_APP_FAILURE) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error parsing H.264 slice header \n");
                return ret;
            }
        }

        if ((hasslice == 1) && ((nalutype == 6) || (nalutype == 7) || 
                    (nalutype == 8) || (nalutype == 9) || (nalutype == 14) || 
                    (nalutype == 15) || (nalutype == 16) || (nalutype == 17) || 
                    (nalutype == 18)))
        {
            *retoffset = endoffset;
            return TRANSCODE_APP_SUCCESS;
        }
        if ((1 <= nalutype) && (nalutype <= 5))
        {
            if (!hasslice)
            {
                hasslice = 1;
                startoffset = endoffset + 1;
                parsedata->last_h264_slice_header = sliceheader;
                continue;
            }

            if (!parsedata->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].valid)
            {
                startoffset = endoffset + 1;
                continue;
            }
            if (!parsedata->h264_seq_parameter_set [
                    parsedata->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].
                    seq_parameter_set_id].valid)
            {
                startoffset = endoffset + 1;
                continue;
            }
            XlnxH264SeqParamSet sps = parsedata->h264_seq_parameter_set [
                parsedata->h264_pic_parameter_set [
                    sliceheader.pic_parameter_set_id].
                        seq_parameter_set_id];

            unsigned char isnewpic = 0;
            if (parsedata->last_h264_slice_header.frame_num != 
                    sliceheader.frame_num) 
                isnewpic = 1;
            if (parsedata->last_h264_slice_header.pic_parameter_set_id != 
                    sliceheader.pic_parameter_set_id) 
                isnewpic = 1;
            if (parsedata->last_h264_slice_header.field_pic_flag != 
                    sliceheader.field_pic_flag) 
                isnewpic = 1;
            if ((sps.frame_mbs_only_flag) && 
                    (parsedata->last_h264_slice_header.field_pic_flag) && 
                    (sliceheader.field_pic_flag) && 
                    (parsedata->last_h264_slice_header.bottom_field_flag != 
                     sliceheader.bottom_field_flag)) 
                isnewpic = 1;
            if ((parsedata->last_h264_slice_header.nal_ref_idc != 
                        sliceheader.nal_ref_idc) && 
                    ((parsedata->last_h264_slice_header.nal_ref_idc == 0) || 
                     (sliceheader.nal_ref_idc == 0))) 
                isnewpic = 1;
            if ((sps.pic_order_cnt_type == 0) && 
                ((parsedata->last_h264_slice_header.pic_order_cnt_lsb != 
                sliceheader.pic_order_cnt_lsb) || 
                (parsedata->last_h264_slice_header.delta_pic_order_cnt_bottom 
                != sliceheader.delta_pic_order_cnt_bottom))) 
                isnewpic = 1;
            if ((sps.pic_order_cnt_type == 1) && 
                ((parsedata->last_h264_slice_header.delta_pic_order_cnt[0] != 
                sliceheader.delta_pic_order_cnt[0]) || 
                (parsedata->last_h264_slice_header.delta_pic_order_cnt[1] != 
                sliceheader.delta_pic_order_cnt[1]))) 
                isnewpic = 1;
            if ((parsedata->last_h264_slice_header.nal_unit_type != 
                        sliceheader.nal_unit_type) && 
                    ((parsedata->last_h264_slice_header.nal_unit_type == 5) || 
                     (sliceheader.nal_unit_type == 5))) 
                isnewpic = 1;
            if ((parsedata->last_h264_slice_header.nal_unit_type == 5) && 
                    (sliceheader.nal_unit_type == 5) && 
                    (parsedata->last_h264_slice_header.idr_pic_id != 
                     sliceheader.idr_pic_id)) 
                isnewpic = 1;

            parsedata->last_h264_slice_header = sliceheader;
            if (isnewpic)
            {
                *retoffset = endoffset;
                return TRANSCODE_APP_SUCCESS;
            }
        }
        startoffset = endoffset + 1;
    }
    return ret;
}
