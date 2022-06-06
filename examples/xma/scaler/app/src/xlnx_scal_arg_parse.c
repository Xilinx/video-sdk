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

#include "xlnx_scal_arg_parse.h"

struct option scaler_options[] = {
    {FLAG_HELP, no_argument, 0, HELP_ARG},
    {FLAG_LOG_LEVEL, required_argument, 0, LOG_LEVEL_ARG},
    {FLAG_COEFF_LOAD, required_argument, 0, COEF_LOAD_ARG},
    {FLAG_DEVICE_ID, required_argument, 0, DEVICE_ID_ARG},
    {FLAG_STREAM_LOOP, required_argument, 0, LOOP_COUNT_ARG},
    {FLAG_WIDTH, required_argument, 0, WIDTH_ARG},
    {FLAG_HEIGHT, required_argument, 0, HEIGHT_ARG},
    {FLAG_PIX_FMT, required_argument, 0, PIX_FMT_ARG},
    {FLAG_INPUT_FILE, required_argument, 0, INPUT_FILE_ARG},
    {FLAG_PIPELINE, required_argument, 0, PIPELINE_ARG},
    {FLAG_MIXRATE, required_argument, 0, MIXRATE_ARG},
    {FLAG_LATENCY_LOGGING, no_argument, 0, LATENCY_LOGGING_ARG},
    {FLAG_FRAMES, required_argument, 0, NUM_FRAMES_ARG},
    {FLAG_FPS, required_argument, 0, FPS_ARG},
    {FLAG_OUTPUT_FILE, required_argument, 0, OUTPUT_FILE_ARG},
    {0, 0, 0, 0}};

/**
 * xlnx_scal_get_help: Return the help message for printing.
 * @return The help message
 */
static char* xlnx_scal_get_help(char* program_name)
{
    char* help_msg = malloc(2048);
    sprintf(
        help_msg,
        "This program ingests an nv12, yuv420p, yuv420p10le, or xv15 input\n"
        "file and utilizes hardware acceleration to scale to various "
        "resolutions.\n\n"
        "Usage:\n"
        "\t%s [options] -w <input-width> -h <input-height> -i\n"
        "\t<input-file> [scaler_options] -w <output-1-width> -h\n"
        "\t<output-1-height> -o <output-1-file> -w ...\n\n"
        "Arguments:\n"
        "\t--help                     Print this message and exit\n"
        "\t-log <level>               Specify the log level\n"
        "\t-d <device-id>             Specify a device on which to run.\n"
        "\t                           Default: 0\n\n"
        "Input Arguments:\n\n"
        "\t-stream_loop <loop-count>  Number of times to loop the input\n"
        "\t                           file\n"
        "\t-w <width>                 Specify the input's width\n"
        "\t-h <height>                Specify the input's height\n"
        "\t-pix_fmt <pixel-format>    Pixel format of the input file (nv12,\n"
        "\t                           xv15, yuv420p10le). Default : nv12.\n"
        "\t-fps <frame-rate>          Frame rate. Used for scaler load\n"
        "\t                           calculation.\n"
        "\t-i <input-file>            Input file to be used\n\n"
        "Output Arguments:\n"
        "\t-coeff_load <load>         Specify the coefficient load. 0 Auto\n"
        "\t                           (default), 1 static, 2 FilterCoef.txt.\n"
        "\t-enable_pipeline           Enable scaler pipeline. Default disabled "
        "(0)\n"
        "\t-rate <half/full>          Set the rate to half. Half rate drops\n"
        "\t                           frames to reduce resource usage.\n"
        "\t                           Default: full.\n"
        "\t-enable-latency-logging    Enable latency logging\n"
        "\t-w <width>                 Specify the output's width\n"
        "\t-h <height>                Specify the output's height\n"
        "\t-pix_fmt <pixel-format>    Pixel format of the output file (nv12,\n"
        "\t                           xv15, yuv420p10le). Default: input fmt\n"
        "\t-frames <frame-count>      Number of frames to be processed.\n"
        "\t-o <file>                  File to which output is written.\n",
        program_name);
    return help_msg;
}

/**
 * xlnx_scal_dump_arguments: Print out the contents of the abr argument struct.
 * @param arguments: The arguments struct
 */
