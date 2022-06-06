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

#include "xlnx_app_utils.h"

/**
 * xlnx_utils_kbhit: returns true if a key has been pressed
 * @return true if a key has been pressed, otherwise false
 */
static int xlnx_utils_kbhit()
{
    struct timeval tv;
    fd_set         fds;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

/**
 * xlnx_utils_was_q_pressed: returns true if q has been pressed
 * @return true if q has been pressed, otherwise false
 */
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

/**
 * xlnx_utils_set_non_blocking: sets or releases blocking when checking if a key
 * has been pressed
 * @param state: if 0, sets blocking; all other values releases blocking
 */
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

/**
 * xlnx_utils_gcd: returns the greatest common denomimator of two values
 * @param a: first value
 * @param b: second value
 * @return the greatest common denomimator of a and b
 */
uint32_t xlnx_utils_gcd(uint32_t a, uint32_t b)
{
    while(b != 0) {
        uint32_t t = b;
        b          = a % b;
        a          = t;
    }
    return a;
}

/* Argument Functions */

/**
 * xlnx_utils_check_if_pattern_matches: Checks if str matches the pattern
 * @param pattern: The parameter which will compile into a regex
 * @param str: The source to check
 * @return True if it matches, False if it doesn't, Exits if error.
 */
bool xlnx_utils_check_if_pattern_matches(char* pattern, char* str)
{
    regex_t regex;
    int     ret, matches;
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if(ret) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                   "Error compiling regex using pattern \"%s\"!\n", pattern);
        exit(XMA_APP_ERROR);
    }
    ret = regexec(&regex, str, 0, NULL, 0);
    if(!ret) { // It matches
        matches = true;
    } else if(ret == REG_NOMATCH) {
        matches = false;
    } else { // Error
        char error_buffer[100];
        regerror(ret, &regex, error_buffer, sizeof(error_buffer));
        xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                   "Regex match failed: %s\n", error_buffer);
        exit(XMA_APP_ERROR);
    }
    regfree(&regex);
    return matches;
}

/**
 * xlnx_utils_set_size_t_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * return XMA_APP_ERROR on failure XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_size_t_arg(size_t* destination, char* source,
                              char* param_name)
{
    bool is_param_given = strcmp(param_name, "") != 0;
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+$", source)) {
        if(is_param_given) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                       "Unrecognized value \"%s\" for argument -%s! Make sure"
                       " the value is of proper type.\n",
                       source, param_name);
        }
        return XMA_APP_ERROR;
    }
    char* end_ptr;
    errno        = 0;
    *destination = strtoumax(source, &end_ptr, 10);
    if(errno) {
        if(is_param_given) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                       "Unable to set param %s. %s\n", param_name, errno);
        }
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_utils_set_uint_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * eturn XMA_APP_ERROR on failure
 *   XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_uint_arg(uint* destination, char* source, char* param_name)
{
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+$", source)) {
        if(strcmp(param_name, "") != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                       "Unrecognized value \"%s\" for argument -%s! Make sure"
                       " the value is of proper type.\n",
                       source, param_name);
        }
        return XMA_APP_ERROR;
    }
    *destination = (uint32_t)atol(source);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_utils_set_int_arg: Set the destination to be
 * @param destination: The parameter to store the value
 * @param source: The source to parse
 * @param param_name: The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 * eturn XMA_APP_ERROR on failure
 *   XMA_APP_SUCCESS on success
 */
int xlnx_utils_set_int_arg(int* destination, char* source, char* param_name)
{
    if(!xlnx_utils_check_if_pattern_matches("^-?[0-9]+$", source)) {
        if(strcmp(param_name, "") != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                       "Unrecognized value \"%s\" for argument -%s! Make sure"
                       " the value is of proper type.\n",
                       source, param_name);
        }
        return XMA_APP_ERROR;
    }
    *destination = atoi(source);
    return XMA_APP_SUCCESS;
}

/**
 * Get the vcu format for the given xma app format.
 * @param app_fmt The app format source
 * @return The corresponding xma vcu format
 */
