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

#include "xlnx_dec_common_parser.h"

/**
 * xlnx_dec_init_get_bits: initializes the GetBits structure
 * @param get_bits: structure to initialize
 * @param start: start of data to read from
 * @param end: end of data to read from
 */
void xlnx_dec_init_get_bits(XlnxDecGetBits* get_bits, const uint8_t* start,
                            const uint8_t* end)
{
    get_bits->start        = start;
    get_bits->end          = end;
    get_bits->offset_bytes = 0;
    get_bits->offset_bits  = 0;
}

/**
 * xlnx_dec_get_bits_eof: returns 1 if the end has been reached, otherwise
 * returns
 * 0
 * @param get_bits: structure to check
 * @return 1 if the end has been reached, otherwise 0
 */
int xlnx_dec_get_bits_eof(XlnxDecGetBits* get_bits)
{
    if(get_bits->start + get_bits->offset_bytes >= get_bits->end)
        return 1;
    return 0;
}

/**
 * xlnx_dec_get_bits_byte: reads up to 8 bits from the GetBits structure
 * @param get_bits: structure to read from
 * @param _bits: number of bits to read
 * @return bits read
 */
uint8_t xlnx_dec_get_bits_byte(XlnxDecGetBits* get_bits, uint8_t bits)
{
    uint8_t ret = 0;
    while(bits > 0) {
        if(xlnx_dec_get_bits_eof(get_bits))
            return ret << bits;
        while(get_bits->offset_bits < 8) {
            ret = (ret << 1) | ((get_bits->start[get_bits->offset_bytes] >>
                                 (7 - get_bits->offset_bits)) &
                                0x01);
            get_bits->offset_bits++;
            bits--;
            if(bits == 0) {
                if(get_bits->offset_bits == 8) {
                    get_bits->offset_bytes++;
                    get_bits->offset_bits = 0;
                }
                return ret;
            }
        }
        get_bits->offset_bytes++;
        get_bits->offset_bits = 0;
    }
    return ret;
}

/**
 * xlnx_dec_get_bits_short: reads up to 16 bits from the GetBits structure
 * @param get_bits: structure to read from
 * @param _bits: number of bits to read
 * @return bits read
 */
uint16_t xlnx_dec_get_bits_short(XlnxDecGetBits* get_bits, uint8_t bits)
{
    uint16_t ret = 0;
    while(bits > 0) {
        if(xlnx_dec_get_bits_eof(get_bits))
            return ret << bits;
        while(get_bits->offset_bits < 8) {
            ret = (ret << 1) | ((get_bits->start[get_bits->offset_bytes] >>
                                 (7 - get_bits->offset_bits)) &
                                0x01);
            get_bits->offset_bits++;
            bits--;
            if(bits == 0) {
                if(get_bits->offset_bits == 8) {
                    get_bits->offset_bytes++;
                    get_bits->offset_bits = 0;
                }
                return ret;
            }
        }
        get_bits->offset_bytes++;
        get_bits->offset_bits = 0;
    }
    return ret;
}

/**
 * xlnx_dec_get_bits_long: reads up to 32 bits from the GetBits structure
 * @param get_bits: structure to read from
 * @param _bits: number of bits to read
 * @return bits read
 */
uint32_t xlnx_dec_get_bits_long(XlnxDecGetBits* get_bits, uint8_t bits)
{
    uint32_t ret = 0;
    while(bits > 0) {
        if(xlnx_dec_get_bits_eof(get_bits))
            return ret << bits;
        while(get_bits->offset_bits < 8) {
            ret = (ret << 1) | ((get_bits->start[get_bits->offset_bytes] >>
                                 (7 - get_bits->offset_bits)) &
                                0x01);
            get_bits->offset_bits++;
            bits--;
            if(bits == 0) {
                if(get_bits->offset_bits == 8) {
                    get_bits->offset_bytes++;
                    get_bits->offset_bits = 0;
                }
                return ret;
            }
        }
        get_bits->offset_bytes++;
        get_bits->offset_bits = 0;
    }
    return ret;
}