void xlnx_scal_dump_arguments(XlnxScalArguments arguments)
{
    fprintf(stderr,
            "App Params:\n\tLog Level: %d\n\tDevice: %4d\n\tLoop Count:"
            " %d\n",
            arguments.log_level, arguments.device_id, arguments.loop_count);
    fprintf(stderr,
            "Input Params:\n\tInput file: %s\n\tInput resolution: "
            "%dx%d\n\tformat: %d\n",
            arguments.input_file, arguments.input_width, arguments.input_height,
            arguments.input_pix_fmt);
    fprintf(stderr, "output Params:\n");
    XlnxScalOutArgs out_args;
    for(int i = 0; i < MAX_SCALER_OUTPUTS; i++) {
        out_args = arguments.out_arg_list[i];
        fprintf(stderr,
                "output %d:\n\tPipeline: %d\n\tResolution: %dx%d\n\t"
                "Rate: %d\n\tnum frames: %zu\n\tOutput File: %s\n",
                i, arguments.enable_pipeline, out_args.width, out_args.height,
                out_args.is_halfrate, arguments.num_frames,
                out_args.output_file);
    }
}

static void xlnx_scal_check_mixrate(XlnxScalArguments* arguments)
{
    assert(arguments->num_fullrate_outputs + arguments->num_halfrate_outputs ==
               arguments->outputs_used &&
           "Somehow num outputs != fullrate + "
           "mixrate outputs used.\n");
    bool is_halfrate_spotted = 0;
    for(int i = 0; i < arguments->outputs_used; i++) {
        XlnxScalOutArgs out_args = arguments->out_arg_list[i];
        if(out_args.is_halfrate) {
            is_halfrate_spotted = 1;
            if(i == 0) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                           "Half rate given before any full rate! "
                           "All full rate outputs must come before "
                           "half rates.\n");
                exit(XMA_APP_ERROR);
            }
        } else if(is_halfrate_spotted) { // Full rate given after halfrate
            xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                       "Full rate given after half rate! Full rate "
                       "cannot follow half rate. Output: %d, "
                       "File: %s\n",
                       i, out_args.output_file);
            exit(XMA_APP_ERROR);
        }
    }
}

/**
 * xlnx_scal_validate_output_arguments: Check the output argument values to make
 * sure they are valid.
 * @param arguments: The arguments struct
 * @param current_output: The current output
 */
static void xlnx_scal_validate_output_arguments(XlnxScalArguments* arguments,
                                                int current_output)
{
    XlnxScalOutArgs* out_args = &arguments->out_arg_list[current_output];
    if(out_args->width < MIN_SCALER_WIDTH ||
       out_args->width > MAX_SCALER_WIDTH) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "%d <= width <= %d. output %d width %d out of "
                   "range\n",
                   MIN_SCALER_WIDTH, MAX_SCALER_WIDTH, current_output,
                   out_args->width);
        exit(XMA_APP_ERROR);
    }
    if(out_args->height < MIN_SCALER_HEIGHT ||
       out_args->height > MAX_SCALER_WIDTH) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "%d <= height <= %d. Output height %d out of "
                   "range\n",
                   MIN_SCALER_HEIGHT, MAX_SCALER_WIDTH, out_args->height);
        exit(XMA_APP_ERROR);
    }
    if(out_args->width * out_args->height >
       MAX_SCALER_WIDTH * MAX_SCALER_HEIGHT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Output resolution %dx%d exceeds maximum %dx%d "
                   "or %dx%d allowed!\n",
                   out_args->width, out_args->height, MAX_SCALER_WIDTH,
                   MAX_SCALER_HEIGHT, MAX_SCALER_HEIGHT, MAX_SCALER_WIDTH);
        exit(XMA_APP_ERROR);
    }
    if(xlnx_utils_get_bit_depth_app_fmt(arguments->input_pix_fmt) !=
       xlnx_utils_get_bit_depth_app_fmt(out_args->pix_fmt)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Output pix fmt for output %d does not match input"
                   " bits per pixel! Unsupported format combination!\n",
                   current_output);
        exit(XMA_APP_ERROR);
    }
}

/**
 * xlnx_scal_validate_args: Check that the argument values are valid ranges.
 * @param arguments: The arguments struct
 */