int xlnx_utils_app_fmt_to_vcu_fmt(XlnxFormatType app_fmt)
{
    switch(app_fmt) {
        case XMA_APP_YUV420P_FMT_TYPE:
        case XMA_APP_NV12_FMT_TYPE:
            return XMA_VCU_NV12_FMT_TYPE;
        case XMA_APP_YUV420P10LE_FMT_TYPE:
        case XMA_APP_XV15_FMT_TYPE:
            return XMA_VCU_NV12_10LE32_FMT_TYPE;
        default:
            return XMA_APP_ERROR;
    }
}

/**
 * Set the destination to the appropriate xma format type given the source
 * format.
 * @param destination The format type equivalent to the source
 * @param source The source pixel format
 * @param param_name The name of the parameter in case of failure. Set to "" to
 * avoid logging an error.
 */
int xlnx_utils_set_pix_fmt_arg(XlnxFormatType* destination, char* source,
                               char* param_name)
{
    if(strcmp(source, "yuv420p") == 0) {
        *destination = XMA_APP_YUV420P_FMT_TYPE;
    } else if(strcmp(optarg, "nv12") == 0) {
        *destination = XMA_APP_NV12_FMT_TYPE;
    } else if(strcmp(optarg, "yuv420p10le") == 0) {
        *destination = XMA_APP_YUV420P10LE_FMT_TYPE;
    } else if(strcmp(optarg, "xv15") == 0 ||
              strcmp(optarg, "nv12_10le32") == 0) {
        *destination = XMA_APP_XV15_FMT_TYPE;
    } else {
        if(strcmp(param_name, "") != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_APP_UTILS_MODULE,
                       "Unsupported pixel format %s option %s\n", source,
                       optarg);
        }
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Compare two arguments and return the result.
 * @param arg1 First argument for comparison
 * @param arg2 Second argument for comparison
 * @return Comparison result of 2 arguments
 */
int xlnx_utils_compare(const void* arg1, const void* arg2)
{
    return (*(int*)arg1 - *(int*)arg2);
}

/* Timer Functions */

/**
 * xlnx_utils_set_segment_time: Used to track how long it has been since the
 * previous
 * @param timer: The timer struct which stores the necessary timespecs.
 */
void xlnx_utils_set_segment_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->segment_time);
}

/**
 * xlnx_utils_set_current_time: Store the current time in timer->curr_time
 * timespec
 * @param timer: The timer struct which stores the necessary timespecs.
 */
static void xlnx_utils_set_current_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->curr_time);
}

/**
 * xlnx_utils_start_tracking_time: Store the current timestamp for fps
 * calculation later.
 * @param timer: The timer struct which stores the necessary timespecs.
 */
void xlnx_utils_start_tracking_time(XlnxAppTimeTracker* timer)
{
    clock_gettime(CLOCK_REALTIME, &timer->start_time);
    xlnx_utils_set_current_time(timer);
    xlnx_utils_set_segment_time(timer);
    timer->last_displayed_frame = 0;
}

/**
 * xlnx_utils_get_total_time: Calculate the real time taken
 * @param timer: The timer struct which stores the necessary timespecs.
 */
double xlnx_utils_get_total_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    // in microseconds
    double time = (timer->curr_time.tv_sec - timer->start_time.tv_sec) * 1e6 +
                  (timer->curr_time.tv_nsec - timer->start_time.tv_nsec) / 1e3;
    return time / 1000000.0;
}

/**
 * xlnx_utils_get_segment_time: Calculate the time taken for the
 * current segment
 * @param timer: The timer struct which stores the necessary timespecs.
 */
double xlnx_utils_get_segment_time(XlnxAppTimeTracker* timer)
{
    xlnx_utils_set_current_time(timer);
    // in microseconds
    double time =
        (timer->curr_time.tv_sec - timer->segment_time.tv_sec) * 1e6 +
        (timer->curr_time.tv_nsec - timer->segment_time.tv_nsec) / 1e3;
    return time / 1000000.0;
}

/* Pixel Format Conversion Functions */

/**
 * Extracts a 10 bit pixel from a vcu word and stores it in a 16 bit word
 * @param pixel_index Which pixel of the vcu word to take (0-2)
 * @param vcu_word The source vcu word containing 3 pixels
 * @param out_word Where to store the first (LSB) pixel
 * @return void
 */
