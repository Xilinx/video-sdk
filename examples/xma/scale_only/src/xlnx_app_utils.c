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

#include "xlnx_app_utils.h"

/*------------------------------------------------------------------------------
xlnx_utils_kbhit: returns true if a key has been pressed
Return:
    true if a key has been pressed, otherwise false
------------------------------------------------------------------------------*/
static int xlnx_utils_kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

/*------------------------------------------------------------------------------
xlnx_utils_was_q_pressed: returns true if q has been pressed
Return:
    true if q has been pressed, otherwise false
------------------------------------------------------------------------------*/
bool xlnx_utils_was_q_pressed()
{
    if(xlnx_utils_kbhit() != 0) { // if the user pressed a key
        char c = fgetc(stdin);
        if((c == 'q') || (c == 'Q')) { // if the key pressed is 'q'
            return true;
        }
    }
    return false;
}

/*------------------------------------------------------------------------------
xlnx_utils_set_non_blocking: sets or releases blocking when checking if a key 
    has been pressed
Parameters:
    state: if 0, sets blocking; all other values releases blocking
------------------------------------------------------------------------------*/
void xlnx_utils_set_non_blocking(int state)
{
    struct termios ttystate;

    tcgetattr(STDIN_FILENO, &ttystate);
    if(state != 0) {
        ttystate.c_lflag &= ~ICANON;
        ttystate.c_cc[VMIN] = 1;
    } else {
        ttystate.c_lflag |= ICANON;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}

/*------------------------------------------------------------------------------
xlnx_utils_gcd: returns the greatest common denomimator of two values
paramters:
    a: first value
    b: second value
Return:
    the greatest common denomimator of a and b
------------------------------------------------------------------------------*/
unsigned long xlnx_utils_gcd(unsigned long a, unsigned long b)
{
    while(b != 0) {
        unsigned long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/* Argument Functions */

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
bool xlnx_utils_check_if_pattern_matches(char* pattern, char* str)
{
    regex_t regex;
    int ret, matches;
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret) {
        XLNX_APP_UTILS_LOG_ERROR("Error compiling regex using pattern "
                                 "\"%s\"!\n", pattern);
        exit(RET_ERROR);
    }
    ret = regexec(&regex, str, 0, NULL, 0);
    if(!ret) { // It matches
        matches = true;
    } else if(ret == REG_NOMATCH) {
        matches = false;
    } else { // Error
        char error_buffer[100];
        regerror(ret, &regex, error_buffer, sizeof(error_buffer));
        XLNX_APP_UTILS_LOG_ERROR("Regex match failed: %s\n", error_buffer);
        exit(RET_ERROR);
    }
    regfree(&regex);
    return matches;
}

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
                              char* param_name)
{
    bool is_param_given = strcmp(param_name, "") != 0;
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+$", source)) {
        if(is_param_given) {
            XLNX_APP_UTILS_LOG_ERROR("Unrecognized value \"%s\" for argument "
                                     "-%s! Make sure the value is of proper "
                                     "type.\n", source, param_name);
        }
        return RET_ERROR;
    }
    char* end_ptr;
    errno = 0;
    *destination = strtoumax(source, &end_ptr, 10);
    if(errno) {
        if(is_param_given) {
            XLNX_APP_UTILS_LOG_ERROR("Unable to set param %s. %s\n", param_name,
                                     errno);
        }
        return RET_ERROR;
    }
    return RET_SUCCESS;
}

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
int xlnx_utils_set_uint_arg(uint* destination, char* source, char* param_name)
{
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+$", source)) {
        if(strcmp(param_name, "") != 0) {
            XLNX_APP_UTILS_LOG_ERROR("Unrecognized value \"%s\" for argument "
                                     "-%s! Make sure the value is of proper "
                                     "type.\n", source, param_name);
        }
        return RET_ERROR;
    }
    *destination = (uint32_t) atol(source);
    return RET_SUCCESS;
}

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
int xlnx_utils_set_int_arg(int* destination, char* source, char* param_name)
{
    if(!xlnx_utils_check_if_pattern_matches("^-?[0-9]+$", source)) {
        if(strcmp(param_name, "") != 0) {
            XLNX_APP_UTILS_LOG_ERROR("Unrecognized value \"%s\" for argument "
                                     "-%s! Make sure the value is of proper "
                                     "type.\n", source, param_name);
        }
        return RET_ERROR;
    }
    *destination = atoi(source);
    return RET_SUCCESS;
}

/* Timer Functions */

/* -----------------------------------------------------------------------------
xlnx_utils_set_segment_time: Used to track how long it has been since the 
    previous 
Parameters:
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
void xlnx_utils_set_segment_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->segment_time);
}

/* -----------------------------------------------------------------------------
xlnx_utils_set_current_time: Store the current time in timer->curr_time 
    timespec
Parameters:
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
static void xlnx_utils_set_current_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->curr_time);
}

/*------------------------------------------------------------------------------
xlnx_utils_start_tracking_time: Store the current timestamp for fps calculation 
    later.
Parameters:
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
void xlnx_utils_start_tracking_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->start_time);
    xlnx_utils_set_current_time(timer);
    xlnx_utils_set_segment_time(timer);
    timer->last_displayed_frame = 0;
}

/*------------------------------------------------------------------------------
xlnx_utils_get_total_time: Calculate the real time taken
Parameters: 
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
double xlnx_utils_get_total_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    // in microseconds
    double time = (timer->curr_time.tv_sec - timer->start_time.tv_sec) *
                  1e6 + (timer->curr_time.tv_nsec - 
                  timer->start_time.tv_nsec) / 1e3; 
    return time / 1000000.0;
}

/*------------------------------------------------------------------------------
xlnx_utils_get_segment_time: Calculate the time taken for the 
    current segment
Parameters: 
    timer: The timer struct which stores the necessary timespecs.
------------------------------------------------------------------------------*/
double xlnx_utils_get_segment_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    // in microseconds
    double time = (timer->curr_time.tv_sec - timer->segment_time.tv_sec) *
                  1e6 + (timer->curr_time.tv_nsec - 
                  timer->segment_time.tv_nsec) / 1e3; 
    return time / 1000000.0;
}