/**
 * xlnx_dec_get_bits_unsigned_eg: reads unsigned Exp-Golomb code from GetBits
 * structure
 * @param get_bits: structure to read from
 * @return value read
 */
uint32_t xlnx_dec_get_bits_unsigned_eg(XlnxDecGetBits* get_bits)
{
    int     leading_zero_bits = -1;
    uint8_t b                 = 0;
    while(!b) {
        b = xlnx_dec_get_bits_byte(get_bits, 1);
        leading_zero_bits++;
        if(xlnx_dec_get_bits_eof(get_bits))
            return 0;
    }
    if(leading_zero_bits == 0)
        return 0;
    uint32_t result = 1;
    int      i;
    for(i = 0; i < leading_zero_bits; i++)
        result *= 2;
    uint32_t remain = 0;
    while(leading_zero_bits != 0) {
        if(leading_zero_bits > 8) {
            remain = (remain << 8) + xlnx_dec_get_bits_byte(get_bits, 8);
            leading_zero_bits -= 8;
        } else {
            remain = (remain << leading_zero_bits) +
                     xlnx_dec_get_bits_byte(get_bits, leading_zero_bits);
            leading_zero_bits = 0;
        }
    }

    return (result - 1 + remain);
}

/**
 * xlnx_dec_get_bits_signed_eg: reads signed Exp-Golomb code from GetBits
 * structure
 * @param get_bits: structure to read from
 * @return value read
 */
signed long xlnx_dec_get_bits_signed_eg(XlnxDecGetBits* get_bits)
{
    signed long code_num = xlnx_dec_get_bits_unsigned_eg(get_bits);
    if(code_num % 2 == 0) {
        return ((code_num / 2) * -1);
    } else {
        return ((code_num + 1) / 2);
    }
}

/**
 * xlnx_dec_get_in_buf: reads at least min_size bytes from input file and
 * fills the Buffer with that data
 * @param file: file to read from
 * @param buffer: buffer to put data into
 * @param min_size: minimum amount to read
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on memory
 * allocation error, or XMA_APP_EOF if the end of the input file is reached
 */
int32_t xlnx_dec_get_in_buf(int32_t file, XlnxDecBuffer* buffer,
                            const uint32_t min_size)
{
    if((buffer->size + min_size) > buffer->allocated) {
        void* newdata = NULL;
        if(posix_memalign(&newdata, BUFFER_ALLOC_ALIGN,
                          buffer->size + min_size) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                       "Unable to allocate input data of size %d.\n",
                       buffer->size + min_size);
            return XMA_APP_ERROR;
        }
        if(buffer->size > 0)
            memcpy(newdata, buffer->data, buffer->size);
        if(buffer->data)
            free(buffer->data);
        buffer->data      = newdata;
        buffer->allocated = buffer->size + min_size;
    }
    do {
        int32_t dataread = read(file, (buffer->data + buffer->size),
                                (buffer->allocated - buffer->size));
        buffer->size += dataread;
        if(dataread == 0)
            return XMA_APP_EOF;
    } while(buffer->allocated - buffer->size > 0);

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_dec_find_next_start_code: searches for the next start code ("0x000001")
 * and reads more data from the file into the buffer if necessary to find it.
 * @param file: file to read from if more data is needed
 * @param buffer: buffer containing data to search
 * @param offset: location in buffer to start searching from
 * @param ret_offset: returns the location of the start of the next start code
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on memory
 * allocation error, or XMA_APP_EOF if the end of the input file is reached
 */