static void xlnx_scal_validate_args(XlnxScalArguments* arguments)
{
    if(access(arguments->input_file, F_OK) != 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "INPUT FILE \"%s\" DOES NOT EXIST!\n",
                   arguments->input_file);
        exit(XMA_APP_ERROR);
    }
    if(arguments->input_width < MIN_SCALER_WIDTH ||
       arguments->input_width > MAX_SCALER_WIDTH) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "%d <= width <= %d. Input width %d out of range\n",
                   MIN_SCALER_WIDTH, MAX_SCALER_WIDTH, arguments->input_width);
        exit(XMA_APP_ERROR);
    }
    if(arguments->input_height < MIN_SCALER_HEIGHT ||
       arguments->input_height > MAX_SCALER_WIDTH) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "%d <= height <= %d. Input height %d out of range\n",
                   MIN_SCALER_HEIGHT, MAX_SCALER_WIDTH,
                   arguments->input_height);
        exit(XMA_APP_ERROR);
    }
    if(arguments->input_width * arguments->input_height >
       MAX_SCALER_WIDTH * MAX_SCALER_HEIGHT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Input resolution %dx%d greater than max supported %dx%d\n",
                   arguments->input_width, arguments->input_height,
                   MAX_SCALER_HEIGHT, MAX_SCALER_WIDTH);
        exit(XMA_APP_ERROR);
    }
    if(arguments->fps_num < 0 || arguments->fps_den <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Invalide fps values %d / %d\n", arguments->fps_num,
                   arguments->fps_den);
        exit(XMA_APP_ERROR);
    }
    if(arguments->loop_count < -1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "stream_loop value of %d is invalid! "
                   "0 <= stream_loop <= MAX_INT. -1 for infinite loop.\n",
                   arguments->loop_count);
        exit(XMA_APP_ERROR);
    }
    if(arguments->enable_pipeline) {
        // With enable_pipeline, make sure at least 3 frames are scaled.
        arguments->num_frames = max(3, arguments->num_frames);
    }
    if(arguments->outputs_used == 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "No outputs specified!\n");
        exit(XMA_APP_ERROR);
    }
    if(arguments->num_fullrate_outputs == 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "All output are half rate! At least oneoutput must be "
                   "full rate!\n");
        exit(XMA_APP_ERROR);
    }
    for(int i = 0; i < arguments->outputs_used; i++) {
        xlnx_scal_validate_output_arguments(arguments, i);
    }
    xlnx_scal_check_mixrate(arguments);
}

/**
 * xlnx_scal_set_param_from_fraction: Set the parameter if the source is of
 * fractional format.
 * @param numerator: Where to store the numerator
 * @param denominator: Where to store the denominator
 * @param source: The source string which should be a number or fraction.
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_scal_set_param_from_fraction(int* numerator, int* denominator,
                                             char* source)
{
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+(\\/[0-9]+)?$", source)) {
        return XMA_APP_ERROR;
    }
    const char frac[1] = "/";
    char*      tok     = strtok(source, frac);
    *numerator         = atoi(tok);
    tok                = strtok(0, frac);
    if(tok == NULL) {
        return XMA_APP_ERROR;
    }
    *denominator = atoi(tok);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_set_param_from_float: Set the parameter if the source is a float
 * @param numerator: Where to store the numerator
 * @param denominator: Where to store the denominator
 * @param source: The source string which should be a number or fraction.
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_scal_set_param_from_float(int* numerator, int* denominator,
                                          char* source)
{
    if(!xlnx_utils_check_if_pattern_matches("^[0-9]+(.[0-9]+)?$", source)) {
        return XMA_APP_ERROR;
    }
    float raw_value = atof(source);
    *numerator      = (int)ceil(raw_value);
    *denominator    = 1;
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_set_fractional_param: Set the fractional parameter from source
 * @param numerator: Where to store the numerator
 * @param denominator: Where to store the denominator
 * @param source: The source string which should be a number or fraction.
 * @param param_name: The name of the parameter to be set (For error message)
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_scal_set_fractional_param(int* numerator, int* denominator,
                                          char* source, char* param_name)
{
    if(xlnx_scal_set_param_from_fraction(numerator, denominator, source)) {

    } else if(xlnx_scal_set_param_from_float(numerator, denominator, source)) {

    } else {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Unrecognized value \"%s\" for argument -%s! "
                   "Make sure the value is of proper type.\n",
                   source, param_name);
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_check_current_output_range: Check if the output argument was
 * specified before input arguments were finished, or if there are too many
 * outputs given.
 * @param current_output: The current output
 * @param arg_name: The name of the output argument
 */
static void xlnx_scal_check_current_output_range(int   current_output,
                                                 char* arg_name)
{
    if(current_output < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Argument %s specified before input arguments are "
                   "complete! Provide an input file before moving "
                   "onto output arguments!\n",
                   arg_name);
        exit(XMA_APP_ERROR);
    } else if(current_output > MAX_SCALER_OUTPUTS - 1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Current output %d exceeds maximum allowed outputs %d\n",
                   current_output, MAX_SCALER_OUTPUTS);
        exit(XMA_APP_ERROR);
    }
}

/**
 * xlnx_scal_parse_commandline: Parse the commandline arguments into the
 * argument structure.
 * @param argc: The number of arguments
 * @param argv: The arguments
 * @param arguments: The arguments struct
 * @return XMA_APP_SUCCESS on success
 */
