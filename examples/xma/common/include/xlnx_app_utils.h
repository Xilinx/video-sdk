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
#ifndef _XLNX_APP_UTILS_H_
#define _XLNX_APP_UTILS_H_
#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <xma.h>


/*
Let xrm decide the device id if it is set to -1.
*/
#define DEFAULT_DEVICE_ID    0
#define XMA_PROPS_TO_JSON_SO "/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so"
#define XCLBIN_PARAM_NAME    "/opt/xilinx/xcdr/xclbins/transcode.xclbin"
#define DYN_PARAMS_LIB_NAME  "/opt/xilinx/xma_apps/libu30_enc_dyn_param.so"

#define XLNX_XMA_APP_MODULE "xlnx_xma_app"
#define XMA_APP_SUCCESS     XMA_SUCCESS
#define XMA_APP_ERROR       XMA_ERROR
#define XMA_APP_EOS         XMA_EOS
#define XMA_APP_EOF         XMA_END_OF_FILE

typedef enum {
    BUFFER_ALLOC_ALIGN    = 4096,
    DEC_OUT_STRIDE_ALIGN  = 256,
    DEC_OUT_HEIGHT_ALIGN  = 64,
    SCAL_OUT_STRIDE_ALIGN = 32,
    SCAL_OUT_HEIGHT_ALIGN = 32,

    SCAL_IN_STRIDE_ALIGN = DEC_OUT_STRIDE_ALIGN,
    SCAL_IN_HEIGHT_ALIGN = DEC_OUT_HEIGHT_ALIGN,

    ENC_IN_STRIDE_ALIGN = SCAL_OUT_STRIDE_ALIGN,
    ENC_IN_HEIGHT_ALIGN = SCAL_OUT_HEIGHT_ALIGN
} XlnxAppAlign;

#define XLNX_ALIGN(x, align) (((x) + (align)-1) & ~((align)-1))

typedef enum {
    XMA_APP_NONE_FMT_TYPE = 0,
    XMA_APP_YUV420P_FMT_TYPE,
    XMA_APP_NV12_FMT_TYPE,
    XMA_APP_YUV420P10LE_FMT_TYPE, // No xma format for this exists
    XMA_APP_XV15_FMT_TYPE         // aka nv12_10le32
} XlnxFormatType;

typedef enum { BITS_PER_PIXEL_8 = 8, BITS_PER_PIXEL_10 = 10 } XlnxBitsPerPixels;

#define xlnx_utils_get_xma_vcu_format(bit_depth)                               \
    (bit_depth == BITS_PER_PIXEL_10 ? XMA_VCU_NV12_10LE32_FMT_TYPE :           \
                                      XMA_VCU_NV12_FMT_TYPE)
/* 10 bit: for every 3 pixels there are 4 bytes of data width 2 empty bits. */
#define xlnx_utils_get_valid_bytes_in_line(bpp, width)                         \
    (bpp == BITS_PER_PIXEL_10 ? ((width + 2) / 3) * 4 : width)
#define xlnx_utils_get_bit_depth_app_fmt(xma_app_fmt)                          \
    (xma_app_fmt == XMA_APP_NV12_FMT_TYPE ||                                   \
             xma_app_fmt == XMA_APP_YUV420P_FMT_TYPE ?                         \
         BITS_PER_PIXEL_8 :                                                    \
         BITS_PER_PIXEL_10)

#define UNASSIGNED             INT32_MIN
#define replace_if_unset(a, b) ((a == UNASSIGNED) ? (b) : (a))

#define H265_CODEC_TYPE 1
#define HEVC_CODEC_TYPE H265_CODEC_TYPE
#define H265_CODEC_NAME "mpsoc_vcu_hevc"
#define HEVC_CODEC_NAME H265_CODEC_NAME

#define H264_CODEC_TYPE 0
#define AVC_CODEC_TYPE  H264_CODEC_TYPE
#define H264_CODEC_NAME "mpsoc_vcu_h264"
#define AVC_CODEC_NAME  H264_CODEC_NAME

#define DEBUG_LOGLEVEL        0
#define XLNX_APP_UTILS_MODULE "xlnx_app_utils"

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * xlnx_utils_was_q_pressed: returns true if q has been pressed
 *
 * @return true if q has been pressed, otherwise false
 */
bool xlnx_utils_was_q_pressed(void);

/**
 * xlnx_utils_set_non_blocking: sets or releases blocking when checking if a key
 * has been pressed
 * @param state: if 0, sets blocking; all other values releases blocking
 */
void xlnx_utils_set_non_blocking(int state);

/**
 * xlnx_utils_gcd: returns the greatest common denomimator of two values
 * @param a: first value
 * @param b: second value
 * @return the greatest common denomimator of a and b
 */
uint32_t xlnx_utils_gcd(uint32_t a, uint32_t b);

/* Argument Helper Functions */

/**
 * xlnx_utils_check_if_pattern_matches: Checks if str matches the pattern
 * @param pattern: The parameter which will compile into a regex
 * @param str: The source to check
 * @return True if it matches False if it doesn't Exits if error.
 */
