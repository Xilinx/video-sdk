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
#ifndef _INCLUDED_XLNX_APP_UTILS_H_
#define _INCLUDED_XLNX_APP_UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <termios.h>
#include <time.h>
#include <regex.h>
#include <errno.h>
#include <xma.h>

/* 
Let xrm decide the device id if it is set to -1.
*/
#define DEFAULT_DEVICE_ID    -1
#define XMA_PROPS_TO_JSON_SO "/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so"
#define XCLBIN_PARAM_NAME    "/opt/xilinx/xcdr/xclbins/transcode.xclbin"

#define RET_ERROR     XMA_ERROR
#define RET_SUCCESS   XMA_SUCCESS
#define RET_EOF       XMA_SUCCESS + 1
#define RET_EOS       RET_EOF + 1

#define STRIDE_ALIGN  256
#define HEIGHT_ALIGN  64
#define ALIGN(x,align) (((x) + (align) - 1) & ~((align) - 1))

#define UNASSIGNED               INT32_MIN
#define replace_if_unset(a,b)    ((a == UNASSIGNED) ? (b) : (a))

#define H265_CODEC_TYPE 1
#define HEVC_CODEC_TYPE H265_CODEC_TYPE
#define H265_CODEC_NAME "mpsoc_vcu_hevc"
#define HEVC_CODEC_NAME H265_CODEC_NAME

#define H264_CODEC_TYPE 0
#define AVC_CODEC_TYPE  H264_CODEC_TYPE
#define H264_CODEC_NAME "mpsoc_vcu_h264"
#define AVC_CODEC_NAME  H264_CODEC_NAME

#define DEBUG_LOGLEVEL           0
#define XLNX_APP_UTILS_MODULE    "xlnx_app_utils"
#define XLNX_APP_UTILS_LOG_ERROR(msg...) \
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE, msg)
#define XLNX_APP_UTILS_LOG_INFO(msg...) \
            xma_logmsg(XMA_INFO_LOG, XLNX_APP_UTILS_MODULE, msg)

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

/*------------------------------------------------------------------------------
xlnx_utils_was_q_pressed: returns true if q has been pressed
Return:
    true if q has been pressed, otherwise false
------------------------------------------------------------------------------*/
bool xlnx_utils_was_q_pressed(void);

/*------------------------------------------------------------------------------
xlnx_utils_set_non_blocking: sets or releases blocking when checking if a key 
    has been pressed
Parameters:
    state: if 0, sets blocking; all other values releases blocking
------------------------------------------------------------------------------*/
void xlnx_utils_set_non_blocking(int state);

/*------------------------------------------------------------------------------
xlnx_utils_gcd: returns the greatest common denomimator of two values
paramters:
    a: first value
    b: second value
Return:
    the greatest common denomimator of a and b
------------------------------------------------------------------------------*/
unsigned long xlnx_utils_gcd(unsigned long a, unsigned long b);

/* Argument Helper Functions */

/* -----------------------------------------------------------------------------
xlnx_utils_check_if_pattern_matches: Checks if str matches the pattern
Parameters:
    pattern: The parameter which will compile into a regex
    str: The source to check
Return:
    True if it matches
    False if it doesn't
    Exits if error.
------------------------------------------------------------------------------*/
bool xlnx_utils_check_if_pattern_matches(char* pattern, char* str);

/* -----------------------------------------------------------------------------
xlnx_utils_set_size_t_arg: Set the destination to be 
Parameters:
    destination: The parameter to store the value
    source: The source to parse
    param_name: The name of the parameter in case of failure. Set to "" to 
        avoid logging an error.
Return:
    RET_ERROR on failure
    RET_SUCCESS on success
------------------------------------------------------------------------------*/
int xlnx_utils_set_size_t_arg(size_t* destination, char* source, 
                              char* param_name);

/* -----------------------------------------------------------------------------
xlnx_utils_set_uint_arg: Set the destination to be 
Parameters:
    destination: The parameter to store the value
    source: The source to parse
    param_name: The name of the parameter in case of failure. Set to "" to 
        avoid logging an error.
Return:
    RET_ERROR on failure
    RET_SUCCESS on success
------------------------------------------------------------------------------*/
int xlnx_utils_set_uint_arg(uint* destination, char* source, char* param_name);


/* -----------------------------------------------------------------------------
xlnx_utils_set_int_arg: Set the destination to be 
Parameters:
    destination: The parameter to store the value
    source: The source to parse
    param_name: The name of the parameter in case of failure. Set to "" to 
        avoid logging an error.
Return:
    RET_ERROR on failure
    RET_SUCCESS on success
------------------------------------------------------------------------------*/
int xlnx_utils_set_int_arg(int* destination, char* source, char* param_name);

/* Timer Functions/structs */

typedef struct XlnxAppTimeTracker
{
    struct timespec start_time;
    struct timespec curr_time;
    struct timespec segment_time;
    int             last_displayed_frame;
} XlnxAppTimeTracker;

/* -----------------------------------------------------------------------------
xlnx_utils_set_segment_time: Used to track how long it has been since the 
    previous 
Parameters:
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
void xlnx_utils_set_segment_time(XlnxAppTimeTracker* timer);

/*------------------------------------------------------------------------------
xlnx_utils_start_tracking_time: Get the curr time stamp 
Parameters: 
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
void xlnx_utils_start_tracking_time(XlnxAppTimeTracker* timer);

/*------------------------------------------------------------------------------
xlnx_utils_get_total_time: Calculate the real time taken
Parameters: 
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
double xlnx_utils_get_total_time(XlnxAppTimeTracker* timer);

/*------------------------------------------------------------------------------
xlnx_utils_get_segment_time: Calculate the time taken since the
    last segment
Parameters: 
    timer: The timer struct which stores the necessary timespecs.
Return: The time taken of this segment
------------------------------------------------------------------------------*/
double xlnx_utils_get_segment_time(XlnxAppTimeTracker* timer);

#endif
