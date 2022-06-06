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

#include "xlnx_enc_arg_parse.h"

static struct option enc_options[] = {
    {FLAG_HELP, no_argument, 0, HELP_ARG},
    {FLAG_DEVICE_ID, required_argument, 0, DEVICE_ID_ARG},
    {FLAG_STREAM_LOOP, required_argument, 0, LOOP_COUNT_ARG},
    {FLAG_INPUT_FILE, required_argument, 0, INPUT_FILE_ARG},
    {FLAG_CODEC_TYPE, required_argument, 0, ENCODER_ARG},
    {FLAG_INPUT_WIDTH, required_argument, 0, INPUT_WIDTH_ARG},
    {FLAG_INPUT_HEIGHT, required_argument, 0, INPUT_HEIGHT_ARG},
    {FLAG_INPUT_PIX_FMT, required_argument, 0, INPUT_PIX_FMT_ARG},
    {FLAG_BITRATE, required_argument, 0, BITRATE_ARG},
    {FLAG_BIT_RATE, required_argument, 0, BITRATE_ARG},
    {FLAG_FPS, required_argument, 0, FPS_ARG},
    {FLAG_INTRA_PERIOD, required_argument, 0, INTRA_PERIOD_ARG},
    {FLAG_CONTROL_RATE, required_argument, 0, CONTROL_RATE_ARG},
    {FLAG_MAX_BITRATE, required_argument, 0, MAX_BITRATE_ARG},
    {FLAG_SLICE_QP, required_argument, 0, SLICE_QP_ARG},
    {FLAG_MIN_QP, required_argument, 0, MIN_QP_ARG},
    {FLAG_MAX_QP, required_argument, 0, MAX_QP_ARG},
    {FLAG_NUM_BFRAMES, required_argument, 0, NUM_BFRAMES_ARG},
    {FLAG_IDR_PERIOD, required_argument, 0, IDR_PERIOD_ARG},
    {FLAG_DYN_IDR, required_argument, 0, DYNAMIC_IDR},
    {FLAG_PROFILE, required_argument, 0, PROFILE_ARG},
    {FLAG_LEVEL, required_argument, 0, LEVEL_ARG},
    {FLAG_NUM_SLICES, required_argument, 0, NUM_SLICES_ARG},
    {FLAG_QP_MODE, required_argument, 0, QP_MODE_ARG},
    {FLAG_ASPECT_RATIO, required_argument, 0, ASPECT_RATIO_ARG},
    {FLAG_SCALING_LIST, required_argument, 0, SCALING_LIST_ARG},
    {FLAG_LOOKAHEAD_DEPTH, required_argument, 0, LOOKAHEAD_DEPTH_ARG},
    {FLAG_TEMPORAL_AQ, required_argument, 0, TEMPORAL_AQ_ARG},
    {FLAG_SPATIAL_AQ, required_argument, 0, SPATIAL_AQ_ARG},
    {FLAG_SPATIAL_AQ_GAIN, required_argument, 0, SPATIAL_AQ_GAIN_ARG},
    {FLAG_QP, required_argument, 0, QP_ARG},
    {FLAG_NUM_FRAMES, required_argument, 0, NUM_FRAMES_ARG},
    {FLAG_NUM_CORES, required_argument, 0, NUM_CORES_ARG},
    {FLAG_TUNE_METRICS, required_argument, 0, TUNE_METRICS_ARG},
    {FLAG_LATENCY_LOGGING, required_argument, 0, LATENCY_LOGGING_ARG},
    {FLAG_EXPERT_OPTIONS, required_argument, 0, EXPERT_OPTIONS_ARG},
    {FLAG_OUTPUT_FILE, required_argument, 0, OUTPUT_FILE_ARG},
    {0, 0, 0, 0}};

XlnxEncProfileLookup enc_prof_h264_lookup[] = {
    {"baseline", ENC_H264_BASELINE},
    {"main", ENC_H264_MAIN},
    {"high", ENC_H264_HIGH},
    {"high-10", ENC_H264_HIGH_10},
    {"high-10-intra", ENC_H264_HIGH_10_INTRA}};

XlnxEncProfileLookup enc_prof_hevc_lookup[] = {
    {"main", ENC_HEVC_MAIN},
    {"main-intra", ENC_HEVC_MAIN_INTRA},
    {"main-10", ENC_HEVC_MAIN_10},
    {"main-10-intra", ENC_HEVC_MAIN_10_INTRA}};