bool xlnx_utils_check_if_pattern_matches(char* pattern, char* str);

/**
 * xlnx_utils_set_size_t_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * @return XMA_APP_ERROR on failure XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_size_t_arg(size_t* destination, char* source,
                              char* param_name);

/**
 * xlnx_utils_set_uint_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * @return XMA_APP_ERROR on failure XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_uint_arg(uint* destination, char* source, char* param_name);

/**
 * xlnx_utils_set_int_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * @return XMA_APP_ERROR on failure XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_int_arg(int* destination, char* source, char* param_name);

/**
 * Get the vcu format for the given xma app format.
 * @param app_fmt The app format source
 * @return The corresponding xma vcu format
 */
int xlnx_utils_app_fmt_to_vcu_fmt(XlnxFormatType app_fmt);

/**
 * Set the destination to the appropriate xma format type given the source
 * format.
 * @param destination The format type equivalent to the source
 * @param needs_convert Set if the format needs to be converted. XMA headers
 * don't have an XMA_YUV420P10LE_FMT_TYPE defined so this is necessary.
 * @param source The source pixel format
 * @param param_name The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 */
int xlnx_utils_set_pix_fmt_arg(XlnxFormatType* destination, char* source,
                               char* param_name);

/**
 * Compare two arguments and return the result.
 * @param arg1 First argument for comparison
 * @param arg2 Second argument for comparison
 * @return Comparison result of 2 arguments
 */
int xlnx_utils_compare(const void* arg1, const void* arg2);

/* Timer Functions/structs */

typedef struct XlnxAppTimeTracker {
    struct timespec start_time;
    struct timespec curr_time;
    struct timespec segment_time;
    int             last_displayed_frame;
} XlnxAppTimeTracker;

/**
 * xlnx_utils_set_segment_time: Used to track how long it has been since the
 * previous
 * @param timer: The timer struct which stores the necessary timespecs.
 */
void xlnx_utils_set_segment_time(XlnxAppTimeTracker* timer);

/**
 * xlnx_utils_start_tracking_time: Get the curr time stamp
 * @param timer: The timer struct which stores the necessary timespecs.
 */
void xlnx_utils_start_tracking_time(XlnxAppTimeTracker* timer);

/**
 * xlnx_utils_get_total_time: Calculate the real time taken
 * @param timer: The timer struct which stores the necessary timespecs.
 */
double xlnx_utils_get_total_time(XlnxAppTimeTracker* timer);

/**
 * xlnx_utils_get_segment_time: Calculate the time taken since the
 * last segment
 * @param timer: The timer struct which stores the necessary timespecs.
 * @return The time since the last segement
 */
double xlnx_utils_get_segment_time(XlnxAppTimeTracker* timer);

/* Pixel Format Conversion Functions */

/**
 * @param width width of the frame
 * @param height height of the frame
 * @param xv15_linesize output linesize for the xv15 buffer
 * @param xv15_y xv15 y plane buffer to be filled
 * @param xv15_uv xv15 uv plane buffer to be filled
 * @param out_f The opened output file to which output should be written
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
int xlnx_utils_xv15_to_yuv420p10le(uint16_t width, uint16_t height,
                                   uint32_t xv15_linesize, uint32_t* xv15_y,
                                   uint32_t* xv15_uv, FILE* out_f);

/**
 * Convert the nv12 buffers into yuv420p output
 * @param width width of the frame
 * @param height height of the frame
 * @param nv12_linesize linesize of the nv12 frame
 * @param nv12_y nv12 y plane buffer containing data
 * @param nv12_uv nv12 uv plane buffer containing data
 * @param out_f The opened output file to which output should be written
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
int xlnx_utils_nv12_to_yuv420p(uint16_t width, uint16_t height,
                               uint16_t nv12_linesize, uint8_t* nv12_y,
                               uint8_t* nv12_uv, FILE* out_f);

/**
 * Remove the padding from the vcu buffer received vcu buffer
 * @param valid_bytes_in_line The number of valid bytes in each line of the vcu
 * buffer
 * @param total_bytes_in_line the total number of bytes in each line of the vcu
 * buffer
 * @param height The height of the output frame
 * @param src_y The padded vcu y buffer
 * @param src_uv The padded vcu uv buffer
 * @param out_f The output file to write to (NULL if not applicable)
 */
int xlnx_utils_remove_vcu_padding(uint16_t valid_bytes_in_line,
                                  uint16_t total_bytes_in_line, uint16_t height,
                                  void* src_y, void* src_uv, FILE* out_f);

/**
 * Read the next frame into the out buffers. Does conversion if necessary.
 * @param width The width of the input frame
 * @param height The height of the input frame
 * @param fmt The format the input is in
 * @param in_fp Opened input file
 * @param out_y Pointer to the output y buffer
 * @param out_uv Pointer to the output uv buffer
 * @return XMA_APP_SUCCESS on success, XMA_APP_EOF on eof.
 */
int32_t xlnx_utils_read_frame(uint16_t width, uint16_t height,
                              XlnxFormatType fmt, FILE* in_fp, void** out_y,
                              void** out_uv);

#endif // _XLNX_APP_UTILS_H_