static void xlnx_utils_extract_pixel_from_xv15_word(uint8_t    pixel_index,
                                                    uint32_t   vcu_word,
                                                    uint16_t** out_word)
{
    if(pixel_index == 0) {
        *(*out_word)++ = (uint16_t)(vcu_word & 0x3FF);
    } else if(pixel_index == 1) {
        *(*out_word)++ = (uint16_t)((vcu_word & 0xFFC00) >> 10);
    } else {
        *(*out_word)++ = (uint16_t)((vcu_word & 0x3FF00000) >> 20);
    }
}

/**
 * Converts an xv15 word into yuv420p10le words stored in the y plane.
 * @param num_pxls_to_xtrct The number of pixels to extract from the source
 * word
 * @param xv15_word The source xv15 word containing 3 pixels of data
 * @param y_plane The output y plane
 * @return void
 */
static void xlnx_utils_y_xv15_wrd_10le_wrds(uint8_t    num_pxls_to_xtrct,
                                            uint32_t   xv15_word,
                                            uint16_t** y_plane)
{
    switch(num_pxls_to_xtrct) {
        case 3:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word, y_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word, y_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word, y_plane);
            break;
        case 2:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word, y_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word, y_plane);
            break;
        case 1:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word, y_plane);
            break;
        default:
            return;
    }
}

/**
 * Converts 1-2 xv15 words into yuv420p10le words stored in their respective u
 * & v planes.
 * @param num_pxls_to_xtrct The number of pixels to extract from the source
 * words
 * @param xv15_word1 The first xv15 source word
 * @param xv15_word2 The second xv15 source word
 * @param u_plane The output u plane
 * @param v_plane The output v plane
 * @return void
 */
static void xlnx_utils_uv_xv15_wrd_to_10le_wrds(uint8_t    num_pxls_to_xtrct,
                                                uint32_t   xv15_word1,
                                                uint32_t   xv15_word2,
                                                uint16_t** u_plane,
                                                uint16_t** v_plane)
{
    switch(num_pxls_to_xtrct) {
        case 6:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word1, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word1, u_plane);

            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word2, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word2, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word2, v_plane);
            break;
        case 5:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word1, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word1, u_plane);

            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word2, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word2, u_plane);
            break;
        case 4:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word1, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word1, u_plane);

            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word2, v_plane);
            break;
        case 3:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word1, v_plane);
            xlnx_utils_extract_pixel_from_xv15_word(2, xv15_word1, u_plane);
            break;
        case 2:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            xlnx_utils_extract_pixel_from_xv15_word(1, xv15_word1, v_plane);
            break;
        case 1:
            xlnx_utils_extract_pixel_from_xv15_word(0, xv15_word1, u_plane);
            break;
        default:
            return;
    }
}

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
                                   uint32_t* xv15_uv, FILE* out_f)
{
    uint16_t  total_words_in_line = xv15_linesize / sizeof(uint32_t);
    uint16_t  valid_words_in_line = width / 3;
    uint8_t   leftover_pixels     = width % 3;
    uint16_t  num_rows_in_plane   = height;
    size_t    yuv420p10le_y_size  = width * height * 2;
    size_t    yuv420p10le_u_size  = yuv420p10le_y_size >> 2;
    uint16_t* tmp_y               = malloc(yuv420p10le_y_size);
    uint16_t* tmp_u               = malloc(yuv420p10le_u_size);
    uint16_t* tmp_v               = malloc(yuv420p10le_u_size);
    uint16_t  w, h;
    for(h = 0; h < num_rows_in_plane; h++) {
        for(w = 0; w < valid_words_in_line; w++) {
            xlnx_utils_y_xv15_wrd_10le_wrds(3, xv15_y[w], &tmp_y);
        }
        xlnx_utils_y_xv15_wrd_10le_wrds(leftover_pixels, xv15_y[w], &tmp_y);
        xv15_y += total_words_in_line;
    }
    num_rows_in_plane   = height / 2;
    valid_words_in_line = width / 6; // Reading 2 words at a time
    leftover_pixels     = width % 6;
    size_t word_index;
    for(h = 0; h < num_rows_in_plane; h++) {
        word_index = 0;
        for(w = 0; w < valid_words_in_line; w++) {
            xlnx_utils_uv_xv15_wrd_to_10le_wrds(6, xv15_uv[word_index],
                                                xv15_uv[word_index + 1], &tmp_u,
                                                &tmp_v);
            word_index += 2;
        }
        xlnx_utils_uv_xv15_wrd_to_10le_wrds(
            leftover_pixels, xv15_uv[word_index], xv15_uv[word_index + 1],
            &tmp_u, &tmp_v);
        xv15_uv += total_words_in_line;
    }
    tmp_y -= yuv420p10le_y_size / sizeof(uint16_t);
    tmp_u -= yuv420p10le_u_size / sizeof(uint16_t);
    tmp_v -= yuv420p10le_u_size / sizeof(uint16_t);
    fwrite(tmp_y, yuv420p10le_y_size, 1, out_f);
    fwrite(tmp_u, yuv420p10le_u_size, 1, out_f);
    fwrite(tmp_v, yuv420p10le_u_size, 1, out_f);
    free(tmp_y);
    free(tmp_u);
    free(tmp_v);
    return XMA_APP_SUCCESS;
}

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
                               uint8_t* nv12_uv, FILE* out_f)
{
    size_t   yuv420p_u_size = (width * height) >> 2;
    uint8_t* tmp_u          = malloc(yuv420p_u_size);
    uint8_t* tmp_v          = malloc(yuv420p_u_size);
    uint16_t w, h;
    /* Y plane */
    for(h = 0; h < height; h++) {
        fwrite(nv12_y, width, 1, out_f);
        nv12_y += nv12_linesize;
    }

    /* uv plane */
    height = height / 2;
    for(h = 0; h < height; h++) {
        for(w = 0; w < width; w += 2) {
            tmp_u[0] = nv12_uv[w];
            tmp_v[0] = nv12_uv[w + 1];
            tmp_u++;
            tmp_v++;
        }
        nv12_uv += nv12_linesize;
    }
    tmp_u -= yuv420p_u_size;
    tmp_v -= yuv420p_u_size;
    fwrite(tmp_u, yuv420p_u_size, 1, out_f);
    fwrite(tmp_v, yuv420p_u_size, 1, out_f);
    free(tmp_u);
    free(tmp_v);
    return XMA_APP_SUCCESS;
}