#define ENC_PROF_H264_KEYS                                                     \
    (sizeof(enc_prof_h264_lookup) / sizeof(XlnxEncProfileLookup))
#define ENC_PROF_HEVC_KEYS                                                     \
    (sizeof(enc_prof_hevc_lookup) / sizeof(XlnxEncProfileLookup))

/**
 * xlnx_enc_get_help: Prints the list of supported arguments for encoder xma
 * application
 *
 * @return List of supported arguments
 */
char* xlnx_enc_get_help()
{

    return " XMA Encoder App Usage:\n\t"
           "./program [input options] -i input-file -c:v <codec-option> "
           " [encoder options] -o <output-file>\n\n"
           "Arguments:\n\n"
           "\t--help                     Print this message and exit.\n"
           "\t-d <device-id>             Specify a device on which the "
           "encoder\n"
           "\t                           to run. Default: 0\n"
           "\t-frames <frame-count>      Number of frames to be processed.\n\n"
           "Input options:\n\n"
           "\t-stream_loop <loop-count>  Number of times to loop the input "
           "YUV\n"
           "\t                           file.\n"
           "\t-w <width>                 Width of YUV input.\n"
           "\t-h <height>                Height of YUV input.\n"
           "\t-pix_fmt <pixel-format>    Pix format of the input file "
           "(yuv420p,\n"
           "\t                           nv12, yuv420p10le, xv15). Default: "
           "nv12\n"
           "\t-i <input-file>            Name and path of input YUV file\n\n"
           "Codec option:\n"
           "\t-c:v <codec>               Encoder codec to be used. Supported\n"
           "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264\n"
           "Encoder params:\n"
           "\t-b:v <bitrate>             Bitrate can be given in Kbps or Mbps\n"
           "\t                           or bits i.e., 5000000, 5000K, 5M.\n"
           "\t                           Default is 200kbps\n"
           "\t-fps <fps>                 Input frame rate. Default is 25.\n"
           "\t-g <intraperiod>           Intra period. Default is 12.\n"
           "\t-control-rate <mode>       Rate control mode. Supported are 0\n"
           "\t                           to 3, default is 1.\n"
           "\t-max-bitrate <bitrate>     Maximum bit rate. Supported are 0 to\n"
           "\t                           350000000, default is 5000\n"
           "\t-slice-qp <qp>             Slice QP. Supported are -1 to 51,\n"
           "\t                           default is -1\n"
           "\t-min-qp <qp>               Minimum QP. Supported are 0 to 51,\n"
           "\t                           default is 0.\n"
           "\t-max-qp <qp>               Maximum QP. Supported values are 0\n"
           "\t                           to 51, default is 51.\n"
           "\t-bf <frames>               Number of B frames. Supported are 0\n"
           "\t                           to INT_MAX, default is 2.\n"
           "\t                           For GOP mode 0, supported are 0 to 4\n"
           "\t                           For GOP mode 1, supported 3, 5, 7, "
           "15\n"
           "\t                           For GOP mode 2, no B frames.\n"
           "\t                           For GOP mode 3, supported are 0 to "
           "INT_MAX\n"
           "\t-periodicity-idr <value>   IDR picture frequency. Supported are\n"
           "\t                           0 to UINT32_MAX, default is\n"
           "\t                           UINT32_MAX.\n"
           "\t-force_key_frame <value>   Insert idr frame dynamically (Frame\n"
           "\t                           numbers starting from 0)\n"
           "\t                           Example: -force_key_frame \"(122,222,"
           "333)\"\n"
           "\t-profile <value>           Encoder profile.\n"
           "\t           For HEVC, supported are 0 or main, 1 or main-intra, \n"
           "\t                           2 or main-10 and 3 or main-10-intra. "
           "\n"
           "\t                           Default is 0/main.\n"
           "\t                           ENC_HEVC_MAIN - 0 or main.\n"
           "\t                           ENC_HEVC_MAIN_INTRA - 1 or "
           "main-intra.\n"
           "\t                           ENC_HEVC_MAIN_10 - 2 or main-10.\n"
           "\t                           ENC_HEVC_MAIN_10_INTRA - 3 or "
           "main-10-intra.\n"
           "\t           For H264, supported are 66 or baseline, 77 or main, \n"
           "\t                           100 or high, 110 or high-10 and \n"
           "\t                           2158 or high-10-intra. Default is "
           "100\n"
           "\t                           ENC_H264_BASELINE - 66 or baseline.\n"
           "\t                           ENC_H264_MAIN - 77 or main.\n"
           "\t                           ENC_H264_HIGH - 100 or high.\n"
           "\t                           ENC_H264_HIGH_10 - 110 or high-10.\n"
           "\t                           ENC_H264_HIGH_10_INTRA - 2158 or "
           "high-10-intra.\n"
           "\t-level <value>             Encoder level.\n"
           "\t                           For HEVC, supported are 10, 11, 20,\n"
           "\t                           21, 30, 31, 40, 41, 50, 51.\n"
           "\t                           default is 10.\n"
           "\t                           For H264, supported are 10, 11, 12,\n"
           "\t                           13, 20, 21, 22, 30, 31, 32, 40, 41, "
           "42,"
           "\t                           50, 51, 52.\n"
           "\t                           default is 10.\n"
           "\t-slices <value>            Number of slices per frame. "
           "Supported\n"
           "\t                           are 1 to 68, default is 1.\n"
           "\t-qp-mode <mode>            QP mode. Supported are 0, 1, and 2,\n"
           "\t                           default is 1.\n"
           "\t-aspect-ratio <value>      Aspect ratio. Supported values are 0\n"
           "\t                           to 3, default is 0.\n"
           "\t-scaling-list <0/1>        Scaling list. Enable/Disable,\n"
           "\t                           default enable.\n"
           "\t-lookahead-depth <value>   Lookahead depth. Supported are 0 to\n"
           "\t                           20, default is 0.\n"
           "\t-temporal-aq <0/1>         Temporal AQ. Enable/Disable,\n"
           "\t                           default disable.\n"
           "\t-spatial-aq <0/1>          Spatial AQ. Enable/Disable,\n"
           "\t                           default disable.\n"
           "\t-spatial-aq-gain <value>   Spatial AQ gain. Supported are 0 to\n"
           "\t                           100, default is 50.\n"
           "\t-cores <value>             Number of cores to use, supported "
           "are\n"
           "\t                           0 to 4, default is 0.\n"
           "\t-tune-metrics <0/1>        Tunes MPSoC H.264/HEVC encoder's "
           "video\n"
           "\t                           quality for objective metrics, "
           "default\n"
           "\t                           disable.\n"
           "\t-latency_logging <0/1>     Enable latency logging in syslog.\n"
           "\t-expert-options <string>   Expert options for the encoder, "
           "stored\n"
           "\t                           as string of key=value pairs.\n"
           "\t                           dynamic-params=<params_file>\n"
           "\t-o <file>                  File to which output is written.\n";
}

