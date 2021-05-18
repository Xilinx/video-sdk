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

#include "xlnx_encoder.h"

static struct option enc_options[] =
{
    {FLAG_HELP,            no_argument,       0, HELP_ARG},
    {FLAG_DEVICE_ID,       required_argument, 0, DEVICE_ID_ARG},
    {FLAG_STREAM_LOOP,     required_argument, 0, LOOP_COUNT_ARG},
    {FLAG_INPUT_FILE,      required_argument, 0, INPUT_FILE_ARG},
    {FLAG_CODEC_TYPE,      required_argument, 0, ENCODER_ARG},
    {FLAG_INPUT_WIDTH,     required_argument, 0, INPUT_WIDTH_ARG},
    {FLAG_INPUT_HEIGHT,    required_argument, 0, INPUT_HEIGHT_ARG},
    {FLAG_INPUT_PIX_FMT,   required_argument, 0, INPUT_PIX_FMT_ARG},
    {FLAG_BITRATE,         required_argument, 0, BITRATE_ARG},
    {FLAG_BIT_RATE,        required_argument, 0, BITRATE_ARG},
    {FLAG_FPS,             required_argument, 0, FPS_ARG},
    {FLAG_INTRA_PERIOD,    required_argument, 0, INTRA_PERIOD_ARG},
    {FLAG_CONTROL_RATE,    required_argument, 0, CONTROL_RATE_ARG},
    {FLAG_MAX_BITRATE,     required_argument, 0, MAX_BITRATE_ARG},
    {FLAG_SLICE_QP,        required_argument, 0, SLICE_QP_ARG},
    {FLAG_MIN_QP,          required_argument, 0, MIN_QP_ARG},
    {FLAG_MAX_QP,          required_argument, 0, MAX_QP_ARG},
    {FLAG_NUM_BFRAMES,     required_argument, 0, NUM_BFRAMES_ARG},
    {FLAG_IDR_PERIOD,      required_argument, 0, IDR_PERIOD_ARG},
    {FLAG_PROFILE,         required_argument, 0, PROFILE_ARG},
    {FLAG_LEVEL,           required_argument, 0, LEVEL_ARG},
    {FLAG_NUM_SLICES,      required_argument, 0, NUM_SLICES_ARG},
    {FLAG_QP_MODE,         required_argument, 0, QP_MODE_ARG},
    {FLAG_ASPECT_RATIO,    required_argument, 0, ASPECT_RATIO_ARG},
    {FLAG_SCALING_LIST,    required_argument, 0, SCALING_LIST_ARG},
    {FLAG_LOOKAHEAD_DEPTH, required_argument, 0, LOOKAHEAD_DEPTH_ARG},
    {FLAG_TEMPORAL_AQ,     required_argument, 0, TEMPORAL_AQ_ARG},
    {FLAG_SPATIAL_AQ,      required_argument, 0, SPATIAL_AQ_ARG},
    {FLAG_SPATIAL_AQ_GAIN, required_argument, 0, SPATIAL_AQ_GAIN_ARG},
    {FLAG_QP,              required_argument, 0, QP_ARG},
    {FLAG_NUM_FRAMES,      required_argument, 0, NUM_FRAMES_ARG},
    {FLAG_NUM_CORES,       required_argument, 0, NUM_CORES_ARG},
    {FLAG_TUNE_METRICS,    required_argument, 0, TUNE_METRICS_ARG},
    {FLAG_LATENCY_LOGGING, required_argument, 0, LATENCY_LOGGING_ARG},
    {FLAG_OUTPUT_FILE,     required_argument, 0, OUTPUT_FILE_ARG},
    {0, 0, 0, 0}
};

XlnxEncProfileLookup enc_prof_h264_lookup[] =
{
    {"baseline",    ENC_H264_BASELINE},
    {"main",        ENC_H264_MAIN},
    {"high",        ENC_H264_HIGH}
};

XlnxEncProfileLookup enc_prof_hevc_lookup[] = 
{
    {"main",        ENC_HEVC_MAIN},
    {"main-intra",  ENC_HEVC_MAIN_INTRA}
};

#define ENC_PROF_H264_KEYS (sizeof(enc_prof_h264_lookup)/sizeof(XlnxEncProfileLookup))
#define ENC_PROF_HEVC_KEYS (sizeof(enc_prof_hevc_lookup)/sizeof(XlnxEncProfileLookup))

