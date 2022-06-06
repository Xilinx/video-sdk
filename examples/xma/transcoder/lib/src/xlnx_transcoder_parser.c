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

#include "xlnx_transcoder_parser.h"

static struct option transcode_options[] = {
    {FLAG_TRANSCODE_HELP, no_argument, 0, HELP_ARG},
    {FLAG_TRANSCODE_DEVICE_ID, required_argument, 0, TRANSCODE_DEVICE_ID_ARG},
    {FLAG_TRANSCODE_STREAM_LOOP, required_argument, 0,
     TRANSCODE_STREAM_LOOP_ARG},
    {FLAG_TRANSCODE_NUM_FRAMES, required_argument, 0, TRANSCODE_NUM_FRAMES_ARG},
    {FLAG_TRANSCODE_GENERIC_MAX, required_argument, 0,
     TRANSCODE_GENERIC_MAX_ARG},
    {0, 0, 0, 0}};

/**
 * xlnx_tran_get_help: Prints the list of supported arguments for transcoder
 * application
 *
 * @return List of supported arguments
 */
char* xlnx_tran_get_help()
{

    return " XMA Transcoder App Usage:\n\t"
           "./program [generic options] -c:v <decoder codec> [decoder options] "
           " -i input-file -multiscale_xma -outputs [num] [Scaler options] "
           " -c:v <encoder codec> [encoder options] -o <output-file> "
           " -c:v <encoder codec> [encoder options] -o <output-file> "
           " -c:v <encoder codec> [encoder options] -o <output-file>.....\n\n"
           "Arguments:\n\n"
           "\t--help                     Print this message and exit.\n"
           "\t-d <device-id>             Specify a device on which the\n"
           "\t                           transcoder to run. Default: 0\n"
           "\t-stream_loop <loop-count>  Number of times to loop the input "
           "file\n"
           "\t-frames <frame-count>      Number of input frames to be "
           "processed\n"
           "\t\n"
           "Decoder options:\n\n"
           "\t-c:v <codec>               Decoder codec to be used. Supported\n"
           "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264\n"
           "\t-dl <device-id>            Specify device on which decoder "
           "should run\n"
           "\t-low-latency <0/1>         Low latency for decoder. Default\n"
           "\t                           disabled\n"
           "\t-latency_logging <0/1>     Latency logging for decoder. Default\n"
           "\t                           disabled\n"
           "\t-i <input-file>            Name and path of input H.264/HEVC "
           "file\n"
           "\t\n"
           "Scaler options:\n\n"
           "\t-multiscale_xma            Name of the ABR scaler filter\n"
           "\t-num-output <value>        Number of output files from scaler\n"
           "\t-xvbm_convert              Convert the scaler's input into a "
           "host buffer.\n"
           "\t                           Useful when running scaler on a "
           "different device.\n"
           "\t-dl <device-id>            Specify device on which scaler should "
           "run\n"
           "\t-enable_pipeline           Enable/disable pipelining in the "
           "scaler. Default disabled (0)\n"
           "\t-out_1_width <width>       Width of the scaler output channel 1\n"
           "\t-out_1_height <height>     Height of the scaler output channel "
           "1\n"
           "\t-out_1_rate <full/half>    Full of Half rate for output channel "
           "1\n"
           "\t-out_2_width <width>       Width of the scaler output channel 2\n"
           "\t-out_2_height <height>     Height of the scaler output channel "
           "2\n"
           "\t-out_2_rate <full/half>    Full of Half rate for output channel "
           "2\n"
           "\t-out_3_width <width>       Width of the scaler output channel 3\n"
           "\t-out_3_height <height>     Height of the scaler output channel "
           "3\n"
           "\t-out_3_rate <full/half>    Full of Half rate for output channel "
           "3\n"
           "\t-out_4_width <width>       Width of the scaler output channel 4\n"
           "\t-out_4_height <height>     Height of the scaler output channel "
           "4\n"
           "\t-out_4_rate <full/half>    Full of Half rate for output channel "
           "4\n"
           "\t-out_5_width <width>       Width of the scaler output channel 5\n"
           "\t-out_5_height <height>     Height of the scaler output channel "
           "5\n"
           "\t-out_5_rate <full/half>    Full of Half rate for output channel "
           "5\n"
           "\t-out_6_width <width>       Width of the scaler output channel 6\n"
           "\t-out_6_height <height>     Height of the scaler output channel "
           "6\n"
           "\t-out_6_rate <full/half>    Full of Half rate for output channel "
           "6\n"
           "\t-out_7_width <width>       Width of the scaler output channel 7\n"
           "\t-out_7_height <height>     Height of the scaler output channel "
           "7\n"
           "\t-out_7_rate <full/half>    Full of Half rate for output channel "
           "7\n"
           "\t-out_8_width <width>       Width of the scaler output channel 8\n"
           "\t-out_8_height <height>     Height of the scaler output channel "
           "8\n"
           "\t-out_8_rate <full/half>    Full of Half rate for output channel "
           "8\n"
           "\t-latency_logging <0/1>     Latency logging for scaler. Default\n"
           "\t                           disabled\n"
           "Encoder options:\n\n"
           "\t-c:v <codec>               Encoder codec to be used. Supported\n"
           "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264\n"
           "\t-dl <device-id>            Specify device on which encoder "
           "should run\n"
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
           "\t-force_key_frame <value>   Insert idr frame dynamically  (Frame\n"
           "\t                           numbers starting from 0)\n"
           "\t                           Example: -force_key_frame "
           "\"(122,222,333)\"\n"
           "\t-profile <value>           Encoder profile.\n"
           "\t           For HEVC, supported are 0 or main, 1 or main-intra,\n"
           "\t                           2 or main-10 and 3 or main-10-intra."
           "\n"
           "\t                           Default is 0/main.\n"
           "\t                           ENC_HEVC_MAIN - 0 or main.\n"
           "\t                           ENC_HEVC_MAIN_INTRA - 1 or "
           "main-intra.\n"
           "\t                           ENC_HEVC_MAIN_10 - 2 or main-10.\n"
           "\t                           ENC_HEVC_MAIN_10_INTRA - 3 or "
           "main-10-intra.\n"
           "\t           For H264, supported are 66 or baseline, 77 or main,\n"
           "\t                           100 or high, 110 or high-10 and\n"
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
           "42, 50, 51, 52.\n"
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
           "stored as\n"
           "\t                           string of key=value pairs.\n"
           "\t                           dynamic-params=<params_file>\n"
           "\t-o <file>                  File to which output is written.\n";
}