/**
 * xlnx_enc_key_from_string: maps string input to the encoder profile value
 *
 * @param enc_prof_lookuptable : encoder's lookup table
 * @param profile_key: encoder's profile in string format
 * @param enc_no_keys: number of keys in the lookup table
 * @return The encoder profile key value or XMA_APP_ERROR on error
 */
static int xlnx_enc_key_from_string(XlnxEncProfileLookup* enc_prof_lookuptable,
                                    char* profile_key, int enc_no_keys)
{
    int cnt;
    if(profile_key == NULL)
        return XMA_APP_ERROR;
    for(cnt = 0; cnt < enc_no_keys; cnt++) {
        XlnxEncProfileLookup enc_profile_key = enc_prof_lookuptable[cnt];
        if(strcmp(enc_profile_key.key, profile_key) == 0)
            return enc_profile_key.value;
    }

    return XMA_APP_ERROR;
}

/**
 * xlnx_enc_validate_codec_arguments: Validates encoder codec arguments
 *
 * @param enc_props: Encoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t
    xlnx_enc_validate_codec_arguments(XlnxEncoderProperties* enc_props)
{

    if(enc_props->bit_rate <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid bit-rate param %d\n", enc_props->bit_rate);
        return XMA_APP_ERROR;
    }

    if((enc_props->fps <= 0) || (enc_props->fps > INT_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid frame rate %d\n", enc_props->fps);
        return XMA_APP_ERROR;
    }

    if((enc_props->gop_size < 0) || (enc_props->gop_size > ENC_MAX_GOP_SIZE)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid intra period %d\n", enc_props->gop_size);
        return XMA_APP_ERROR;
    }

    if((enc_props->control_rate < 0) || (enc_props->control_rate > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid rate control mode %d\n", enc_props->control_rate);
        return XMA_APP_ERROR;
    }

    if((enc_props->max_bitrate < 0) ||
       (enc_props->max_bitrate > ENC_SUPPORTED_MAX_BITRATE)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid max btirate %ld\n", enc_props->max_bitrate);
        return XMA_APP_ERROR;
    }

    if((enc_props->slice_qp < -1) ||
       (enc_props->slice_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid slice qp %d\n",
                   enc_props->slice_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->min_qp < ENC_SUPPORTED_MIN_QP) ||
       (enc_props->min_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid min qp %d\n",
                   enc_props->min_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->max_qp < ENC_SUPPORTED_MIN_QP) ||
       (enc_props->max_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid max qp %d\n",
                   enc_props->max_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->qp_mode < 0) || (enc_props->qp_mode > 2)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid qp mode %d\n",
                   enc_props->qp_mode);
        return XMA_APP_ERROR;
    }

    if((enc_props->idr_period < ENC_DEFAULT_IDR_PERIOD) ||
       (enc_props->idr_period > INT32_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid IDR period %d\n", enc_props->idr_period);
        return XMA_APP_ERROR;
    }

    if(enc_props->num_bframes > INT_MAX) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid number of B frames %d\n", enc_props->num_bframes);
        return XMA_APP_ERROR;
    }

    /* Check for valid number of b-frames in different gop-modes */
    switch(enc_props->gop_mode) {
        case 0:
            if(enc_props->num_bframes > 4) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Invalid number of B frames %d for GOP mode %d \n",
                           enc_props->num_bframes, enc_props->gop_mode);
                return XMA_APP_ERROR;
            } else
                break;
        case 1:
            if(!((enc_props->num_bframes == 3) ||
                 (enc_props->num_bframes == 5) ||
                 (enc_props->num_bframes == 7) ||
                 (enc_props->num_bframes == 15))) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Invalid number of B frames %d for GOP mode %d \n",
                           enc_props->num_bframes, enc_props->gop_mode);
                return XMA_APP_ERROR;
            } else
                break;
    }

    if((enc_props->aspect_ratio < 0) || (enc_props->aspect_ratio > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid aspect ratio %d\n", enc_props->aspect_ratio);
        return XMA_APP_ERROR;
    }

    if((enc_props->scal_list != 0) && (enc_props->scal_list != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid scaling list value %d\n", enc_props->scal_list);
        return XMA_APP_ERROR;
    }

    if((enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH) ||
       (enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid LA depth %d\n",
                   enc_props->lookahead_depth);
        return XMA_APP_ERROR;
    }

    if((enc_props->spatial_aq_gain < 0) ||
       (enc_props->spatial_aq_gain > ENC_MAX_SPAT_AQ_GAIN)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid spatial aq gain %d\n", enc_props->spatial_aq_gain);
        return XMA_APP_ERROR;
    }

    if((enc_props->num_cores < 0) && (enc_props->num_cores > 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid number of cores %d\n", enc_props->num_cores);
        return XMA_APP_ERROR;
    }

    if((enc_props->tune_metrics != 0) && (enc_props->tune_metrics != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid value for Tune metrics %d\n",
                   enc_props->tune_metrics);
        return XMA_APP_ERROR;
    }

    if((enc_props->lookahead_depth > enc_props->gop_size) ||
       ((enc_props->idr_period >= 0) &&
        (enc_props->lookahead_depth > enc_props->idr_period))) {
        xma_logmsg(
            XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
            "LA Depth %d cannot be more than GOP size(%d) or IDR period(%d)\n",
            enc_props->lookahead_depth, enc_props->gop_size,
            enc_props->idr_period);
        return XMA_APP_ERROR;
    }

    if(enc_props->codec_id == ENCODER_ID_H264) {
        if(enc_props->level < 10 || enc_props->level > 52) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Unsupported H264 Level %d\n", enc_props->level);
            return XMA_APP_ERROR;
        }
    } else if(enc_props->codec_id == ENCODER_ID_HEVC) {
        if(enc_props->level < 10 || enc_props->level > 51) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Unsupported HEVC Level %d\n", enc_props->level);
            return XMA_APP_ERROR;
        }
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_validate_arguments: Validates generic encoder app arguments
 *
 * @param enc_ctx: Encoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_validate_arguments(XlnxEncoderArguments* arguments)
{
    XlnxEncoderProperties* enc_props      = &arguments->enc_props;
    XlnxEncExpertOptions*  expert_options = &arguments->expert_options;
    int32_t                ret            = XMA_APP_ERROR;
    if(access(arguments->input_file, F_OK) != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unable to access input file \"%s\"\n",
                   arguments->input_file);
        return XMA_APP_ERROR;
    }
    if((arguments->num_frames <= 0) || (arguments->num_frames > SIZE_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid number of frames to encode %zu\n",
                   arguments->num_frames);
        return XMA_APP_ERROR;
    }
    if(arguments->loop_count < -1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid stream_loop %d. 0 <= stream_loop <= MAX_INT."
                   " -1 for infinite loop.\n",
                   arguments->loop_count);
        return XMA_APP_ERROR;
    }
    if((enc_props->device_id < -1) || (enc_props->device_id > 15)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unsupported device ID %d\n", enc_props->device_id);
        return XMA_APP_ERROR;
    }
    if((enc_props->width > ENC_SUPPORTED_MAX_WIDTH) ||
       (enc_props->height > ENC_SUPPORTED_MAX_WIDTH) ||
       ((enc_props->width * enc_props->height) > ENC_SUPPORTED_MAX_PIXELS)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Input resolution %dx%d exceeds maximum supported "
                   "resolution %dx%d\n",
                   enc_props->width, enc_props->height, ENC_SUPPORTED_MAX_WIDTH,
                   ENC_SUPPORTED_MAX_HEIGHT);
        return XMA_APP_ERROR;
    }
    if((enc_props->width < ENC_SUPPORTED_MIN_WIDTH) || (enc_props->width % 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Unsupported width %d\n",
                   enc_props->width);
        return XMA_APP_ERROR;
    }

    if((enc_props->height < ENC_SUPPORTED_MIN_HEIGHT) ||
       (enc_props->height % 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unsupported height %d\n", enc_props->height);
        return XMA_APP_ERROR;
    }

    if(expert_options->dynamic_params_check) {
        if(access(expert_options->dynamic_params_file, F_OK) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Expert-options: "
                       "Unable to access dynamic encoder input parameters "
                       "file \"%s\"\n",
                       expert_options->dynamic_params_file);
            return XMA_APP_ERROR;
        }
    }

    ret = xlnx_enc_validate_codec_arguments(enc_props);
    return ret;
}