/*-----------------------------------------------------------------------------
xlnx_enc_get_help: Prints the list of supported arguments for encoder xma 
                  application

Return:
List of supported arguments
-----------------------------------------------------------------------------*/
char* xlnx_enc_get_help()
{

    return " XMA Encoder App Usage: \n\t"
        "./program [input options] -i input-file -c:v <codec-option> "
        " [encoder options] -o <output-file>\n\n" 
        "Arguments:\n\n"
        "\t--help                     Print this message and exit.\n"
        "\t-d <device-id>             Specify a device on which the encoder \n"
        "\t                           to run. Default: 0\n"
        "\t-frames <frame-count>      Number of frames to be processed.\n\n"
        "Input options:\n\n"
        "\t-stream_loop <loop-count>  Number of times to loop the input YUV \n"
        "\t                           file.\n"
        "\t-w <width>                 Width of YUV input. \n"
        "\t-h <height>                Height of YUV input. \n"
        "\t-pix_fmt <pixel-format>    Pixel format of the input file (yuv420p / nv12). \n"
        "\t                           Default input file format will be chosen as nv12. \n"
        "\t-i <input-file>            Name and path of input YUV file \n\n"
        "Codec option:\n"
        "\t-c:v <codec>               Encoder codec to be used. Supported \n"
        "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264 \n"
        "Encoder params: \n"
        "\t-b:v <bitrate>             Bitrate can be given in Kbps or Mbps \n"
        "\t                           or bits i.e., 5000000, 5000K, 5M. \n"
        "\t                           Default is 200kbps \n"
        "\t-fps <fps>                 Input frame rate. Default is 25. \n"
        "\t-g <intraperiod>           Intra period. Default is 12. \n"
        "\t-control-rate <mode>       Rate control mode. Supported are 0 \n"
        "\t                           to 3, default is 1.\n"
        "\t-max-bitrate <bitrate>     Maximum bit rate. Supported are 0 to \n"
        "\t                           350000000, default is 5000 \n"
        "\t-slice-qp <qp>             Slice QP. Supported are -1 to 51, \n"
        "\t                           default is -1 \n"
        "\t-min-qp <qp>               Minimum QP. Supported are 0 to 51, \n"
        "\t                           default is 0. \n"
        "\t-max-qp <qp>               Maximum QP. Supported values are 0 \n"
        "\t                           to 51, default is 51. \n"
        "\t-bf <frames>               Number of B frames. Supported are 0 \n"
        "\t                           to 7, default is 2. \n"
        "\t-periodicity-idr <value>   IDR picture frequency. Supported are \n"
        "\t                           0 to UINT32_MAX, default is \n"
        "\t                           UINT32_MAX. \n"
        "\t-profile <value>           Encoder profile. \n"
        "\t           For HEVC, supported are 0 or main and 1 or main-intra. \n"
        "\t                           Default is 0/main. \n"
        "\t                           ENC_HEVC_MAIN - 0 or main. \n"
        "\t                           ENC_HEVC_MAIN_INTRA - 1 or main-intra. \n"
        "\t           For H264, supported are 66 or baseline, 77 or main, \n"
        "\t                           100 or high, default is 100/high. \n"
        "\t                           ENC_H264_BASELINE - 66 or baseline. \n"
        "\t                           ENC_H264_MAIN - 77 or main. \n"
        "\t                           ENC_H264_HIGH - 100 or high. \n"
        "\t-level <value>             Encoder level. \n"
        "\t                           For HEVC, supported are 10 to 51, \n"
        "\t                           default is 50. \n"
        "\t                           For H264, supported are 10 to 52, \n"
        "\t                           default is 50. \n"
        "\t-slices <value>            Number of slices per frame. Supported \n"
        "\t                           are 1 to 68, default is 1. \n"
        "\t-qp-mode <mode>            QP mode. Supported are 0, 1, and 2, \n"
        "\t                           default is 1. \n"
        "\t-aspect-ratio <value>      Aspect ratio. Supported values are 0 \n"
        "\t                           to 3, default is 0. \n"
        "\t-scaling-list <0/1>        Scaling list. Enable/Disable, \n"
        "\t                           default enable. \n"
        "\t-lookahead-depth <value>   Lookahead depth. Supported are 0 to \n"
        "\t                           20, default is 0. \n"
        "\t-temporal-aq <0/1>         Temporal AQ. Enable/Disable, \n"
        "\t                           default disable. \n"
        "\t-spatial-aq <0/1>          Spatial AQ. Enable/Disable, \n"
        "\t                           default disable. \n"
        "\t-spatial-aq-gain <value>   Spatial AQ gain. Supported are 0 to \n"
        "\t                           100, default is 50. \n"
        "\t-cores <value>             Number of cores to use, supported are \n"
        "\t                           0 to 4, default is 0. \n"
        "\t-tune-metrics <0/1>        Tunes MPSoC H.264/HEVC encoder's video \n"
        "\t                           quality for objective metrics, default \n"
        "\t                           disable. \n"
        "\t-latency_logging <0/1>     Enable latency logging in syslog.\n"
        "\t-o <file>                  File to which output is written.\n";
}

/*-----------------------------------------------------------------------------
xlnx_enc_key_from_string: maps string input to the encoder profile value

Parameters:
enc_prof_lookuptable : encoder's lookup table
profile_key: encoder's profile in string format
enc_no_keys: number of keys in the lookup table

Return: profile value
-----------------------------------------------------------------------------*/
static int xlnx_enc_key_from_string(XlnxEncProfileLookup *enc_prof_lookuptable, 
                                    char *profile_key, int enc_no_keys)
{
    int cnt;
    if (profile_key == NULL)
        return ENC_APP_FAILURE;
    for (cnt = 0; cnt < enc_no_keys; cnt++)
    {
        XlnxEncProfileLookup enc_profile_key = enc_prof_lookuptable[cnt];
        if(strcmp(enc_profile_key.key, profile_key) == 0)
            return enc_profile_key.value;
    }

    return ENC_APP_FAILURE;

}

