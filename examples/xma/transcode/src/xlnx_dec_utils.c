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

#include "xlnx_dec_utils.h"

/*-----------------------------------------------------------------------------
xlnx_dec_init_get_bits: initializes the GetBits structure
Parameters:
getbits: structure to initialize
start: start of data to read from
end: end of data to read from
-----------------------------------------------------------------------------*/
void xlnx_dec_init_get_bits (XlnxDecGetBits* getbits, const unsigned char* start, 
        const unsigned char* end)
{
    getbits->start = start;
    getbits->end = end;
    getbits->offset_bytes = 0;
    getbits->offset_bits = 0;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_eof: returns 1 if the end has been reached, otherwise returns 0
Parameters:
getbits: structure to check
Return:
1 if the end has been reached, otherwise 0
-----------------------------------------------------------------------------*/
int xlnx_dec_get_bits_eof (XlnxDecGetBits* getbits)
{
    if (getbits->start + getbits->offset_bytes >= getbits->end)
        return 1;
    return 0;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_byte: reads up to 8 bits from the GetBits structure
Parameters:
getbits: structure to read from
bits: number of bits to read
Return:
bits read
-----------------------------------------------------------------------------*/
unsigned char xlnx_dec_get_bits_byte (XlnxDecGetBits* getbits, unsigned char bits)
{
    unsigned char ret = 0;
    while (bits > 0)
    {
        if (xlnx_dec_get_bits_eof (getbits))
            return ret << bits;
        while (getbits->offset_bits < 8)
        {
            ret = (ret << 1) | ((getbits->start[getbits->offset_bytes] >> 
                        (7 - getbits->offset_bits)) & 0x01);
            getbits->offset_bits++;
            bits--;
            if (bits == 0)
            {
                if (getbits->offset_bits == 8)
                {
                    getbits->offset_bytes++;
                    getbits->offset_bits = 0;
                }
                return ret;
            }
        }
        getbits->offset_bytes++;
        getbits->offset_bits = 0;
    }
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_short: reads up to 16 bits from the GetBits structure
Parameters:
getbits: structure to read from
bits: number of bits to read
Return:
bits read
-----------------------------------------------------------------------------*/
unsigned short xlnx_dec_get_bits_short (XlnxDecGetBits* getbits, unsigned char bits)
{
    unsigned short ret = 0;
    while (bits > 0)
    {
        if (xlnx_dec_get_bits_eof (getbits))
            return ret << bits;
        while (getbits->offset_bits < 8)
        {
            ret = (ret << 1) | ((getbits->start[getbits->offset_bytes] >> 
                        (7 - getbits->offset_bits)) & 0x01);
            getbits->offset_bits++;
            bits--;
            if (bits == 0)
            {
                if (getbits->offset_bits == 8)
                {
                    getbits->offset_bytes++;
                    getbits->offset_bits = 0;
                }
                return ret;
            }
        }
        getbits->offset_bytes++;
        getbits->offset_bits = 0;
    }
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_long: reads up to 32 bits from the GetBits structure
Parameters:
getbits: structure to read from
bits: number of bits to read
Return:
bits read
-----------------------------------------------------------------------------*/
unsigned long xlnx_dec_get_bits_long (XlnxDecGetBits* getbits, unsigned char bits)
{
    unsigned long ret = 0;
    while (bits > 0)
    {
        if (xlnx_dec_get_bits_eof (getbits))
            return ret << bits;
        while (getbits->offset_bits < 8)
        {
            ret = (ret << 1) | ((getbits->start[getbits->offset_bytes] >> 
                        (7 - getbits->offset_bits)) & 0x01);
            getbits->offset_bits++;
            bits--;
            if (bits == 0)
            {
                if (getbits->offset_bits == 8)
                {
                    getbits->offset_bytes++;
                    getbits->offset_bits = 0;
                }
                return ret;
            }
        }
        getbits->offset_bytes++;
        getbits->offset_bits = 0;
    }
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_unsigned_eg: reads unsigned Exp-Golomb code from GetBits structure
Parameters:
getbits: structure to read from
Return:
value read
-----------------------------------------------------------------------------*/
unsigned long xlnx_dec_get_bits_unsigned_eg (XlnxDecGetBits* getbits)
{
    int leadingzerobits = -1;
    unsigned char b = 0;
    while (!b)
    {
        b = xlnx_dec_get_bits_byte(getbits, 1);
        leadingzerobits++;
        if (xlnx_dec_get_bits_eof (getbits))
            return 0;
    }
    if (leadingzerobits == 0)
        return 0;
    unsigned long result = 1;
    int i;
    for (i = 0; i < leadingzerobits; i++)
        result *= 2;
    unsigned long remain = 0;
    while (leadingzerobits != 0)
    {
        if (leadingzerobits > 8)
        {
            remain = (remain << 8) + xlnx_dec_get_bits_byte (getbits, 8);
            leadingzerobits -= 8;
        } else {
            remain = (remain << leadingzerobits) + 
                xlnx_dec_get_bits_byte(getbits, leadingzerobits);
            leadingzerobits = 0;
        }
    }

    return (result - 1 + remain);
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_bits_signed_eg: reads signed Exp-Golomb code from GetBits structure
Parameters:
getbits: structure to read from
Return:
value read
-----------------------------------------------------------------------------*/
signed long xlnx_dec_get_bits_signed_eg (XlnxDecGetBits* getbits)
{
    signed long codenum = xlnx_dec_get_bits_unsigned_eg (getbits);
    if (codenum % 2 == 0)
        return ((codenum / 2) * -1);
    else
        return ((codenum + 1) / 2);
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_in_buf: reads at least minincreasesize bytes from input file and 
fills the Buffer with that data
Parameters:
file: file to read from
buffer: buffer to put data into
minincreasesize: minimum amount to read
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on memory 
allocation error, or DEC_INPUT_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
int32_t xlnx_dec_get_in_buf (int32_t file, XlnxDecBuffer *buffer, 
        const uint32_t min_size)
{
    if ((buffer->size + min_size) > buffer->allocated)
    {
        unsigned char* newdata = malloc (buffer->size + min_size);
        if (!newdata)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Malloc error of size %d.\n", buffer->size + min_size);
            return TRANSCODE_APP_FAILURE;
        }
        if (buffer->size > 0)
            memcpy (newdata, buffer->data, buffer->size);
        if (buffer->data)
            free (buffer->data);
        buffer->data = newdata;
        buffer->allocated = buffer->size + min_size;
    }
    do
    {
        int32_t dataread = read (file, (buffer->data + buffer->size), 
                (buffer->allocated - buffer->size));
        buffer->size += dataread;
        if (dataread == 0)
            return DEC_INPUT_EOF;
    } while (buffer->allocated - buffer->size > 0);

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_dec_find_next_start_code: searches for the next start code ("0x000001") and reads 
more data from the file into the buffer if necessary to
find it.
Parameters:
file: file to read from if more data is needed
buffer: buffer containing data to search
offset: location in buffer to start searching from
retoffset: returns the location of the start of the next start code
Return:
TRANSCODE_APP_SUCCESS on success, TRANSCODE_APP_FAILURE on memory 
allocation error, or DEC_INPUT_EOF if the end of the input file is reached
-----------------------------------------------------------------------------*/
int xlnx_dec_find_next_start_code (int file, XlnxDecBuffer* buffer, int offset, 
        int* retoffset)
{
    unsigned char* pt = buffer->data + offset;
    unsigned char* end = buffer->data + buffer->size - 3;
    while (true)
    {
        if (pt > end)
        {
            int ret = xlnx_dec_get_in_buf (file, buffer, 4096);
            if (ret == TRANSCODE_APP_FAILURE)
                return TRANSCODE_APP_FAILURE;
            if (ret == DEC_INPUT_EOF)
                return DEC_INPUT_EOF;
            pt = buffer->data + offset;
            end = buffer->data + buffer->size - 3;
        }
        if ((pt[0] == 0x00) && (pt[1] == 0x00) && (pt[2] == 0x01))
        {
            *retoffset = pt - buffer->data;
            return TRANSCODE_APP_SUCCESS;
        }
        pt++;
    }
}

/*-----------------------------------------------------------------------------
xlnx_dec_convert_to_rbsp: convert escaped bit stream payload to raw bit-stream payload
Parameters:
buffer: buffer containing escaped bit-stream payload
startoffset: starting offset in buffer to convert
endoffset: ending offset in buffer to convert
newbuffer: buffer to place raw bit-stream payload in
Return:
TRANSCODE_APP_SUCCESS on success or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int xlnx_dec_convert_to_rbsp (XlnxDecBuffer* buffer, int startoffset, int endoffset, 
        XlnxDecBuffer* newbuffer)
{
    newbuffer->allocated = endoffset - startoffset;
    newbuffer->data = malloc (newbuffer->allocated);
    if (!newbuffer->data)
        return TRANSCODE_APP_FAILURE;
    newbuffer->size = 0;
    int state = 0;
    unsigned char* pt = buffer->data + startoffset;
    unsigned char* end = buffer->data + endoffset;
    unsigned char* dst = newbuffer->data;
    while (pt != end)
    {
        if (state == 0 && *pt == 0)
            state = 1;
        else if (state == 1 && *pt == 0)
            state = 2;
        else if (state == 2 && *pt == 3)
        {
            state = 3;
            ++pt;
            continue;
        } else if (state == 3 && *pt == 0)
            state = 1;
        else if (state == 3 && ((*pt == 1) || (*pt == 2) || (*pt == 3)))
            state = 0;
        else if (state == 3)
        {
            *dst++ = 3;
            ++newbuffer->size;
            state = 0;
        }
        else
            state = 0;
        *dst++ = *pt++;
        ++newbuffer->size;
    }
    return TRANSCODE_APP_SUCCESS;
}