/**
 * xlnx_enc_get_br_in_kbps: Get value of bit rate
 * @param desination: Where to store the bitrate
 * @param source: User input value
 * @param param_name: Name of the parameter
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_enc_get_br_in_kbps(int64_t* destination, char* source,
                                   char* param_name)
{
    float br_in_kbps = atof(source);
    if(xlnx_utils_check_if_pattern_matches("^-?[0-9]*\\.?[0-9]+[M|m|K|k]*$",
                                           source) == 0) {

        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unrecognized value "
                   "\"%s\" for argument -%s! Make sure the value is of proper "
                   "type.\n",
                   source, param_name);
        return XMA_APP_ERROR;
    }

    if(xlnx_utils_check_if_pattern_matches("[M|m]+", source)) {
        *destination = br_in_kbps * 1000;
    } else if(xlnx_utils_check_if_pattern_matches("[K|k]+", source)) {
        *destination = br_in_kbps;
    } else {
        *destination = (br_in_kbps / 1000);
    }
    return XMA_APP_SUCCESS;
}

/**
 * Retrieve tokens from a string
 * @param idr_input User's input idr periods
 * @param dyanmic_idr_periods Integer array for idr periods
 * @return Number of dynamic idr values in the user's input string
 */
static int xlnx_enc_retrieve_token(char* idr_input, uint32_t* dyn_idr_arr)
{
    int        count        = 0, ret;
    const char delimiters[] = "( , )";
    char*      token        = strtok(idr_input, delimiters);
    while(token != NULL) {
        ret = xlnx_utils_set_uint_arg(&dyn_idr_arr[count], token, FLAG_DYN_IDR);
        if(ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
        token = strtok(NULL, delimiters);
        count++;
    }
    return count;
}

/**
 * Retrive user's input idr period and save in integer array
 * @param optargs User's input idr periods
 * @param dynamic_idr Struct for dynamic idr parameters
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_enc_get_idr_frames(char* optargs, XlnxDynIdrFrames* dynamic_idr)
{
    int count = 0, i = 0;
    while(optargs[i] != '\0') {
        if(optargs[i] == ',' ||
           (optargs[i] == ' ' &&
            (optargs[i - 1] != ' ' && optargs[i - 1] != ','))) {
            count++;
        }
        i++;
    }
    dynamic_idr->idr_arr = (uint32_t*)calloc(count + 1, sizeof(uint32_t));
    count = xlnx_enc_retrieve_token(optargs, dynamic_idr->idr_arr);
    if(count == XMA_APP_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Invalid value for dynamic idr frame");
        return count;
    }
    dynamic_idr->idr_arr_idx = 0;
    dynamic_idr->len_idr_arr = count;

    qsort(dynamic_idr->idr_arr, dynamic_idr->len_idr_arr, sizeof(uint32_t),
          xlnx_utils_compare);

    return XMA_APP_SUCCESS;
}

/**
 * Parse expert options if present and save the values for respective keys
 * @param optargs: User's input string having the key:value pair
 * @param arguments: XlnxEncoderArguments structure to store the parsed values
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_enc_parse_expert_options(char*                 optargs,
                                         XlnxEncoderArguments* arguments)
{
    XlnxEncExpertOptions* expert_options = &arguments->expert_options;
    const char            delimiters[]   = "=";
    char*                 key            = strtok(optargs, delimiters);
    char*                 value          = NULL;
    expert_options->dynamic_params_check = 0;
    if(key != NULL) {
        if(!strcmp(key, "dynamic-params")) {
            value = strtok(NULL, delimiters);
            if(value != NULL) {
                expert_options->dynamic_params_file  = value;
                expert_options->dynamic_params_check = 1;
            } else {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Missing value for expert option with key=%s "
                           "in command line",
                           key);
                return XMA_APP_ERROR;
            }
        } else {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Incorrect expert option key %s in command line", key);
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_parse_args: Parses the command line arguments
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param enc_ctx: Encoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_parse_args(int32_t argc, char* argv[],
                                   XlnxEncoderArguments* arguments)
{
    XlnxEncoderProperties* enc_props = &arguments->enc_props;
    int32_t                flag;
    int32_t                option_index;
    int32_t                ret = INT32_MIN;
    while(1) {
        flag = getopt_long_only(argc, argv, "", enc_options, &option_index);
        if(flag == -1) {
            break;
        }
        switch(flag) {
            case HELP_ARG:
                printf("%s\n", xlnx_enc_get_help());
                exit(0);
            case DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->device_id, optarg,
                                             FLAG_DEVICE_ID);
                break;

            case INPUT_FILE_ARG:
                arguments->input_file = optarg;
                break;

            case OUTPUT_FILE_ARG:
                arguments->output_file = optarg;
                break;

            case NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&arguments->num_frames, optarg,
                                                FLAG_NUM_FRAMES);
                break;

            case LOOP_COUNT_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->loop_count, optarg,
                                             FLAG_STREAM_LOOP);
                break;

            case ENCODER_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264")) {
                    enc_props->codec_id = ENCODER_ID_H264;
                    /* Changing the default profile from HEVC_MAIN to
                       H264_HIGH for 264 encoding */
                    enc_props->profile = ENC_H264_HIGH;
                } else if(!strcmp(optarg, "mpsoc_vcu_hevc")) {
                    enc_props->codec_id = ENCODER_ID_HEVC;
                } else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                               "Unsupported codec %s\n", optarg);
                    return XMA_APP_ERROR;
                }
                break;

            case INPUT_WIDTH_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->width, optarg,
                                             FLAG_INPUT_WIDTH);
                break;

            case INPUT_HEIGHT_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->height, optarg,
                                             FLAG_INPUT_HEIGHT);
                break;

            case INPUT_PIX_FMT_ARG:
                ret = xlnx_utils_set_pix_fmt_arg(&arguments->pix_fmt, optarg,
                                                 FLAG_INPUT_PIX_FMT);
                enc_props->bits_per_pixel =
                    xlnx_utils_get_bit_depth_app_fmt(arguments->pix_fmt);
                enc_props->pix_fmt =
                    xlnx_utils_get_xma_vcu_format(enc_props->bits_per_pixel);
                break;

            case BITRATE_ARG:
                ret = xlnx_enc_get_br_in_kbps(&enc_props->bit_rate, optarg,
                                              FLAG_BITRATE);
                break;

            case FPS_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->fps, optarg, FLAG_FPS);
                break;

            case INTRA_PERIOD_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->gop_size, optarg,
                                             FLAG_INTRA_PERIOD);
                break;

            case DYNAMIC_IDR:
                ret = xlnx_enc_get_idr_frames(optarg, &arguments->dynamic_idr);
                break;
            case CONTROL_RATE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->control_rate, optarg,
                                             FLAG_CONTROL_RATE);
                break;

            case MAX_BITRATE_ARG:
                ret = xlnx_enc_get_br_in_kbps(&enc_props->max_bitrate, optarg,
                                              FLAG_MAX_BITRATE);
                break;

            case SLICE_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->slice_qp, optarg,
                                             FLAG_SLICE_QP);
                break;

            case MIN_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->min_qp, optarg,
                                             FLAG_MIN_QP);
                break;

            case MAX_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->max_qp, optarg,
                                             FLAG_MAX_QP);
                break;

            case NUM_BFRAMES_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->num_bframes, optarg,
                                              FLAG_NUM_BFRAMES);
                break;

            case IDR_PERIOD_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->idr_period, optarg,
                                             FLAG_IDR_PERIOD);
                break;

            case PROFILE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->profile, optarg, "");
                if(ret == XMA_APP_ERROR) {
                    if(enc_props->codec_id == ENCODER_ID_H264) {
                        ret = xlnx_enc_key_from_string(
                            enc_prof_h264_lookup, optarg, ENC_PROF_H264_KEYS);
                        if(ret == XMA_APP_ERROR) {
                            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                                       "Invalid H264 codec profile value %s\n",
                                       optarg);
                        } else {
                            enc_props->profile = ret;
                        }
                    } else {
                        ret = xlnx_enc_key_from_string(
                            enc_prof_hevc_lookup, optarg, ENC_PROF_HEVC_KEYS);
                        if(ret == XMA_APP_ERROR) {
                            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                                       "Invalid HEVC codec profile value %s\n",
                                       optarg);
                        } else {
                            enc_props->profile = ret;
                        }
                    }
                }
                break;

            case LEVEL_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->level, optarg,
                                             FLAG_LEVEL);
                break;

            case NUM_SLICES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_slices, optarg,
                                             FLAG_NUM_SLICES);
                break;

            case QP_MODE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->qp_mode, optarg,
                                             FLAG_QP_MODE);
                break;

            case ASPECT_RATIO_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->aspect_ratio, optarg,
                                             FLAG_ASPECT_RATIO);
                break;

            case SCALING_LIST_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->scal_list, optarg,
                                             FLAG_SCALING_LIST);
                break;

            case LOOKAHEAD_DEPTH_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->lookahead_depth,
                                             optarg, FLAG_LOOKAHEAD_DEPTH);
                break;

            case TEMPORAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->temporal_aq, optarg,
                                             FLAG_TEMPORAL_AQ);
                break;

            case SPATIAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq, optarg,
                                             FLAG_SPATIAL_AQ);
                break;

            case SPATIAL_AQ_GAIN_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq_gain,
                                             optarg, FLAG_SPATIAL_AQ_GAIN);
                break;

            case NUM_CORES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_cores, optarg,
                                             FLAG_NUM_CORES);
                break;

            case TUNE_METRICS_ARG:
                xlnx_utils_set_int_arg(&enc_props->tune_metrics, optarg,
                                       FLAG_TUNE_METRICS);
                break;

            case LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->latency_logging,
                                             optarg, FLAG_LATENCY_LOGGING);
                break;

            case EXPERT_OPTIONS_ARG:
                ret = xlnx_enc_parse_expert_options(optarg, arguments);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "ERROR: Failed to parse commandline.\n");
                return XMA_APP_ERROR;
        }

        if(ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
    }
    return ret;
}