/*-----------------------------------------------------------------------------
xlnx_enc_validate_codec_arguments: Validates encoder codec arguments

Parameters:
enc_props: Encoder properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_validate_codec_arguments(XlnxEncoderProperties *enc_props)
{

    if(enc_props->bit_rate < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Invalid bit-rate param %d \n", enc_props->bit_rate);
        return ENC_APP_FAILURE;
    }

    if((enc_props->fps <= 0) || (enc_props->fps > INT_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid frame rate %d \n", enc_props->fps);
        return ENC_APP_FAILURE;
    }

    if((enc_props->gop_size < 0) || (enc_props->gop_size > INT_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid intra period %d \n", enc_props->gop_size);
        return ENC_APP_FAILURE;
    }

    if((enc_props->control_rate < 0) || (enc_props->control_rate > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Invalid rate control mode %d \n", enc_props->control_rate);
        return ENC_APP_FAILURE;
    }

    if((enc_props->max_bitrate < 0) || 
            (enc_props->max_bitrate > ENC_SUPPORTED_MAX_BITRATE)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid max btirate %ld \n", enc_props->max_bitrate);
        return ENC_APP_FAILURE;
    }

    if((enc_props->slice_qp < -1) || 
       (enc_props->slice_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid slice qp %d \n", enc_props->slice_qp);
        return ENC_APP_FAILURE;
    }

    if((enc_props->min_qp < ENC_SUPPORTED_MIN_QP) || 
            (enc_props->min_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid min qp %d \n", 
                enc_props->min_qp);
        return ENC_APP_FAILURE;
    }

    if((enc_props->max_qp < ENC_SUPPORTED_MIN_QP) || 
            (enc_props->max_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid max qp %d \n", 
                enc_props->max_qp);
        return ENC_APP_FAILURE;
    }

    if((enc_props->qp_mode < 0) || (enc_props->qp_mode > 2)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Invalid qp mode %d \n", 
                enc_props->qp_mode);
        return ENC_APP_FAILURE;
    }

    if((enc_props->idr_period < 0) || (enc_props->idr_period > UINT32_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid IDR period %d \n", enc_props->idr_period);
        return ENC_APP_FAILURE;
    }

    if((enc_props->gop_size < 0) || (enc_props->gop_size > INT_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid GOP size %d \n", enc_props->gop_size);
        return ENC_APP_FAILURE;
    }

    if((enc_props->num_bframes < 0) || (enc_props->num_bframes > INT_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid number of B frames %d \n", enc_props->num_bframes);
        return ENC_APP_FAILURE;
    }

    if((enc_props->aspect_ratio < 0) || (enc_props->aspect_ratio > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid aspect ratio %d \n", enc_props->aspect_ratio);
        return ENC_APP_FAILURE;
    }

    if((enc_props->scaling_list != 0) && (enc_props->scaling_list != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Invalid scaling list value %d \n", enc_props->scaling_list);
        return ENC_APP_FAILURE;
    }

    if((enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH) || 
            (enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Invalid LA depth %d \n", enc_props->lookahead_depth);
        return ENC_APP_FAILURE;
    }

    if((enc_props->spatial_aq_gain < 0) || 
            (enc_props->spatial_aq_gain > ENC_MAX_SPAT_AQ_GAIN)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Invalid spatial aq gain %d \n", enc_props->spatial_aq_gain);
        return ENC_APP_FAILURE;
    }

    if((enc_props->num_cores < 0) && (enc_props->num_cores > 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Invalid number of cores %d \n", enc_props->num_cores);
        return ENC_APP_FAILURE;
    }

    if((enc_props->tune_metrics != 0) && (enc_props->tune_metrics != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
            "Invalid value for Tune metrics %d \n", enc_props->tune_metrics);
        return ENC_APP_FAILURE;
    }

    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_enc_validate_arguments: Validates generic encoder app arguments

Parameters:
enc_ctx: Encoder context

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_validate_arguments(XlnxEncoderCtx *enc_ctx)
{

    int32_t ret = ENC_APP_FAILURE;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    if(enc_ctx->in_file == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Error opening input file \n");
        return ENC_APP_FAILURE;
    }

    if(enc_ctx->out_file == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Error opening output file \n");
        return ENC_APP_FAILURE;
    }

    if (enc_props->lookahead_depth)
    {
        if((enc_props->width > ENC_MAX_LA_INPUT_WIDTH) || 
            (enc_props->height > ENC_MAX_LA_INPUT_WIDTH) ||
            ((enc_props->width * enc_props->height) > ENC_MAX_LA_PIXELS)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Input to encoder for lookahead usecase %dx%d exceeds maximum supported resolution %dx%d \n", 
                    enc_props->width, enc_props->height, ENC_MAX_LA_INPUT_WIDTH, ENC_MAX_LA_INPUT_HEIGHT);
            return ENC_APP_FAILURE;
        }
    }
    else
    {
        if((enc_props->width > ENC_SUPPORTED_MAX_WIDTH) || 
            (enc_props->height > ENC_SUPPORTED_MAX_WIDTH) ||
            ((enc_props->width * enc_props->height) > ENC_SUPPORTED_MAX_PIXELS)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "Input resolution %dx%d exceeds maximum supported resolution %dx%d \n", 
                    enc_props->width, enc_props->height, ENC_SUPPORTED_MAX_WIDTH, ENC_SUPPORTED_MAX_HEIGHT);
            return ENC_APP_FAILURE;
        }
    }

    if((enc_props->width < ENC_SUPPORTED_MIN_WIDTH) || (enc_props->width%4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Unsupported width %d \n", enc_props->width);
        return ENC_APP_FAILURE;
    }

    if((enc_props->height < ENC_SUPPORTED_MIN_HEIGHT) || (enc_props->height%4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Unsupported height %d \n", enc_props->height);
        return ENC_APP_FAILURE;
    }

    if(enc_ctx->loop_count < -1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "ERROR: stream_loop value of %d is invalid! "
                "0 <= stream_loop <= MAX_INT. -1 for infinite loop.\n", 
                enc_ctx->loop_count);
        return ENC_APP_FAILURE;
    }

    if((enc_ctx->num_frames <= 0) || (enc_ctx->num_frames > SIZE_MAX)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
            "Invalid number of frames to encode %zu \n", enc_ctx->num_frames);
        return ENC_APP_FAILURE;
    }

    ret = xlnx_enc_validate_codec_arguments(enc_props);
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_get_br_in_kbps: Get value of bit rate
Parameters:
    desination: Where to store the bitrate
    source: User input value
    param_name: Name of the parameter
Return:
    ENC_APP_SUCCESS if success
    ENC_APP_FAILURE if failure
-----------------------------------------------------------------------------*/
static int xlnx_enc_get_br_in_kbps(int64_t* destination, char* source, 
                                     char* param_name)
{
    float br_in_kbps = atof(source);
    if(xlnx_utils_check_if_pattern_matches("^-?[0-9]*\\.?[0-9]+[M|m|K|k]*$", 
       source) == 0) {
       
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, "Unrecognized value "
                   "\"%s\" for argument -%s! Make sure the value is of proper "
                   "type.\n", source, param_name);
        return ENC_APP_FAILURE;
    }

    if(xlnx_utils_check_if_pattern_matches("[M|m]+", source)) {
        *destination = br_in_kbps * 1000;
    }
    else if(xlnx_utils_check_if_pattern_matches("[K|k]+", source)) {
        *destination = br_in_kbps;
    }
    else {
        *destination = (br_in_kbps / 1000);
    }
    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_parse_args: Parses the command line arguments