int xlnx_dec_find_next_start_code(int file, XlnxDecBuffer* buffer, int offset,
                                  int* ret_offset)
{
    uint8_t* pt  = buffer->data + offset;
    uint8_t* end = buffer->data + buffer->size - 3;
    while(true) {
        if(pt > end) {
            int ret = xlnx_dec_get_in_buf(file, buffer, 4096);
            if(ret == XMA_APP_ERROR)
                return XMA_APP_ERROR;
            if(ret == XMA_APP_EOF)
                return XMA_APP_EOF;
            pt  = buffer->data + offset;
            end = buffer->data + buffer->size - 3;
        }
        if((pt[0] == 0x00) && (pt[1] == 0x00) && (pt[2] == 0x01)) {
            *ret_offset = pt - buffer->data;
            return XMA_APP_SUCCESS;
        }
        pt++;
    }
}

/**
 * xlnx_dec_convert_to_rbsp: convert escaped bit stream payload to raw
 * bit-stream payload
 * @param buffer: buffer containing escaped bit-stream payload
 * @param start_offset: starting offset in buffer to convert
 * @param end_offset: ending offset in buffer to convert
 * @param new_buffer: buffer to place raw bit-stream payload in
 * @return XMA_APP_SUCCESS on success or XMA_APP_ERROR
 */
int xlnx_dec_convert_to_rbsp(XlnxDecBuffer* buffer, int start_offset,
                             int end_offset, XlnxDecBuffer* new_buffer)
{
    new_buffer->allocated = end_offset - start_offset;
    if(posix_memalign((void**)&new_buffer->data, BUFFER_ALLOC_ALIGN,
                      new_buffer->allocated) != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DEC_APP_MODULE,
                   "Unable to allocate input data.\n");
        return XMA_APP_ERROR;
    }
    new_buffer->size = 0;
    int      state   = 0;
    uint8_t* pt      = buffer->data + start_offset;
    uint8_t* end     = buffer->data + end_offset;
    uint8_t* dst     = new_buffer->data;
    while(pt != end) {
        if(state == 0 && *pt == 0)
            state = 1;
        else if(state == 1 && *pt == 0)
            state = 2;
        else if(state == 2 && *pt == 3) {
            state = 3;
            ++pt;
            continue;
        } else if(state == 3 && *pt == 0)
            state = 1;
        else if(state == 3 && ((*pt == 1) || (*pt == 2) || (*pt == 3)))
            state = 0;
        else if(state == 3) {
            *dst++ = 3;
            ++new_buffer->size;
            state = 0;
        } else {
            state = 0;
        }
        *dst++ = *pt++;
        ++new_buffer->size;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Set the initial values for the parse_data
 * @param parse_data A pointer to the XlnxDecFrameParseData whose values will
 * set
 * @return XMA_APP_SUCCESS
 */
int xlnx_dec_init_parse_data(XlnxDecFrameParseData* parse_data)
{
    memset(parse_data, 0, sizeof(XlnxDecFrameParseData));

    int i;
    for(i = 0; i < 32; i++)
        parse_data->h264_seq_parameter_set[i].valid = 0;
    for(i = 0; i < 256; i++)
        parse_data->h264_pic_parameter_set[i].valid = 0;
    parse_data->last_h264_slice_header.delta_pic_order_cnt_bottom = -1;
    parse_data->last_h264_slice_header.delta_pic_order_cnt[0]     = -1;
    parse_data->last_h264_slice_header.delta_pic_order_cnt[1]     = -1;
    parse_data->last_h264_slice_header.frame_num                  = 0;
    parse_data->last_h264_slice_header.idr_pic_id                 = 0;
    parse_data->last_h264_slice_header.pic_order_cnt_lsb          = 0;
    parse_data->last_h264_slice_header.pic_parameter_set_id       = 0;
    parse_data->last_h264_slice_header.field_pic_flag             = 0;
    parse_data->last_h264_slice_header.bottom_field_flag          = 0;
    parse_data->last_h264_slice_header.nal_ref_idc                = 0;
    parse_data->last_h264_slice_header.nal_unit_type              = 0;

    return XMA_APP_SUCCESS;
}