static int xlnx_scal_parse_commandline(int argc, char* argv[],
                                       XlnxScalArguments* arguments)
{
    int flag;
    int option_index;
    int current_output = -1; // Input args are -1 output
    int ret            = XMA_APP_SUCCESS;
    while(1) {
        flag = getopt_long_only(argc, argv, "", scaler_options, &option_index);
        if(flag == -1) {
            break;
        }
        switch(flag) {
            case HELP_ARG:; // Need statement to follow label.
                char* help_message = xlnx_scal_get_help(argv[0]);
                printf("%s\n", help_message);
                free(help_message);
                exit(XMA_APP_SUCCESS);
            case LOG_LEVEL_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->log_level, optarg,
                                             FLAG_LOG_LEVEL);
                break;
            case DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->device_id, optarg,
                                             FLAG_DEVICE_ID);
                break;
            case LOOP_COUNT_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->loop_count, optarg,
                                             FLAG_STREAM_LOOP);
                break;
            case INPUT_FILE_ARG:
                arguments->input_file = optarg;
                current_output++; // Input args are done, move onto output args
                break;
            case PIPELINE_ARG:
                ret = xlnx_utils_set_int_arg(&arguments->enable_pipeline,
                                             optarg, FLAG_PIPELINE);
                break;
            case LATENCY_LOGGING_ARG:
                arguments->enable_latency_logging = 1;
                break;
            case PIX_FMT_ARG:
                if(current_output > MAX_SCALER_OUTPUTS) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                               "Too many outputs specified. "
                               "Unable to set pix fmt %s. output %d\n",
                               optarg, current_output);
                    ret = XMA_APP_ERROR;
                    break;
                }
                if(current_output < 0) {
                    ret = xlnx_utils_set_pix_fmt_arg(&arguments->input_pix_fmt,
                                                     optarg, FLAG_PIX_FMT);
                } else {
                    ret = xlnx_utils_set_pix_fmt_arg(
                        &arguments->out_arg_list[current_output].pix_fmt,
                        optarg, FLAG_PIX_FMT);
                }
                break;
            case WIDTH_ARG:
                if(current_output > MAX_SCALER_OUTPUTS) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                               "Too many outputs specified. "
                               "Unable to set width %s. output %d\n",
                               optarg, current_output);
                    ret = XMA_APP_ERROR;
                    break;
                }
                if(current_output < 0) {
                    ret = xlnx_utils_set_int_arg(&arguments->input_width,
                                                 optarg, FLAG_WIDTH);
                } else {
                    ret = xlnx_utils_set_int_arg(
                        &arguments->out_arg_list[current_output].width, optarg,
                        FLAG_WIDTH);
                }
                break;
            case HEIGHT_ARG:
                if(current_output > MAX_SCALER_OUTPUTS) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                               "Too many outputs specified. "
                               "Unable to set height %s. output %d\n",
                               optarg, current_output);
                    ret = XMA_APP_ERROR;
                    break;
                }
                if(current_output < 0) {
                    ret = xlnx_utils_set_int_arg(&arguments->input_height,
                                                 optarg, FLAG_HEIGHT);
                } else {
                    ret = xlnx_utils_set_int_arg(
                        &arguments->out_arg_list[current_output].height, optarg,
                        FLAG_HEIGHT);
                }
                break;
            case FPS_ARG:
                ret = xlnx_scal_set_fractional_param(
                    &arguments->fps_num, &arguments->fps_den, optarg, FLAG_FPS);
                break;
            case NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&arguments->num_frames, optarg,
                                                FLAG_FRAMES);
                break;
            case MIXRATE_ARG:
                xlnx_scal_check_current_output_range(current_output,
                                                     "-" FLAG_MIXRATE);
                if(strcmp(optarg, "half") == 0) {
                    arguments->out_arg_list[current_output].is_halfrate = 1;
                } else if(strcmp(optarg, "full") != 0) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                               "Unrecognized rate: %s\n", optarg);
                    ret = XMA_APP_ERROR;
                }
                break;
            case COEF_LOAD_ARG:
                xlnx_scal_check_current_output_range(current_output,
                                                     "-" FLAG_COEFF_LOAD);
                ret = xlnx_utils_set_int_arg(
                    &arguments->out_arg_list[current_output].coeff_load, optarg,
                    FLAG_COEFF_LOAD);
                break;
            case OUTPUT_FILE_ARG:
                // Make sure current_output is in range
                xlnx_scal_check_current_output_range(current_output, "-o");
                arguments->out_arg_list[current_output].output_file = optarg;
                if(arguments->out_arg_list[current_output].is_halfrate) {
                    arguments->num_halfrate_outputs++;
                } else {
                    arguments->num_fullrate_outputs++;
                }
                current_output++;
                break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                           "ERROR: Failed to properly parse commandline.\n");
                ret = XMA_APP_ERROR;
        }
        // XMA_APP_ERROR should == XMA_APP_ERROR.
        if(ret == XMA_APP_ERROR || ret == XMA_APP_ERROR) {
            exit(ret);
        }
    }
    arguments->outputs_used = current_output;
    if(optind < argc) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_SCALER_APP_MODULE,
                   "Failed Unrecognized argument %s.\n", argv[optind]);
        exit(XMA_APP_ERROR);
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_bubble_sort_output_args: Sort output arguments by resolution in
 * descending order. This improves scaler quality
 * @param arguments: The arguments struct
 */
