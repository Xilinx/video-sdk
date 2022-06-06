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

#ifndef _XLNX_DEC_UTILS_H_
#define _XLNX_DEC_UTILS_H_

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "xlnx_app_utils.h"
#include "xlnx_transcoder_constants.h"
#include <xma.h>


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

void xlnx_dec_init_get_bits(XlnxDecGetBits* getbits, const uint8_t* start,
                            const uint8_t* end);

int         xlnx_dec_get_bits_eof(XlnxDecGetBits* getbits);
uint8_t     xlnx_dec_get_bits_byte(XlnxDecGetBits* getbits, uint8_t bits);
uint16_t    xlnx_dec_get_bits_short(XlnxDecGetBits* getbits, uint8_t bits);
uint32_t    xlnx_dec_get_bits_long(XlnxDecGetBits* getbits, uint8_t bits);
uint32_t    xlnx_dec_get_bits_unsigned_eg(XlnxDecGetBits* getbits);
signed long xlnx_dec_get_bits_signed_eg(XlnxDecGetBits* getbits);

int32_t xlnx_dec_get_in_buf(int32_t file, XlnxDecBuffer* buffer,
                            const uint32_t min_size);

int xlnx_dec_find_next_start_code(int file, XlnxDecBuffer* buffer, int offset,
                                  int* retoffset);

int xlnx_dec_convert_to_rbsp(XlnxDecBuffer* buffer, int startoffset,
                             int endoffset, XlnxDecBuffer* newbuffer);

#endif //_XLNX_DEC_UTILS_H_