/**
 * Remove the padding from the vcu buffer received vcu buffer
 * @param valid_bytes_in_line The number of valid bytes in each line of the vcu
 * buffer
 * @param total_bytes_in_line the total number of bytes in each line of the vcu
 * buffer
 * @param height The height of the output frame
 * @param src_y The padded vcu y buffer
 * @param src_uv The padded vcu uv buffer
 * @param out_f The opened output file to which output is written
 */
int xlnx_utils_remove_vcu_padding(uint16_t valid_bytes_in_line,
                                  uint16_t total_bytes_in_line, uint16_t height,
                                  void* src_y, void* src_uv, FILE* out_f)
{
    uint16_t h;
    for(h = 0; h < height; h++) {
        fwrite(src_y, valid_bytes_in_line, 1, out_f);
        src_y += total_bytes_in_line;
    }
    height = height / 2;
    for(h = 0; h < height; h++) {
        fwrite(src_uv, valid_bytes_in_line, 1, out_f);
        src_uv += total_bytes_in_line;
    }
    fflush(out_f);
    return XMA_APP_SUCCESS;
}

/**
 * Write the values of 3 pixels into the next word of the xv15 (aka nv12_10le32)
 * buffer and increment the buffer to the next 32 bit WORD.
 * @param p1 The first pixel to be written (LSB)
 * @param p2 The second pixel to be written
 * @param p3 The third pixel to be written
 * @param xv15_buffer A pointer to the output xv15 (aka nv12_10le32)
 * buffer
 * @return void
 */
static void xlnx_utils_yuv10b_pixls_to_xv15_wrd(uint16_t p1, uint16_t p2,
                                                uint16_t   p3,
                                                uint32_t** xv15_buffer)
{
    *(*xv15_buffer)++ = 0x3FFFFFFF & (p1 | (p2 << 10) | (p3 << 20));
}