/**
 * Check that the arugments parsed specifically by the transcoder are in correct
 * range.
 * @param transcode_ctx
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error.
 */
static int32_t xlnx_tran_validate_arguments(XlnxTranscoderCtx* transcode_ctx)
{
    int dev_index = transcode_ctx->dec_ctx.dec_props.dev_index;
    if(dev_index < DEFAULT_DEVICE_ID || dev_index > XLNX_MAX_DEVICE_COUNT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported device ID %d\n", dev_index);
        return XMA_APP_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Make sure if different parts of the pipeline are running on different
 * devices, xvbm_convert has been specified.
 * @param transcode_ctx
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error.
 */
static int32_t xlnx_tran_check_dev_indices(XlnxTranscoderCtx* transcode_ctx)
{
    int32_t dev_index_1, dev_index_2;
    dev_index_1 = transcode_ctx->dec_ctx.dec_props.dev_index;
    dev_index_2 = transcode_ctx->scal_ctx.scal_props.dev_index;
    if(dev_index_1 != dev_index_2 && !transcode_ctx->scal_ctx.convert_input) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Decoder input is on a different device than the scaler, "
                   "but the input is not converted! Use -xvbm_convert or run "
                   "them on the same device!\n");
        return XMA_APP_ERROR;
    }
    for(int i = 0; i < transcode_ctx->num_enc_channels; i++) {
        if(i >= transcode_ctx->num_enc_channels - transcode_ctx->num_scal_out) {
            dev_index_1 = transcode_ctx->scal_ctx.scal_props.dev_index;
        }
        dev_index_2 = transcode_ctx->enc_ctx[i].enc_props.dev_index;
        if(dev_index_1 != dev_index_2 &&
           !transcode_ctx->enc_ctx[i].convert_input) {
            xma_logmsg(
                XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Encoder input is on a different device than the "
                "encoder channel %d, but the input is not converted! Use "
                "-xvbm_convert or run them on the same device!\n",
                i);
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_tran_parse_args: Function to parse command line arguments
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param transcode_ctx: Transcoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_tran_parse_args(int32_t argc, char* argv[],
                                    XlnxTranscoderCtx* transcode_ctx)
{

    int32_t ret  = XMA_APP_SUCCESS;
    int32_t flag = 0;
    int32_t option_index;
    int32_t i;

    flag = getopt_long_only(argc, argv, "", transcode_options, &option_index);
    if(flag == -1) {
        return XMA_APP_ERROR;
    }
    while(flag != TRANSCODE_GENERIC_MAX_ARG) {
        switch(flag) {
            case HELP_ARG:
                printf("%s\n", xlnx_tran_get_help());
                exit(0);

            case TRANSCODE_DEVICE_ID_ARG:;
                int dev_index = DEFAULT_DEVICE_ID;
                ret           = xlnx_utils_set_int_arg(&dev_index, optarg,
                                             FLAG_TRANSCODE_DEVICE_ID);
                if(ret == XMA_APP_SUCCESS) {
                    transcode_ctx->dec_ctx.dec_props.dev_index   = dev_index;
                    transcode_ctx->scal_ctx.scal_props.dev_index = dev_index;
                    for(int i = 0; i < SCAL_MAX_ABR_CHANNELS; i++) {
                        transcode_ctx->enc_ctx[i].enc_props.dev_index =
                            dev_index;
                    }
                }
                break;

            case TRANSCODE_STREAM_LOOP_ARG:
                ret = xlnx_utils_set_int_arg(&transcode_ctx->loop_count, optarg,
                                             FLAG_TRANSCODE_STREAM_LOOP);
                break;

            case TRANSCODE_NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&transcode_ctx->num_frames,
                                                optarg,
                                                FLAG_TRANSCODE_NUM_FRAMES);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in parsing generic transcoder arguments %d\n",
                           flag);
                return XMA_APP_ERROR;
        }

        flag =
            getopt_long_only(argc, argv, "", transcode_options, &option_index);
        if(flag == -1 || ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
    }
    if(xlnx_tran_validate_arguments(transcode_ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    if((ret = xlnx_dec_parse_args(argc, argv, &transcode_ctx->dec_ctx, flag)) !=
       XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    flag = 0;
    if((ret = xlnx_scal_parse_args(argc, argv, &transcode_ctx->scal_ctx,
                                   &flag)) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    transcode_ctx->num_scal_out = transcode_ctx->scal_ctx.scal_props.nb_outputs;
    transcode_ctx->num_enc_channels = (transcode_ctx->num_scal_out + 1);

    for(i = 0; i < transcode_ctx->num_enc_channels; i++) {
        if((ret = xlnx_enc_parse_args(argc, argv, &transcode_ctx->enc_ctx[i],
                                      flag)) <= XMA_APP_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error in parsing encoder arguments %d\n", flag);
            return XMA_APP_ERROR;
        } else if(ret & TRANSCODE_PARSING_DONE) {
            break;
        }
        flag = 0;
    }

    transcode_ctx->num_enc_channels = i;
    transcode_ctx->non_scal_channels =
        (transcode_ctx->num_enc_channels - transcode_ctx->num_scal_out);

    for(i = 1; i < transcode_ctx->num_enc_channels; i++) {
        if(transcode_ctx->enc_ctx[i - 1].enc_props.lookahead_depth !=
           transcode_ctx->enc_ctx[i].enc_props.lookahead_depth) {
            xma_logmsg(
                XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Different LA depths are not supported across channels\n");
            return XMA_APP_ERROR;
        }
    }
    if(xlnx_tran_check_dev_indices(transcode_ctx) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    return ret;
}

/**
 * xlnx_tran_update_props: Function for updating transcoder properties
 *
 * @param transcode_ctx: Transcoder context
 * @param transcode_props: Transcoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_tran_update_props(XlnxTranscoderCtx*        transcode_ctx,
                                      XlnxTranscoderProperties* transcode_props)
{
    XlnxScalerProperties* scal_props = &transcode_ctx->scal_ctx.scal_props;
    xlnx_dec_update_props(&transcode_ctx->dec_ctx,
                          &transcode_props->xma_dec_props);

    if(transcode_ctx->num_scal_out) {
        /* Update scaler width, height and fps from decoder context */
        scal_props->in_width  = transcode_ctx->dec_ctx.frame_data.width;
        scal_props->in_height = transcode_ctx->dec_ctx.frame_data.height;
        scal_props->fr_num    = transcode_ctx->dec_ctx.frame_data.fr_num;
        scal_props->fr_den    = transcode_ctx->dec_ctx.frame_data.fr_den;
        scal_props->bits_per_pixel =
            transcode_ctx->dec_ctx.frame_data.luma_bit_depth;

        if(xlnx_scal_update_props(&transcode_ctx->scal_ctx,
                                  &transcode_props->xma_scal_props) <=
           XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }

        if(transcode_ctx->scal_ctx.convert_input) {
            transcode_ctx->scal_ctx.send_frame = xlnx_scal_create_xma_frame(
                &transcode_ctx->scal_ctx.scal_props, 1);
        } else {
            transcode_ctx->scal_ctx.send_frame =
                transcode_ctx->scal_ctx.in_frame;
        }
    }

    transcode_ctx->num_scal_fullrate =
        transcode_ctx->scal_ctx.session_nb_outputs[SCAL_SESSION_FULL_RATE];

    for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {
        XlnxEncoderCtx*        enc_ctx   = &transcode_ctx->enc_ctx[i];
        XlnxEncoderProperties* enc_props = &enc_ctx->enc_props;

        enc_props->bits_per_pixel =
            transcode_ctx->dec_ctx.frame_data.luma_bit_depth;
        if((i == 0) && transcode_ctx->non_scal_channels) {
            enc_props->width  = transcode_ctx->dec_ctx.frame_data.width;
            enc_props->height = transcode_ctx->dec_ctx.frame_data.height;
        } else {
            int32_t buf_idx   = i - transcode_ctx->non_scal_channels;
            enc_props->width  = scal_props->out_width[buf_idx];
            enc_props->height = scal_props->out_height[buf_idx];
        }
        if(enc_props->fps == ENC_DEFAULT_FRAMERATE) {
            if(i < (transcode_ctx->num_scal_fullrate +
                    transcode_ctx->non_scal_channels)) {
                enc_props->fps = (transcode_ctx->dec_ctx.frame_data.fr_num /
                                  transcode_ctx->dec_ctx.frame_data.fr_den);
            } else {
                enc_props->fps = (scal_props->fr_num / scal_props->fr_den);
            }
        }

        enc_ctx->xma_buffer = &transcode_ctx->xma_out_buffer[i];

        if((xlnx_enc_update_props(
               enc_ctx, &transcode_props->xma_enc_props[i])) == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }

        /* If the decoder output needs to be used by both scaler and encoder,
           then a copy of decoder xma frame to be done to send it to the
           encoder. xma_app_frame[1] will be a copy of decoder output
           i.e., xma_app_frame[0] */
        if(transcode_ctx->non_scal_channels) {
            enc_ctx->in_frame = &transcode_ctx->xma_app_frame[i + 1];
        } else {
            enc_ctx->in_frame = &transcode_ctx->xma_app_frame[i + 2];
        }
        if(enc_ctx->convert_input) {
            enc_ctx->send_frame = xlnx_enc_create_xma_frame(enc_props);
        } else {
            enc_ctx->send_frame = enc_ctx->in_frame;
        }
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_tran_context_init: Initialize transcoder context params to default
 *
 * @param transcode_ctx: Transcoder context
 */
static void xlnx_tran_context_init(XlnxTranscoderCtx* transcode_ctx)
{

    transcode_ctx->loop_count         = 0;
    transcode_ctx->enc_chan_idx       = 0;
    transcode_ctx->num_frames         = SIZE_MAX;
    transcode_ctx->flush_mode         = 0;
    transcode_ctx->eos_count          = 0;
    transcode_ctx->out_frame_cnt      = 0;
    transcode_ctx->in_frame_cnt       = 0;
    transcode_ctx->transcoder_state   = TRANSCODE_DEC_READ_FRAME;
    transcode_ctx->la_in_index        = 0;
    transcode_ctx->dec_out_index      = 0;
    transcode_ctx->num_scal_fullrate  = 0;
    transcode_ctx->num_scal_out       = 0;
    transcode_ctx->num_enc_channels   = 0;
    transcode_ctx->curr_sess_channels = 0;
    transcode_ctx->non_scal_channels  = 0;

    transcode_ctx->dec_ctx.out_frame = &transcode_ctx->xma_app_frame[0];
    transcode_ctx->scal_ctx.in_frame = &transcode_ctx->xma_app_frame[0];
    for(int32_t i = 0; i < SCAL_MAX_ABR_CHANNELS; i++) {
        transcode_ctx->scal_ctx.out_frame[i] =
            &transcode_ctx->xma_app_frame[i + 2];
    }

    xlnx_dec_context_init(&transcode_ctx->dec_ctx);
    xlnx_scal_context_init(&transcode_ctx->scal_ctx);
    for(int32_t i = 0; i < TRANSCODE_MAX_ABR_CHANNELS; i++) {
        xlnx_enc_context_init(&transcode_ctx->enc_ctx[i]);
        transcode_ctx->xma_app_frame[i].data[0].buffer_type =
            XMA_DEVICE_BUFFER_TYPE;
    }

    return;
}

/**
 * xlnx_tran_parser: Function for argument parsing and initialization
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param transcode_ctx: Transcoder context
 * @param transcode_props: Transcoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_tran_parser(int32_t argc, char* argv[],
                         XlnxTranscoderCtx*        transcode_ctx,
                         XlnxTranscoderProperties* transcode_props)
{

    /* Initializing the transcoder context with default values */
    xlnx_tran_context_init(transcode_ctx);
    if(xlnx_tran_parse_args(argc, argv, transcode_ctx) <= XMA_APP_ERROR) {
        return XMA_APP_ERROR;
    }

    if((xlnx_dec_parse_frame(&transcode_ctx->dec_ctx)) != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error in decoder parser\n");
        return XMA_APP_ERROR;
    }

    if((xlnx_tran_update_props(transcode_ctx, transcode_props)) ==
       XMA_APP_ERROR) {
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}
