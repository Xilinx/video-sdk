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

#include "xlnx_dec_arg_parse.h"

struct option dec_options[] =
{
    {FLAG_HELP,                no_argument,       0, HELP_ARG},
    {FLAG_LOG_LEVEL,           required_argument, 0, LOG_LEVEL_ARG},
    {FLAG_DEVICE_ID,           required_argument, 0, DEVICE_ID_ARG},
    {FLAG_STREAM_LOOP,         required_argument, 0, LOOP_COUNT_ARG},
    {FLAG_INPUT_FILE,          required_argument, 0, INPUT_FILE_ARG},
    {FLAG_CODEC_TYPE,          required_argument, 0, DECODER_ARG},
    {FLAG_LOW_LATENCY,         required_argument, 0, LOW_LATENCY_ARG},
    {FLAG_ENTROPY_BUF_CNT,     required_argument, 0, ENTROPY_BUFFERS_COUNT_ARG},
    {FLAG_LATENCY_LOGGING,     required_argument, 0, LATENCY_LOGGING_ARG},
    {FLAG_SPLITBUFF_MODE,      required_argument, 0, SPLITBUFF_MODE_ARG},
    {FLAG_NUM_FRAMES,          required_argument, 0, NUM_FRAMES_ARG},
    {FLAG_OUTPUT_FILE,         required_argument, 0, OUTPUT_FILE_ARG},
    {0, 0, 0, 0}
};

static char* xlnx_dec_get_help(char* program_name)
{
    char* help_msg = malloc(4096);
    sprintf(help_msg,
        "This is a standalone xma decoder app. It ingests an h264 or h265 \n"
        "encoded file and utilizes hardware acceleration to get the decoded \n"
        "output.\n\n"
        "Usage: \n"
            "\t%s [options] -i <input-file> -c:v <codec-type> \n"
            "\t[codec_options] -o <output-file>\n\n" 
        "Arguments:\n"
        "\t--help                     Print this message and exit\n"
        "\t-log <level>               Specify the log level\n"
        "\t-d <device-id>             Specify a device on which to run.\n"
        "\t                           Default: 0\n\n"
        "Input Arguments:\n\n"
        "\t-stream_loop <loop-count>  Number of times to loop the input\n"
        "\t                           file\n"
        "\t-i <input-file>            Input file to be used\n\n"
        "Codec Arguments:\n\n"
        "\t-c:v <codec>               Specify H264 or H265 decoding. \n"
        "\t                           ("H264_CODEC_NAME", " H265_CODEC_NAME")\n"
        "\t-low_latency               Should low latency decoding be used\n"
        "\t-entropy_buf_cnt <count>   Specify number of internal entropy\n"
        "\t                           buffers. [2-10], default: 2\n"
        "\t-latency_logging           Log latency information to syslog\n"
        "\t-splitbuff_mode            Configure decoder in split/unsplit\n"
        "\t                           input buffer mode\n"
        "\t-frames <frame-count>      Number of frames to be processed.\n"
        "\t-o <file>                  File to which output is written.\n", 
        program_name);
    return help_msg;
}

// TODO: align the spacing 
void xlnx_dec_dump_arguments(XlnxDecoderArguments arguments)
{
    printf("App Params:\n\tLog Level: %d\n\tDevice: %d\n\tLoop Count: %d\n",
           arguments.log_level, arguments.device_id, arguments.loop_count);
    printf("Input Params:\n\tInput file: %s\n",arguments.input_file);
    printf("Decoder Params:\n\tCodec: %d, Low Latency: %d\n\tEntropy "
           "Buffer Count: %d\n\tLatency Logging: %d\n\tSplit Buffer Mode: "
           "%d\n\tOutput File: %s\n", arguments.decoder, 
           arguments.low_latency, arguments.entropy_buf_cnt, 
           arguments.latency_logging, arguments.splitbuff_mode, 
           arguments.output_file);
}

static void xlnx_dec_validate_codec_arguments(XlnxDecoderArguments* arguments)
{
    if(arguments->entropy_buf_cnt < MIN_ENTROPY_BUFF_COUNT || 
       arguments->entropy_buf_cnt > MAX_ENTROPY_BUFF_COUNT) {
        DECODER_APP_LOG_ERROR("Invalid entropy buffer count %d! Valid values "
                              "range [2-10]. Default is 2.\n", 
                              arguments->entropy_buf_cnt);
        exit(DEC_APP_ERROR);
    }
    if(strcmp(arguments->output_file, "") == 0) {
        DECODER_APP_LOG_ERROR("Output file not set!\n");
        exit(DEC_APP_ERROR);
    }
}