/**
 * Write up to 3 pixels from the source y buffer into the xv15 (aka nv12_10le32)
 * buffer
 * @param num_pixels_to_write The number of pixels to write. 1-3
 * @param y_buffer A pointer to the source y plane buffer
 * @param xv15_buffer A pointer to the output xv15 (aka nv12_10le32) buffer
 * @return void
 */
static void xlnx_utils_y_10b_seg_to_xv15_wrd(uint8_t    num_pixels_to_write,
                                             uint16_t** y_buffer,
                                             uint32_t** xv15_buffer)
{
    switch(num_pixels_to_write) {
        case 3:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*y_buffer)[0], (*y_buffer)[1],
                                                (*y_buffer)[2], xv15_buffer);
            break;
        case 2:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*y_buffer)[0], (*y_buffer)[1],
                                                0, xv15_buffer);
            break;
        case 1:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*y_buffer)[0], 0, 0,
                                                xv15_buffer);
            break;
        default:
            return;
    }
    *y_buffer += num_pixels_to_write;
}

/**
 * Write up to 6 pixels from the source u & v buffers into the xv15
 * (aka nv12_10le32) buffer
 * @param num_pixels_to_write The number of pixels to write. 1-6
 * @param u_buffer A pointer to the source u plane buffer
 * @param v_buffer A pointer to the source v plane buffer
 * @param xv15_buffer A pointer to the output xv15 (aka nv12_10le32) buffer
 * @return void
 */
static void xlnx_utils_uv_10b_seg_to_xv15_wrd(uint8_t    num_pixels_to_write,
                                              uint16_t** u_buffer,
                                              uint16_t** v_buffer,
                                              uint32_t** xv15_buffer)
{
    switch(num_pixels_to_write) {
        case 6:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], (*v_buffer)[0],
                                                (*u_buffer)[1], xv15_buffer);
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*v_buffer)[1], (*u_buffer)[2],
                                                (*v_buffer)[2], xv15_buffer);
            break;
        case 5:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], (*v_buffer)[0],
                                                (*u_buffer)[1], xv15_buffer);
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*v_buffer)[1], (*u_buffer)[2],
                                                0, xv15_buffer);
            break;
        case 4:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], (*v_buffer)[0],
                                                (*u_buffer)[1], xv15_buffer);
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*v_buffer)[1], 0, 0,
                                                xv15_buffer);
            break;
        case 3:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], (*v_buffer)[0],
                                                (*u_buffer)[1], xv15_buffer);
            break;
        case 2:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], (*v_buffer)[0],
                                                0, xv15_buffer);
            break;
        case 1:
            xlnx_utils_yuv10b_pixls_to_xv15_wrd((*u_buffer)[0], 0, 0,
                                                xv15_buffer);
            break;
        default:
            return;
    }
    *u_buffer += (num_pixels_to_write + 1) / 2;
    *v_buffer += num_pixels_to_write / 2;
}