/**
 * xlnx_enc_set_arg_defaults: Initializes the encoder arguments with default
 * values.
 *
 * @param enc_ctx: A pointer to the arguments struct
 * @return None
 */
static void xlnx_enc_set_arg_defaults(XlnxEncoderArguments* arguments)
{
    XlnxEncoderProperties* enc_props = &arguments->enc_props;

    /* Initialize the encoder parameters to default */
    arguments->loop_count  = 0;
    arguments->num_frames  = SIZE_MAX;
    arguments->pix_fmt     = XMA_APP_NV12_FMT_TYPE;
    arguments->input_file  = "";
    arguments->output_file = "";

    enc_props->device_id      = DEFAULT_DEVICE_ID;
    enc_props->codec_id       = -1;
    enc_props->width          = ENC_DEFAULT_WIDTH;
    enc_props->height         = ENC_DEFAULT_HEIGHT;
    enc_props->bits_per_pixel = ENC_DEFAULT_BITS_PER_PIXEL;
    enc_props->bit_rate       = ENC_DEFAULT_BITRATE;
    enc_props->fps            = ENC_DEFAULT_FRAMERATE;
    enc_props->gop_size       = ENC_DEFAULT_GOP_SIZE;
    enc_props->slice_qp       = -1;
    enc_props->control_rate   = 1;
    enc_props->custom_rc      = 0;
    enc_props->max_bitrate    = ENC_DEFAULT_MAX_BITRATE;
    enc_props->min_qp         = 0;
    enc_props->max_qp         = ENC_SUPPORTED_MAX_QP;
    enc_props->cpb_size       = 2.0;
    enc_props->initial_delay  = 1.0;
    enc_props->gop_mode       = 0;
    enc_props->gdr_mode       = 0;
    enc_props->num_bframes    = ENC_DEFAULT_NUM_B_FRAMES;
    enc_props->idr_period     = ENC_DEFAULT_IDR_PERIOD;

    /* Assigning the default profile as HEVC profile. If the codec option
       is H264, this will be updated */
    enc_props->profile                = ENC_HEVC_MAIN;
    enc_props->level                  = ENC_DEFAULT_LEVEL;
    enc_props->tier                   = 0;
    enc_props->num_slices             = 1;
    enc_props->qp_mode                = 1;
    enc_props->aspect_ratio           = 0;
    enc_props->lookahead_depth        = 0;
    enc_props->temporal_aq            = 1;
    enc_props->spatial_aq             = 1;
    enc_props->spatial_aq_gain        = ENC_DEFAULT_SPAT_AQ_GAIN;
    enc_props->scal_list              = 1;
    enc_props->filler_data            = 0;
    enc_props->dependent_slice        = 0;
    enc_props->slice_size             = 0;
    enc_props->entropy_mode           = 1;
    enc_props->loop_filter            = 1;
    enc_props->constrained_intra_pred = 0;
    enc_props->prefetch_buffer        = 1;
    enc_props->latency_logging        = 0;
    enc_props->enable_hw_buf          = 1;
    enc_props->num_cores              = 0;
    enc_props->tune_metrics           = 0;
    enc_props->pix_fmt                = XMA_VCU_NV12_FMT_TYPE;
}

