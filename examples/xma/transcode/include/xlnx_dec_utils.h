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

#ifndef _XLNX_DEC_UTILS_H_
#define _XLNX_DEC_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

#include <xma.h>
#include "xlnx_transcoder_constants.h"

typedef struct XlnxDecBuffer
{
    unsigned char* data;
    uint32_t size;
    uint32_t allocated;
}XlnxDecBuffer;

typedef struct XlnxDecGetBits
{
    const unsigned char* start;
    const unsigned char* end;
    unsigned int offset_bytes;
    unsigned char offset_bits;
}XlnxDecGetBits;

unsigned long min (unsigned long a, unsigned long b);
unsigned long gcd (unsigned long a, unsigned long b);

void xlnx_dec_init_get_bits (XlnxDecGetBits* getbits, const unsigned char* start, 
        const unsigned char* end);

int xlnx_dec_get_bits_eof (XlnxDecGetBits* getbits);
unsigned char xlnx_dec_get_bits_byte (XlnxDecGetBits* getbits, unsigned char bits);
unsigned short xlnx_dec_get_bits_short (XlnxDecGetBits* getbits, unsigned char bits);
unsigned long xlnx_dec_get_bits_long (XlnxDecGetBits* getbits, unsigned char bits);
unsigned long xlnx_dec_get_bits_unsigned_eg (XlnxDecGetBits* getbits);
signed long xlnx_dec_get_bits_signed_eg (XlnxDecGetBits* getbits);

int32_t xlnx_dec_get_in_buf (int32_t file, XlnxDecBuffer *buffer, 
        const uint32_t min_size);

int xlnx_dec_find_next_start_code (int file, XlnxDecBuffer* buffer, int offset, 
        int* retoffset);

int xlnx_dec_convert_to_rbsp (XlnxDecBuffer* buffer, int startoffset, int endoffset, 
        XlnxDecBuffer* newbuffer);

#endif //_XLNX_DEC_UTILS_H_