/**
 * @param width width of the frame
 * @param height height of the frame
 * @param xv15_linesize output linesize for the xv15 buffer
 * @param yuv420p10le_y yuv420p10le y plane buffer
 * @param yuv420p10le_u yuv420p10le y plane buffer
 * @param yuv420p10le_v yuv420p10le y plane buffer
 * @param xv15_y xv15 y plane buffer to be filled
 * @param xv15_uv xv15 uv plane buffer to be filled
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_utils_yuv420p10le_to_xv15(uint16_t width, uint16_t height,
                                          uint16_t*  yuv420p10le_y,
                                          uint16_t*  yuv420p10le_u,
                                          uint16_t*  yuv420p10le_v,
                                          uint32_t** xv15_y, uint32_t** xv15_uv)
{
    uint32_t* current_buffer  = (uint32_t*)(*xv15_y);
    uint16_t  rows_in_plane   = height;
    uint16_t  words_in_line   = width / 3;
    uint8_t   leftover_pixels = width % 3;
    uint16_t  w, h;
    for(h = 0; h < rows_in_plane; h++) {
        for(w = 0; w < words_in_line; w++) {
            xlnx_utils_y_10b_seg_to_xv15_wrd(3, &yuv420p10le_y,
                                             &current_buffer);
        }
        if(leftover_pixels) {
            xlnx_utils_y_10b_seg_to_xv15_wrd(leftover_pixels, &yuv420p10le_y,
                                             &current_buffer);
        }
    }

    current_buffer  = (uint32_t*)(*xv15_uv);
    words_in_line   = width / 6;
    leftover_pixels = width % 6;
    rows_in_plane   = height / 2;
    for(h = 0; h < rows_in_plane; h++) {
        for(w = 0; w < words_in_line; w++) {
            xlnx_utils_uv_10b_seg_to_xv15_wrd(6, &yuv420p10le_u, &yuv420p10le_v,
                                              &current_buffer);
        }
        if(leftover_pixels) {
            xlnx_utils_uv_10b_seg_to_xv15_wrd(leftover_pixels, &yuv420p10le_u,
                                              &yuv420p10le_v, &current_buffer);
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * Convert the yuv420p input into nv12 output
 * @param y_size The size of the y plane of the frame
 * @param yuv420p_y yuv420p y plane buffer
 * @param yuv420p_u yuv420p u plane buffer
 * @param yuv420p_v yuv420p v plane buffer
 * @param nv12_y nv12 y plane buffer to be filled
 * @param nv12_uv nv12 uv plane buffer to be filled
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_utils_yuv420p_to_nv12(size_t y_size, uint8_t* yuv420p_y,
                                      uint8_t* yuv420p_u, uint8_t* yuv420p_v,
                                      uint8_t** nv12_y, uint8_t** nv12_uv)
{
    size_t u_size = y_size >> 2;
    /* Write y */
    memcpy(*nv12_y, yuv420p_y, y_size);
    /* Write uv */
    for(size_t i = 0; i < u_size; i++) {
        (*nv12_uv)[2 * i]     = yuv420p_u[i];
        (*nv12_uv)[2 * i + 1] = yuv420p_v[i];
    }
    return XMA_APP_SUCCESS;
}

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
                              void** out_uv)
{
    int               ret          = XMA_APP_SUCCESS;
    XlnxBitsPerPixels bpp          = xlnx_utils_get_bit_depth_app_fmt(fmt);
    size_t            frame_size_y = width * height * ((bpp + 7) >> 3);
    size_t            read_size    = 0;
    switch(fmt) {
        case XMA_APP_YUV420P_FMT_TYPE:
        case XMA_APP_YUV420P10LE_FMT_TYPE:;
            size_t frame_size_u = frame_size_y >> 2;
            void*  tmp_y        = malloc(frame_size_y);
            void*  tmp_u        = malloc(frame_size_u);
            void*  tmp_v        = malloc(frame_size_u); // /u/ == /v/
            read_size           = fread(tmp_y, 1, frame_size_y, in_fp);
            read_size += fread(tmp_u, 1, frame_size_u, in_fp);
            read_size += fread(tmp_v, 1, frame_size_u, in_fp);
            if(read_size != frame_size_y * 1.5) {
                free(tmp_y);
                free(tmp_u);
                free(tmp_v);
                return XMA_APP_EOF;
            }
            if(bpp == BITS_PER_PIXEL_10) {
                ret = xlnx_utils_yuv420p10le_to_xv15(
                    width, height, (uint16_t*)tmp_y, (uint16_t*)tmp_u,
                    (uint16_t*)tmp_v, (uint32_t**)out_y, (uint32_t**)out_uv);
            } else {
                ret = xlnx_utils_yuv420p_to_nv12(
                    frame_size_y, (uint8_t*)tmp_y, (uint8_t*)tmp_u,
                    (uint8_t*)tmp_v, (uint8_t**)out_y, (uint8_t**)out_uv);
            }
            free(tmp_y);
            free(tmp_u);
            free(tmp_v);
            break;
        case XMA_APP_XV15_FMT_TYPE:
            frame_size_y = (((width + 2) / 3) * 4) * height;
            // fall through
        case XMA_APP_NV12_FMT_TYPE:
            read_size = fread(*out_y, 1, frame_size_y, in_fp);
            read_size += fread(*out_uv, 1, frame_size_y >> 1, in_fp);
            if(read_size != frame_size_y * 1.5) {
                return XMA_APP_EOF;
            }
            break;
        default:
            assert(!"Unformatted pixel format passed in!");
    }
    return ret;
}