void xlnx_enc_copy_enc_props_to_la(XlnxEncoderArguments* arguments)
{
    XlnxEncoderProperties*   enc_props = &arguments->enc_props;
    XlnxLookaheadProperties* la_props  = &arguments->la_props;
    la_props->xma_fmt_type             = enc_props->pix_fmt;
    la_props->framerate.numerator      = enc_props->fps;
    la_props->framerate.denominator    = 1;
    if(enc_props->codec_id == ENCODER_ID_H264) {
        la_props->codec_type = LOOKAHEAD_ID_H264;
    } else {
        la_props->codec_type = LOOKAHEAD_ID_HEVC;
    }
    la_props->codec_type     = enc_props->codec_id;
    la_props->device_id      = enc_props->device_id;
    la_props->width          = enc_props->width;
    la_props->height         = enc_props->height;
    la_props->bits_per_pixel = enc_props->bits_per_pixel;
    if(enc_props->gop_size <= 0) {
        la_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    } else {
        la_props->gop_size = enc_props->gop_size;
    }
    la_props->lookahead_depth   = enc_props->lookahead_depth;
    la_props->spatial_aq_mode   = enc_props->spatial_aq;
    la_props->temporal_aq_mode  = enc_props->temporal_aq;
    la_props->rate_control_mode = enc_props->custom_rc;
    la_props->spatial_aq_gain   = enc_props->spatial_aq_gain;
    la_props->num_bframes       = enc_props->num_bframes;
    la_props->latency_logging   = enc_props->latency_logging;
    la_props->enable_hw_buf     = 0;
}

/**
 * xlnx_enc_parser: Parses and initializes the encoder parameters
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param arguments: The arguments struct to be created.
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_get_arguments(int32_t argc, char* argv[],
                               XlnxEncoderArguments* arguments)
{
    if(argc < 2) {
        printf("%s\n", xlnx_enc_get_help());
        exit(XMA_APP_SUCCESS);
    }
    memset(arguments, 0, sizeof(XlnxEncoderArguments));
    /* Encoder context parameters initialization */
    xlnx_enc_set_arg_defaults(arguments);

    /* Parse the argumenst and update the structure */
    if(xlnx_enc_parse_args(argc, argv, arguments) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    if(xlnx_enc_validate_arguments(arguments) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}