Parameters:
argc: Number of arguments
argv: Pointer to the arguments
enc_ctx: Encoder context

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_parse_args(int32_t argc, char* argv[], 
        XlnxEncoderCtx *enc_ctx)
{
    int32_t flag;
    int32_t option_index;
    int32_t ret = ENC_APP_FAILURE;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;
    while(1)
    {
        flag = getopt_long_only(argc, argv, "", enc_options, &option_index);
        if(flag == -1) {
            break;
        }

        switch (flag)
        {
            case HELP_ARG:
                printf( "%s\n", xlnx_enc_get_help());
                exit(0);

            case DEVICE_ID_ARG:
                xlnx_utils_set_int_arg(&enc_ctx->enc_xrm_ctx.device_id, optarg,
                                       FLAG_DEVICE_ID);
                enc_ctx->enc_xrm_ctx.device_id = atoi(optarg);
                if((enc_ctx->enc_xrm_ctx.device_id < 0) || 
                    (enc_ctx->enc_xrm_ctx.device_id > 15)) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                               "Unsupported device ID %d \n", 
                               enc_ctx->enc_xrm_ctx.device_id);
                    return ENC_APP_FAILURE;
                }
                break;

            case LOOP_COUNT_ARG:
                ret = xlnx_utils_set_int_arg(&enc_ctx->loop_count, optarg, 
                                        FLAG_STREAM_LOOP);
                break;

            case INPUT_FILE_ARG:
                enc_ctx->in_file = fopen(optarg, "r");
                break;

            case OUTPUT_FILE_ARG:
                enc_ctx->out_file = fopen(optarg, "w");
                break;

            case ENCODER_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264")) {
                    enc_props->codec_id = ENCODER_ID_H264;
                    /* Changing the default profile from HEVC_MAIN to 
                       H264_HIGH for 264 encoding */
                    enc_props->profile = ENC_H264_HIGH;
                }
                else if(!strcmp(optarg, "mpsoc_vcu_hevc")) {
                    enc_props->codec_id = ENCODER_ID_HEVC;
                }
                else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                            "Unsupported codec %s \n", optarg);
                    return ENC_APP_FAILURE;
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
                if(!strcmp(optarg, "yuv420p"))
                    enc_props->pix_fmt = YUV_420P_ID;
                else if(!strcmp(optarg, "nv12"))
                    enc_props->pix_fmt = YUV_NV12_ID;
                else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                            "Unsupported pixel format option %s \n", optarg);
                    return ENC_APP_FAILURE;
                }
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

            case CONTROL_RATE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->control_rate, optarg, 
                                             FLAG_CONTROL_RATE);
                break;

            case MAX_BITRATE_ARG:
                ret =  xlnx_enc_get_br_in_kbps(&enc_props->max_bitrate, optarg, 
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
                ret = xlnx_utils_set_int_arg(&enc_props->max_qp , optarg, 
                                             FLAG_MAX_QP);
                break;

            case NUM_BFRAMES_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->num_bframes, optarg, 
                                              FLAG_NUM_BFRAMES);
                break;

            case IDR_PERIOD_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->idr_period, optarg, 
                                              FLAG_IDR_PERIOD);
                break;

            case PROFILE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->profile, optarg, "");
                if (ret == ENC_APP_FAILURE) {
                    if(enc_props->codec_id == ENCODER_ID_H264) {
                        ret = xlnx_enc_key_from_string(enc_prof_h264_lookup, 
                                                       optarg, ENC_PROF_H264_KEYS);
                        if (ret == ENC_APP_FAILURE)
                            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                                        "Invalid H264 codec profile value %s \n", 
                                        optarg);
                        else
                            enc_props->profile = ret;
                    }
                    else {
                        ret = xlnx_enc_key_from_string(enc_prof_hevc_lookup, 
                                                        optarg, ENC_PROF_HEVC_KEYS);
                        if (ret == ENC_APP_FAILURE)
                            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                                        "Invalid HEVC codec profile value %s \n", 
                                        optarg);
                        else
                            enc_props->profile = ret;
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
                ret = xlnx_utils_set_int_arg(&enc_props->scaling_list, optarg, 
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

            case NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&enc_ctx->num_frames, optarg, 
                                              FLAG_NUM_FRAMES);
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

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                        "ERROR: Failed to parse commandline.\n");
                return ENC_APP_FAILURE;
        }

        if(ret == ENC_APP_FAILURE)
            return ENC_APP_FAILURE;
    }

    ret = xlnx_enc_validate_arguments(enc_ctx);
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_update_props: Updates xma encoder properties and options that will be
                       sent to xma plugin