static void xlnx_dec_validate_arguments(XlnxDecoderArguments* arguments)
{
    if(access(arguments->input_file, F_OK) != 0) {
        DECODER_APP_LOG_ERROR("INPUT FILE \"%s\" DOES NOT EXIST!\n", 
                              arguments->input_file);
        exit(DEC_APP_ERROR);
    }
    if(arguments->loop_count < -1) {
        DECODER_APP_LOG_ERROR("stream_loop value of %d is invalid! "
                              "0 <= stream_loop <= MAX_INT. -1 for infinite "
                              "loop.\n", arguments->loop_count);
        exit(DEC_APP_ERROR);
    }
    xlnx_dec_validate_codec_arguments(arguments);
}

static int xlnx_dec_parse_commandline(int argc, char* argv[], 
                                      XlnxDecoderArguments* arguments)
{
    int flag;
    int option_index;
    int ret = DEC_APP_SUCCESS;
    while(1) {
        flag = getopt_long_only(argc, argv, "", dec_options, &option_index);
        if(flag == -1) {
            break;
        }
        switch (flag) {
            case HELP_ARG:
                fprintf(stderr, "%s\n", xlnx_dec_get_help(argv[0]));
                exit(DEC_APP_SUCCESS);
            case LOG_LEVEL_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->log_level, 
                                             optarg, FLAG_LOG_LEVEL);
                break;
            case DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->device_id, 
                                             optarg, FLAG_DEVICE_ID);
                break;
            case LOOP_COUNT_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->loop_count, 
                                             optarg, FLAG_STREAM_LOOP);
                break;
            case INPUT_FILE_ARG:
                arguments->input_file = optarg;
                break;
            case DECODER_ARG:
                if(xlnx_utils_check_if_pattern_matches(HEVC_PATTERN_MATCH, 
                   optarg)) {
                    arguments->decoder = HEVC_CODEC_TYPE;
                } else if(xlnx_utils_check_if_pattern_matches(AVC_PATTERN_MATCH, 
                                                              optarg)) {
                    arguments->decoder = AVC_CODEC_TYPE;
                } else {
                    DECODER_APP_LOG_ERROR("ERROR: Unrecognized decoder %s\n", 
                                          optarg);
                    ret = DEC_APP_ERROR;
                }
                break;
            case LOW_LATENCY_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->low_latency, 
                                             optarg, FLAG_LOW_LATENCY);
                break;
            case ENTROPY_BUFFERS_COUNT_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->entropy_buf_cnt, 
                                             optarg, FLAG_ENTROPY_BUF_CNT);
                break;
            case LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->latency_logging, 
                                             optarg, FLAG_LATENCY_LOGGING);
                break;
            case SPLITBUFF_MODE_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->splitbuff_mode, optarg, 
                                             FLAG_SPLITBUFF_MODE);
                break;
            case NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&arguments->num_frames, optarg, 
                                                FLAG_NUM_FRAMES);
                break;
            case OUTPUT_FILE_ARG:
                arguments->output_file = optarg;
                break;
            default:
                DECODER_APP_LOG_ERROR("Failed to properly parse commandline. %s"
                                      "\n", argv[optind-1]);
                ret = DEC_APP_ERROR;
        }
        /* DEC_APP_ERROR should == RET_ERROR */
        if(ret == DEC_APP_ERROR || ret == RET_ERROR) { 
            exit(ret);
        }
    }
    if(optind < argc) {
        DECODER_APP_LOG_ERROR("Failed Unrecognized argument %s.\n", 
                              argv[optind]);
        return DEC_APP_ERROR;
    }
    return DEC_APP_SUCCESS;
}

XlnxDecoderArguments xlnx_dec_get_arguments(int argc, char* argv[])
{
    if(argc < 2) {
        printf("%s\n", xlnx_dec_get_help(argv[0]));
        exit(DEC_APP_SUCCESS);
    }
    XlnxDecoderArguments arguments;
    /* Set defaults */
    arguments.log_level                =  DEBUG_LOGLEVEL;
    arguments.input_file               =  "";
    arguments.loop_count               =  0;
    arguments.device_id                =  DEFAULT_DEVICE_ID;
    arguments.output_file              =  "";
    arguments.low_latency              =  0;
    arguments.entropy_buf_cnt          =  DEFAULT_ENTROPY_BUFF_COUNT;
    arguments.latency_logging          =  0;
    arguments.splitbuff_mode           =  0;
    arguments.num_frames               =  SIZE_MAX;
    xlnx_dec_parse_commandline(argc, argv, &arguments);
    xlnx_dec_validate_arguments(&arguments);
    return arguments;
}

#if 0
int main(int argc, char *argv[])
{
    XlnxDecoderArguments arguments = getarguments(argc, argv);
    xlnx_dec_validate_arguments(&arguments);
    xlnx_dec_dump_arguments(arguments);
    return DEC_APP_SUCCESS;
}
#endif