static void xlnx_scal_bubble_sort_output_args(XlnxScalArguments* arguments,
                                              int start, int end)
{
    int             current_pixels, max_pixels, max_index;
    int             j;
    XlnxScalOutArgs temp, current_output;
    for(int i = start; i < end; i++) {
        max_pixels = 0;
        max_index  = 0;
        for(j = i; j < end; j++) {
            current_output = arguments->out_arg_list[j];
            current_pixels = current_output.width * current_output.height;
            if(current_pixels > max_pixels) {
                max_pixels = current_pixels;
                max_index  = j;
            }
        }
        if(j != i) { // There was a bigger resolution
            temp = arguments->out_arg_list[max_index];
            arguments->out_arg_list[max_index] = arguments->out_arg_list[i];
            arguments->out_arg_list[i]         = temp;
        }
    }
}

/**
 * xlnx_scal_fix_output_arguments: Make sure there are no null values
 * @param arguments: The arguments struct
 */
static void xlnx_scal_fix_output_arguments(XlnxScalArguments* arguments)
{
    XlnxScalOutArgs* out_args;
    for(int i = 0; i < arguments->outputs_used; i++) {
        out_args = &arguments->out_arg_list[i];
        out_args->width =
            replace_if_unset(out_args->width, arguments->input_width);
        out_args->height =
            replace_if_unset(out_args->height, arguments->input_height);
        if(out_args->pix_fmt == XMA_APP_NONE_FMT_TYPE) {
            out_args->pix_fmt = arguments->input_pix_fmt;
        }
    }
}

/**
 * Set default arguments
 * @param arguments The empty argument struct
 * @return void
 */
static void xlnx_scal_set_default_arguments(XlnxScalArguments* arguments)
{
    arguments->log_level     = DEBUG_LOGLEVEL;
    arguments->input_pix_fmt = XMA_APP_NV12_FMT_TYPE;
    arguments->input_file    = "";
    arguments->device_id     = DEFAULT_DEVICE_ID;
    arguments->fps_num       = 25;
    arguments->fps_den       = 1;
    arguments->num_frames    = SIZE_MAX;

    // Null values will match input args if not set.
    for(int i = 0; i < MAX_SCALER_OUTPUTS; i++) {
        arguments->out_arg_list[i].output_file = "";
        arguments->out_arg_list[i].width       = UNASSIGNED; // INT32_MIN
        arguments->out_arg_list[i].height      = UNASSIGNED;
        arguments->out_arg_list[i].pix_fmt     = XMA_APP_NONE_FMT_TYPE;
    }
}

/**
 * xlnx_scal_get_arguments: Parses the commandline arguments into a struct
 * corresponding to scaler arguments
 * @param argc: The number of arguments
 * @param argv: The arguments
 * @return An argument struct containing scaler arguments.
 */
XlnxScalArguments xlnx_scal_get_arguments(int argc, char* argv[])
{
    if(argc < 2) {
        char* help_message = xlnx_scal_get_help(argv[0]);
        printf("%s\n", help_message);
        free(help_message);
        exit(XMA_APP_SUCCESS);
    }
    XlnxScalArguments arguments;
    memset(&arguments, 0, sizeof(arguments));
    xlnx_scal_set_default_arguments(&arguments);
    xlnx_scal_parse_commandline(argc, argv, &arguments);

    xlnx_scal_fix_output_arguments(&arguments);
    xlnx_scal_validate_args(&arguments);
    xlnx_scal_bubble_sort_output_args(&arguments, 0,
                                      arguments.num_fullrate_outputs);
    if(arguments.num_halfrate_outputs > 0) {
        /* We can't mix full rate and half rate arguments because of xma plugin
           limitation, so we sort them separately. */
        xlnx_scal_bubble_sort_output_args(
            &arguments, arguments.num_fullrate_outputs, arguments.outputs_used);
    }
    return arguments;
}