Parameters:
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_update_props(XlnxEncoderCtx *enc_ctx, 
                                     XmaEncoderProperties *xma_enc_props)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;
    enc_props->enc_options = calloc(1, ENC_MAX_OPTIONS_SIZE);

    /* Enable custom rate control when rate control is set to CBR and 
    lookahead is set, disable when expert option lookahead-rc-off is set. */
    if((enc_props->control_rate == 1) && (enc_props->lookahead_depth > 1)) {
        enc_props->custom_rc = 1;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if (enc_props->lookahead_depth >= 1 && (enc_props->temporal_aq == 1 || 
                enc_props->spatial_aq == 1) && (enc_props->tune_metrics == 0)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting qp mode to 2, as the lookahead params are set \n");
        enc_props->qp_mode = 2;
    }
    else if ((enc_props->lookahead_depth == 0) || 
            (enc_props->tune_metrics == 1)) {
        if (enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if (enc_props->spatial_aq)
            enc_props->spatial_aq = 0;
    }

    /* Tunes video quality for objective scores by setting flat scaling-list 
       and uniform qp-mode */
    if (enc_props->tune_metrics){
        enc_props->scaling_list = 0;
        enc_props->qp_mode = 0;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if (enc_props->lookahead_depth >= 1 && (enc_props->temporal_aq == 1 || 
        enc_props->spatial_aq == 1)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting qp mode to 2, as the lookahead params are set \n");
        enc_props->qp_mode = 2;
    }
    else if (enc_props->lookahead_depth == 0) {
        if (enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if (enc_props->spatial_aq)
            enc_props->spatial_aq = 0;

        enc_props->enable_hw_buf = 0;
    }

    /* Set IDR period to gop-size, when the user has not specified it on 
       the command line */
    if (enc_props->idr_period == -1)
    {
        if (enc_props->gop_size > 0){
            enc_props->idr_period = enc_props->gop_size;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE, 
                "Setting IDR period to GOP size \n");
    }

    return xlnx_enc_get_xma_props(enc_props, xma_enc_props);
}

/*-----------------------------------------------------------------------------
xlnx_enc_frame_init: Allocates and initialize XMA frame for encoder 

Parameters:
enc_ctx: Encoder context

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_enc_frame_init(XlnxEncoderCtx *enc_ctx)
{

    XmaFrameProperties *frame_props = &(enc_ctx->in_frame.frame_props);
    frame_props->format = XMA_VCU_NV12_FMT_TYPE;
    frame_props->width  = enc_ctx->enc_props.width;
    frame_props->height = enc_ctx->enc_props.height;
    frame_props->linesize[0] = enc_ctx->enc_props.width;
    frame_props->linesize[1] = enc_ctx->enc_props.width;
    frame_props->bits_per_pixel = 8;

    return;
}

/*-----------------------------------------------------------------------------
xlnx_enc_context_init: Initializes the encoder context with default values.

Parameters:
enc_ctx: Encoder context

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_enc_context_init(XlnxEncoderCtx *enc_ctx)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    /* Initialize the encoder parameters to default */
    enc_ctx->enc_xrm_ctx.device_id = -1;
    enc_ctx->enc_xrm_ctx.enc_res_idx = -1;
    enc_ctx->enc_xrm_ctx.enc_res_in_use = 0;
    enc_ctx->enc_xrm_ctx.lookahead_res_inuse = 0;

    enc_ctx->loop_count = 0;
    enc_ctx->num_frames = SIZE_MAX;
    enc_props->codec_id = -1;
    enc_props->width = ENC_DEFAULT_WIDTH;
    enc_props->height = ENC_DEFAULT_HEIGHT;
    enc_props->bit_rate = ENC_DEFAULT_BITRATE;
    enc_props->fps = ENC_DEFAULT_FRAMERATE;
    enc_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    enc_props->slice_qp = -1;
    enc_props->control_rate = 1;
    enc_props->custom_rc = 0;
    enc_props->max_bitrate = ENC_DEFAULT_MAX_BITRATE;
    enc_props->min_qp = 0;
    enc_props->max_qp = ENC_SUPPORTED_MAX_QP;
    enc_props->cpb_size = 2.0;
    enc_props->initial_delay = 1.0;
    enc_props->gop_mode = 0;
    enc_props->gdr_mode = 0;
    enc_props->num_bframes = ENC_DEFAULT_NUM_B_FRAMES;
    enc_props->idr_period = -1;

    /* Assigning the default profile as HEVC profile. If the codec option 
       is H264, this will be updated */
    enc_props->profile = ENC_HEVC_MAIN;
    enc_props->level = ENC_DEFAULT_LEVEL;
    enc_props->tier = 0;
    enc_props->num_slices = 1;
    enc_props->qp_mode = 1;
    enc_props->aspect_ratio = 0;
    enc_props->lookahead_depth = 0;
    enc_props->temporal_aq = 1;
    enc_props->spatial_aq = 1;
    enc_props->spatial_aq_gain = ENC_DEFAULT_SPAT_AQ_GAIN;
    enc_props->scaling_list = 1;
    enc_props->filler_data = 0;
    enc_props->dependent_slice = 0;
    enc_props->slice_size = 0;
    enc_props->entropy_mode = 1;
    enc_props->loop_filter = 1;
    enc_props->constrained_intra_pred = 0;
    enc_props->prefetch_buffer = 1;
    enc_props->latency_logging = 0;
    enc_props->enable_hw_buf = 1;
    enc_props->num_cores = 0;
    enc_props->tune_metrics = 0;

    enc_ctx->pts = 0;
    enc_ctx->out_frame_cnt = 0;
    enc_ctx->in_frame_cnt = 0;
    enc_ctx->enc_state = ENC_READ_INPUT;
    enc_ctx->la_in_frame = &(enc_ctx->in_frame);
    enc_ctx->enc_in_frame = &(enc_ctx->in_frame);

}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc_device_id: Allocates CU for encoder based on device 
                                  index

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder XMA properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_xlnx_enc_cu_alloc_device_id(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                        XmaEncoderProperties *xma_enc_props)
{

    xrmCuProperty encode_cu_hw_prop, encode_cu_sw_prop;

    int32_t ret = -1;

    memset(&encode_cu_hw_prop, 0, sizeof(xrmCuProperty));
    memset(&encode_cu_sw_prop, 0, sizeof(xrmCuProperty));
    memset(&enc_xrm_ctx->encode_cu_list_res, 0, sizeof(xrmCuListResource));

    strcpy(encode_cu_hw_prop.kernelName, "encoder");
    strcpy(encode_cu_hw_prop.kernelAlias, "ENCODER_MPSOC");
    encode_cu_hw_prop.devExcl = false;
    encode_cu_hw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);

    strcpy(encode_cu_sw_prop.kernelName, "kernel_vcu_encoder");
    encode_cu_sw_prop.devExcl = false;
    encode_cu_sw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);

    ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->device_id, 
            &encode_cu_hw_prop, &enc_xrm_ctx->encode_cu_list_res.cuResources[0]);

    if (ret <= ENC_APP_FAILURE)
    {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "xrm failed to allocate encoder resources on device %d\n", 
                   enc_xrm_ctx->device_id);
        return ret;
    }
    else
    {
        ret = xrmCuAllocFromDev(enc_xrm_ctx->xrm_ctx, enc_xrm_ctx->device_id, 
                &encode_cu_sw_prop, &enc_xrm_ctx->encode_cu_list_res.cuResources[1]);
        if (ret <= ENC_APP_FAILURE)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                    "xrm failed to allocate encoder resources on device %d\n", 
                    enc_xrm_ctx->device_id);
            return ret;
        }
    }

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].cuId;
    xma_enc_props->channel_id = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].channelId;

    enc_xrm_ctx->enc_res_in_use = 1;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc_reserve_id: Allocates CU for encoder based on 
                                   reservation index

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder XMA properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_xlnx_enc_cu_alloc_reserve_id(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                        XmaEncoderProperties *xma_enc_props)
{

    int32_t ret = ENC_APP_FAILURE;

    /* XRM encoder allocation */
    xrmCuListProperty encode_cu_list_prop;

    memset(&encode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(&enc_xrm_ctx->encode_cu_list_res, 0, sizeof(xrmCuListResource));

    encode_cu_list_prop.cuNum = 2;
    strcpy(encode_cu_list_prop.cuProps[0].kernelName, "encoder");
    strcpy(encode_cu_list_prop.cuProps[0].kernelAlias, "ENCODER_MPSOC");
    encode_cu_list_prop.cuProps[0].devExcl = false;
    encode_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_xrm_ctx->enc_load);
    encode_cu_list_prop.cuProps[0].poolId = enc_xrm_ctx->enc_res_idx;

    strcpy(encode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_encoder");
    encode_cu_list_prop.cuProps[1].devExcl = false;
    encode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    encode_cu_list_prop.cuProps[1].poolId = enc_xrm_ctx->enc_res_idx;

    ret = xrmCuListAlloc(enc_xrm_ctx->xrm_ctx, &encode_cu_list_prop, 
            &enc_xrm_ctx->encode_cu_list_res);
    if (ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Failed to allocate encoder cu from reserve id \n");
        return ret;
    }

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].cuId;
    xma_enc_props->channel_id = 
        enc_xrm_ctx->encode_cu_list_res.cuResources[1].channelId;

    enc_xrm_ctx->enc_res_in_use = 1;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc: Allocates CU for encoder

Parameters:
enc_xrm_ctx: Encoder XRM context
xma_enc_props: Encoder XMA properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_cu_alloc(XlnxEncoderXrmCtx *enc_xrm_ctx, 
                                 XmaEncoderProperties *xma_enc_props)
{

    int32_t ret = ENC_APP_FAILURE;

    if(enc_xrm_ctx->device_id >= 0) {
        ret = xlnx_xlnx_enc_cu_alloc_device_id(enc_xrm_ctx, xma_enc_props);
    }
    else {
        ret = xlnx_xlnx_enc_cu_alloc_reserve_id(enc_xrm_ctx, xma_enc_props);
    }

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_parser: Parses and initializes the encoder parameters

Parameters:
argc: Number of arguments
argv: Pointer to the arguments
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_parser(int32_t argc, char *argv[], XlnxEncoderCtx *enc_ctx,
                        XmaEncoderProperties *xma_enc_props)
{

    /* Encoder context parameters initialization */
    xlnx_enc_context_init(enc_ctx);

    /* Parse the argumenst and update the structure */
    if(xlnx_enc_parse_args(argc, argv, enc_ctx) != ENC_APP_SUCCESS)
        return ENC_APP_FAILURE;

    /* Update the xma encoder properties */
    if(xlnx_enc_update_props(enc_ctx, xma_enc_props) != ENC_APP_SUCCESS)
        return ENC_APP_FAILURE;

    /* Initializing xma frame */
    xlnx_enc_frame_init(enc_ctx);

    return ENC_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_enc_la_init: Lookahead module initialization

Parameters:
enc_ctx: Encoder context
xma_la_props: XMA lookahead filter properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_la_init(XlnxEncoderCtx *enc_ctx, 
                         XmaFilterProperties  *xma_la_props)
{
    int32_t ret = ENC_APP_FAILURE;
    XlnxLookaheadProperties *la_props = &enc_ctx->la_ctx.la_props;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    la_props->width = enc_props->width;
    la_props->height = enc_props->height;
    la_props->framerate.numerator = enc_props->fps;
    la_props->framerate.denominator = 1;

    //TODO: Assume 256 aligned for now. Needs to be fixed later
    la_props->stride = XLNX_ENC_LINE_ALIGN(enc_props->width, VCU_STRIDE_ALIGN);
    la_props->bits_per_pixel = 8;

    if (enc_props->gop_size <= 0) {
        la_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    } else {
        la_props->gop_size = enc_props->gop_size;
    }

    la_props->lookahead_depth = enc_props->lookahead_depth;
    la_props->spatial_aq_mode = enc_props->spatial_aq;
    la_props->spatial_aq_gain = enc_props->spatial_aq_gain;
    la_props->temporal_aq_mode = enc_props->temporal_aq;
    la_props->rate_control_mode = enc_props->custom_rc;
    la_props->num_bframes = enc_props->num_bframes;
    la_props->latency_logging = enc_props->latency_logging;

    /* Only NV12 format is supported in this application */
    la_props->xma_fmt_type = XMA_VCU_NV12_FMT_TYPE;
    la_props->enable_hw_buf = 0;

    switch (enc_props->codec_id) {
        case ENCODER_ID_H264:
            la_props->codec_type = LOOKAHEAD_ID_H264;
            break;
        case ENCODER_ID_HEVC:
            la_props->codec_type = LOOKAHEAD_ID_HEVC;
            break;
    }

    ret = xlnx_la_create(&enc_ctx->la_ctx, &enc_ctx->enc_xrm_ctx, xma_la_props);
    if (ret != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Error : init_la : create_xlnx_la Failed \n");
        return ENC_APP_FAILURE;
    }

    enc_ctx->la_bypass = xlnx_la_get_bypass_mode(&enc_ctx->la_ctx);

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_create_session: Creates encoder session

Parameters:
enc_ctx: Encoder context
xma_enc_props: Encoder properties

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_create_session(XlnxEncoderCtx *enc_ctx, 
                                XmaEncoderProperties *xma_enc_props)
{

    XlnxEncoderXrmCtx *enc_xrm_ctx = &enc_ctx->enc_xrm_ctx;
    if(xlnx_enc_cu_alloc(enc_xrm_ctx, xma_enc_props) != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                "Error in encoder CU allocation \n");
        return ENC_APP_FAILURE;
    }

    /* Encoder session creation */
    enc_ctx->enc_session = xma_enc_session_create(xma_enc_props);
    if(enc_ctx->enc_session == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Encoder session creation failed \n");
        return ENC_APP_FAILURE;
    }

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_read_y_component: Read y component of YUV frame

Parameters:
enc_ctx: Encoder context
xma_frame: XMA frame that holds Y and UV buffers

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
uint32_t xlnx_enc_read_y_component(XlnxEncoderCtx *enc_ctx,
                        XmaFrame *xma_frame, uint32_t frame_size_y)
{
    int32_t read_size = 0;

    read_size = fread(xma_frame->data[0].buffer, sizeof(char), frame_size_y,
                    enc_ctx->in_file);

    if(read_size != frame_size_y) {
        if (enc_ctx->loop_count-- > 0) {
            fseek (enc_ctx->in_file, 0, SEEK_SET);
            read_size = fread(xma_frame->data[0].buffer, sizeof(char),
                            frame_size_y, enc_ctx->in_file);
        }

        if(read_size == frame_size_y)
            return ENC_APP_SUCCESS;

        free(xma_frame->data[0].buffer);
        free(xma_frame->data[1].buffer);
        return ENC_APP_FAILURE;
    }

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_read_u_v_component: Read u and v component of YUV frame

Parameters:
enc_ctx: Encoder context
xma_frame: XMA frame that holds Y and UV buffers

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
uint32_t xlnx_enc_read_u_v_component(XlnxEncoderCtx *enc_ctx,
                                    XmaFrame *xma_frame)
{
    uint32_t frame_size_uv = (enc_ctx->enc_props.width *
                                enc_ctx->enc_props.height)/2;
    int32_t read_size = 0;

    if(enc_ctx->enc_props.pix_fmt == YUV_420P_ID)
    {
        char *frame_u, *frame_v, *frame_uv, *frame_chroma_data;

        frame_chroma_data = (char*)calloc(sizeof(char), frame_size_uv);

        read_size = fread(frame_chroma_data, sizeof(char), frame_size_uv,
                enc_ctx->in_file);
        if(read_size != frame_size_uv) {
            free(xma_frame->data[0].buffer);
            free(xma_frame->data[1].buffer);
            free(frame_chroma_data);
            return ENC_APP_FAILURE;
        }

        frame_u = frame_chroma_data;
        frame_v = frame_chroma_data + (frame_size_uv/2);

        frame_uv = xma_frame->data[1].buffer;
        for (int i = 0; i < (frame_size_uv/2); i++) {
            frame_uv[2*i] = frame_u[i];
            frame_uv[2*i +1] = frame_v[i];
        }

        free(frame_chroma_data);
    }

    else if(enc_ctx->enc_props.pix_fmt == YUV_NV12_ID)
    {
        read_size = fread(xma_frame->data[1].buffer, sizeof(char), frame_size_uv,
                enc_ctx->in_file);
        if(read_size != frame_size_uv) {
            free(xma_frame->data[0].buffer);
            free(xma_frame->data[1].buffer);
            return ENC_APP_FAILURE;
        }
    }

     return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_read_frame: Reads a YUV frame data from input file and updates
                     xma_frame.

Parameters:
enc_ctx: Encoder context
xma_frame: XMA frame that holds Y and UV buffers

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_read_frame(XlnxEncoderCtx *enc_ctx, XmaFrame *xma_frame)
{

    uint32_t ret = ENC_APP_SUCCESS;
    uint32_t frame_size_y = (enc_ctx->enc_props.width *
                                enc_ctx->enc_props.height);

    xma_frame->data[0].refcount = 1;
    xma_frame->data[0].buffer_type = XMA_HOST_BUFFER_TYPE;
    xma_frame->data[0].is_clone = false;
    xma_frame->data[0].buffer = calloc(1, frame_size_y);
    xma_frame->data[1].refcount = 1;
    xma_frame->data[1].buffer_type = XMA_HOST_BUFFER_TYPE;
    xma_frame->data[1].is_clone = false;
    xma_frame->data[1].buffer = calloc(1, (frame_size_y)/2);

    ret = xlnx_enc_read_y_component(enc_ctx, xma_frame, frame_size_y);
        if (ret == ENC_APP_FAILURE)
            return ret;

    ret = xlnx_enc_read_u_v_component(enc_ctx, xma_frame);
        if (ret == ENC_APP_FAILURE)
            return ret;

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_frame_process: Process an input YUV frame

Parameters:
enc_ctx: Encoder context
enc_stop: Encoder stop flag

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_frame_process(XlnxEncoderCtx *enc_ctx, int32_t *enc_stop)
{

    int32_t ret = ENC_APP_FAILURE;
    XmaFrame *xma_frame = &(enc_ctx->in_frame);

    if(*enc_stop) {
        enc_ctx->enc_state = ENC_STOP;
        *enc_stop = 0;
    }

    switch(enc_ctx->enc_state) {

        case ENC_READ_INPUT:
            ret = xlnx_enc_read_frame(enc_ctx, xma_frame);
            if(ret == ENC_APP_FAILURE) {
                /* Once the input file read is completed, encoder flush will be 
                   triggered */
                enc_ctx->enc_state = ENC_EOF;
            }
            else {
                if(!enc_ctx->la_bypass)
                    enc_ctx->enc_state = ENC_LA_PROCESS;
                else
                    enc_ctx->enc_state = ENC_SEND_INPUT;

                /* PTS is not used in the encoder application, but xma 
                   plugin expects PTS from the application */
                xma_frame->pts = enc_ctx->pts++;
                if(enc_ctx->in_frame_cnt >= enc_ctx->num_frames) {
                    enc_ctx->enc_state = ENC_EOF;
                }
                enc_ctx->in_frame_cnt++;
            }
            break;

        case ENC_LA_PROCESS:
        case ENC_LA_FLUSH:
            if((ret = xlnx_la_process_frame(&enc_ctx->la_ctx, 
             enc_ctx->la_in_frame, &enc_ctx->enc_in_frame)) == XMA_SUCCESS) {
                enc_ctx->enc_state = ENC_SEND_INPUT;
            }
            else if (ret <= XMA_ERROR) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                        "LA frame process failed with error %d \n", ret);
                return ENC_APP_DONE;
            }
            else if ((ret == XMA_SEND_MORE_DATA) && 
                    (enc_ctx->la_in_frame != NULL)) {
                enc_ctx->enc_state = ENC_READ_INPUT;
            }
            else {
                enc_ctx->enc_state = ENC_EOF;
                /* Once all the LA frames are flushed, LA pipeline will be 
                   skipped and encoder flush will be started. */
                if(ret == XMA_EOS) {
                    enc_ctx->la_bypass = 1;
                }
            }
            /* The luma and chroma buffers to be freed once LA process them */
            if(enc_ctx->la_in_frame != NULL) { 
                free(enc_ctx->la_in_frame->data[0].buffer);
                free(enc_ctx->la_in_frame->data[1].buffer);
            }
            break;

        case ENC_SEND_INPUT:
            ret = xma_enc_session_send_frame(enc_ctx->enc_session, 
                    enc_ctx->enc_in_frame);
            if (ret == XMA_SUCCESS) {
                enc_ctx->enc_state = ENC_GET_OUTPUT;
            }
            else if(ret == XMA_SEND_MORE_DATA) {
                enc_ctx->enc_state = ENC_READ_INPUT;
            }
            else {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                        "Encoder send frame failed: %d !!\n", ret);
                return ENC_APP_DONE;
            }
            if (enc_ctx->enc_in_frame) {
                /* Release the LA output frame(encoder input frame), once 
                   encoder consumes it. This will be reused by LA module */
                if(!enc_ctx->la_bypass)
                    xlnx_la_release_frame(&enc_ctx->la_ctx, 
                                         enc_ctx->enc_in_frame);
            }
            else {
                enc_ctx->enc_state = ENC_FLUSH;
            }
            break;

        case ENC_GET_OUTPUT:
        case ENC_FLUSH:
            {
                int32_t recv_size = 0;
                ret = xma_enc_session_recv_data(enc_ctx->enc_session, 
                        &(enc_ctx->xma_buffer), &recv_size);
                if(enc_ctx->enc_state != ENC_FLUSH)
                    enc_ctx->enc_state = ENC_READ_INPUT;

                if (ret == XMA_SUCCESS) {
                    /* Encoder output frame received */
                    fwrite(enc_ctx->xma_buffer.data.buffer, sizeof(char), 
                           recv_size, enc_ctx->out_file);
                    enc_ctx->out_frame_cnt++;
                    recv_size = 0;
                } 
                else if(ret == XMA_EOS) {
                    return ENC_APP_DONE;
                }
                else if(ret <= XMA_ERROR) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                            "Encoder receive frame failed: %d \n", ret);
                    return ENC_APP_DONE;
                }
            }
            break;

        case ENC_EOF:
            /* If LA is enabled and active, trigger LA flush, followed by  
               encoder flush */
            if(!enc_ctx->la_bypass) {
                enc_ctx->enc_state = ENC_LA_FLUSH;
                enc_ctx->la_in_frame = NULL;
            }
            else {
                xma_frame->is_last_frame = 1;
                xma_frame->pts = -1;
                ret = xma_enc_session_send_frame(enc_ctx->enc_session, 
                                                 xma_frame);
                if (ret != XMA_SUCCESS) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                            "Encoder send null frame failed:%d \n", ret);
                    return ENC_APP_DONE;
                }
                enc_ctx->enc_state = ENC_FLUSH;
            }
            break;

        case ENC_STOP:
            /* pretend we reached the end of the source and prepare to quit */
            enc_ctx->loop_count = 0;
            fseek (enc_ctx->in_file, 0, SEEK_END);
            enc_ctx->enc_state = ENC_EOF;
            break;

        default:
            break;
    }

    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_deinit: Encoder deinitialize

Parameters:
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_enc_deinit(XlnxEncoderCtx *enc_ctx, 
                     XmaEncoderProperties *xma_enc_props)
{
    if(enc_ctx->enc_session != NULL)
        xma_enc_session_destroy(enc_ctx->enc_session);

    fclose(enc_ctx->out_file);
    fclose(enc_ctx->in_file);

    free(enc_ctx->enc_props.enc_options);
    xlnx_enc_free_xma_props(xma_enc_props);
    return;
}
