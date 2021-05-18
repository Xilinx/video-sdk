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

#include "xlnx_transcoder_parser.h"

static struct option transcode_options[] =
{
    {FLAG_TRANSCODE_HELP,           no_argument,       0, HELP_ARG},
    {FLAG_TRANSCODE_DEVICE_ID,      required_argument, 0, TRANSCODE_DEVICE_ID_ARG},
    {FLAG_TRANSCODE_STREAM_LOOP,    required_argument, 0, TRANSCODE_STREAM_LOOP_ARG},
    {FLAG_TRANSCODE_NUM_FRAMES,     required_argument, 0, TRANSCODE_NUM_FRAMES_ARG},
    {FLAG_TRANSCODE_GENERIC_MAX,    required_argument, 0, TRANSCODE_GENERIC_MAX_ARG},
    {0, 0, 0, 0}
};

/*-----------------------------------------------------------------------------
xlnx_tran_get_help: Prints the list of supported arguments for transcoder 
                    application

Return:
List of supported arguments
-----------------------------------------------------------------------------*/
char* xlnx_tran_get_help()
{

    return " XMA Transcoder App Usage: \n\t"
        "./program [generic options] -c:v <decoder codec> [decoder options] "
        " -i input-file -multiscale_xma -outputs [num] [Scaler options] "
        " -c:v <encoder codec> [encoder options] -o <output-file> " 
        " -c:v <encoder codec> [encoder options] -o <output-file> " 
        " -c:v <encoder codec> [encoder options] -o <output-file>..... \n\n" 
        "Arguments:\n\n"
        "\t--help                     Print this message and exit.\n"
        "\t-d <device-id>             Specify a device on which the \n"
        "\t                           transcoder to run. Default: 0\n"
        "\t-stream_loop <loop-count>  Number of times to loop the input file \n"
        "\t-frames <frame-count>      Number of input frames to be processed \n"
        "\t                           \n"
        "Decoder options:\n\n"
        "\t-c:v <codec>               Decoder codec to be used. Supported \n"
        "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264 \n"
        "\t-low-latency <0/1>         Low latency for decoder. Default \n"
        "\t                           disabled \n"
        "\t-latency_logging <0/1>     Latency logging for decoder. Default \n"
        "\t                           disabled \n"
        "\t-i <input-file>            Name and path of input H.264/HEVC file \n"
        "\t                           \n"
        "Scaler options:\n\n"
        "\t-multiscale_xma            Name of the ABR scaler filter \n"
        "\t-num-output <value>        Number of output files from scaler \n"
        "\t-out_1_width <width>       Width of the scaler output channel 1 \n"
        "\t-out_1_height <height>     Height of the scaler output channel 1 \n"
        "\t-out_1_rate <full/half>    Full of Half rate for output channel 1 \n"
        "\t-out_2_width <width>       Width of the scaler output channel 2 \n"
        "\t-out_2_height <height>     Height of the scaler output channel 2 \n"
        "\t-out_2_rate <full/half>    Full of Half rate for output channel 2 \n"
        "\t-out_3_width <width>       Width of the scaler output channel 3 \n"
        "\t-out_3_height <height>     Height of the scaler output channel 3 \n"
        "\t-out_3_rate <full/half>    Full of Half rate for output channel 3 \n"
        "\t-out_4_width <width>       Width of the scaler output channel 4 \n"
        "\t-out_4_height <height>     Height of the scaler output channel 4 \n"
        "\t-out_4_rate <full/half>    Full of Half rate for output channel 4 \n"
        "\t-out_5_width <width>       Width of the scaler output channel 5 \n"
        "\t-out_5_height <height>     Height of the scaler output channel 5 \n"
        "\t-out_5_rate <full/half>    Full of Half rate for output channel 5 \n"
        "\t-out_6_width <width>       Width of the scaler output channel 6 \n"
        "\t-out_6_height <height>     Height of the scaler output channel 6 \n"
        "\t-out_6_rate <full/half>    Full of Half rate for output channel 6 \n"
        "\t-out_7_width <width>       Width of the scaler output channel 7 \n"
        "\t-out_7_height <height>     Height of the scaler output channel 7 \n"
        "\t-out_7_rate <full/half>    Full of Half rate for output channel 7 \n"
        "\t-out_8_width <width>       Width of the scaler output channel 8 \n"
        "\t-out_8_height <height>     Height of the scaler output channel 8 \n"
        "\t-out_8_rate <full/half>    Full of Half rate for output channel 8 \n"
        "\t-latency_logging <0/1>     Latency logging for scaler. Default \n"
        "\t                           disabled \n"
        "Encoder options:\n\n"
        "\t-c:v <codec>               Encoder codec to be used. Supported \n"
        "\t                           are mpsoc_vcu_hevc, mpsoc_vcu_h264 \n"
        "\t-b:v <bitrate>             Bitrate can be given in Kbps or Mbps \n"
        "\t                           or bits i.e., 5000000, 5000K, 5M. \n"
        "\t                           Default is 5000kbps \n"
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
        "\t            For HEVC, supported are 0 or main and 1 or main-intra. \n"
        "\t                           Default is 0/main.\n"
        "\t                           ENC_HEVC_MAIN - 0 or main. \n"
        "\t                           ENC_HEVC_MAIN_INTRA - 1 or main-intra. \n"
        "\t            For H264, supported are 66 or baseline, 77 or main, \n"
        "\t                           and 100 or high. \n"
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
xlnx_tran_parse_args: Function to parse command line arguments

Parameters:
argc: Number of arguments
argv: Pointer to the arguments
transcode_ctx: Transcoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_parse_args(int32_t argc, char *argv[], 
        XlnxTranscoderCtx *transcode_ctx)
{

    int32_t ret = TRANSCODE_APP_SUCCESS;
    int32_t flag = 0;
    int32_t option_index;
    int32_t i;

    flag = getopt_long_only(argc, argv, "", transcode_options, &option_index);
    if(flag == -1) {
        return TRANSCODE_APP_FAILURE;
    }
    while(flag != TRANSCODE_GENERIC_MAX_ARG)
    {
        switch (flag)
        {
            case HELP_ARG:
                printf("%s\n", xlnx_tran_get_help());
                exit(0);

            case TRANSCODE_DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&transcode_ctx->
                      app_xrm_ctx.device_id, optarg, FLAG_TRANSCODE_DEVICE_ID);
                if((transcode_ctx->app_xrm_ctx.device_id < 0) || 
                    (transcode_ctx->app_xrm_ctx.device_id > 15))
                {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Unsupported device ID %d \n", 
                               transcode_ctx->app_xrm_ctx.device_id);
                    return TRANSCODE_APP_FAILURE;
                }
                break;

            case TRANSCODE_STREAM_LOOP_ARG:
                ret = xlnx_utils_set_int_arg(&transcode_ctx->loop_count, optarg, FLAG_TRANSCODE_STREAM_LOOP);
                break;

            case TRANSCODE_NUM_FRAMES_ARG:
                ret = xlnx_utils_set_size_t_arg(&transcode_ctx->num_frames, optarg, FLAG_TRANSCODE_NUM_FRAMES);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error in parsing generic transcoder arguments %d \n", flag);
                return TRANSCODE_APP_FAILURE;
        }

        flag = getopt_long_only(argc, argv, "", transcode_options, 
                                &option_index);
        if(flag == -1 || ret == TRANSCODE_APP_FAILURE) {
            return TRANSCODE_APP_FAILURE;
        }
    }

    if((ret = xlnx_dec_parse_args(argc, argv, &transcode_ctx->dec_ctx, flag)) != 
            TRANSCODE_APP_SUCCESS){
        return TRANSCODE_APP_FAILURE;
    }

    flag = 0;
    if((ret = xlnx_scal_parse_args(argc, argv, &transcode_ctx->scal_ctx, &flag)) != 
            TRANSCODE_APP_SUCCESS){
        return TRANSCODE_APP_FAILURE;
    }

    transcode_ctx->num_scal_out = 
                       transcode_ctx->scal_ctx.scal_props.nb_outputs;
    transcode_ctx->num_enc_channels = (transcode_ctx->num_scal_out + 1);

    for(i = 0; i < transcode_ctx->num_enc_channels; i++) {
        if((ret = xlnx_enc_parse_args(argc, argv, &transcode_ctx->enc_ctx[i], 
            flag)) <= TRANSCODE_APP_FAILURE){
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error in parsing encoder arguments %d \n", flag);
            return TRANSCODE_APP_FAILURE;
        }
        else if(ret & TRANSCODE_PARSING_DONE) {
            break;
        }
        flag = 0;
    }
    transcode_ctx->num_enc_channels = i;
    transcode_ctx->non_scal_channels = (transcode_ctx->num_enc_channels - 
                                                transcode_ctx->num_scal_out);

    return ret;

}

/*-----------------------------------------------------------------------------
xlnx_tran_update_props: Function for updating transcoder properties

Parameters:
transcode_ctx: Transcoder context
transcode_props: Transcoder properties

return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static uint32_t xlnx_tran_update_props(XlnxTranscoderCtx *transcode_ctx, 
        XlnxTranscoderProperties *transcode_props)
{
    XlnxScalerProperties *scal_props = &transcode_ctx->scal_ctx.scal_props;
    xlnx_dec_update_props(&transcode_ctx->dec_ctx, 
                     &transcode_props->xma_dec_props);

    if(transcode_ctx->num_scal_out) {
        /* Update scaler width, height and fps from decoder context */
        scal_props->in_width = transcode_ctx->dec_ctx.frame_data.width;
        scal_props->in_height = transcode_ctx->dec_ctx.frame_data.height;
        scal_props->fr_num = transcode_ctx->dec_ctx.frame_data.fr_num;
        scal_props->fr_den = transcode_ctx->dec_ctx.frame_data.fr_den;

        xlnx_scal_update_props(&transcode_ctx->scal_ctx, 
                &transcode_props->xma_scal_props);
    }

    transcode_ctx->num_scal_fullrate = 
        transcode_ctx->scal_ctx.session_nb_outputs[SCAL_SESSION_FULL_RATE];

    for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {

        XlnxEncoderProperties *enc_props = &transcode_ctx->enc_ctx[i].enc_props;

        transcode_ctx->enc_ctx[i].la_in_frame = 
                &transcode_ctx->xma_app_frame[i];
        if((i == 0) && transcode_ctx->non_scal_channels) {
            enc_props->width = transcode_ctx->dec_ctx.frame_data.width;
            enc_props->height = transcode_ctx->dec_ctx.frame_data.height;
        }
        else {
            int32_t buf_idx = i - transcode_ctx->non_scal_channels;
            enc_props->width = scal_props->out_width[buf_idx];
            enc_props->height = scal_props->out_height[buf_idx];
        }
        if(enc_props->fps == ENC_DEFAULT_FRAMERATE) {
            if(i < (transcode_ctx->num_scal_fullrate + 
                    transcode_ctx->non_scal_channels)) {
                enc_props->fps = (transcode_ctx->dec_ctx.frame_data.fr_num/
                                transcode_ctx->dec_ctx.frame_data.fr_den);
            }
            else {
                enc_props->fps = (scal_props->fr_num/scal_props->fr_den);
            }
        }

        transcode_ctx->enc_ctx[i].xma_buffer = 
                        &transcode_ctx->xma_out_buffer[i];

        if((xlnx_enc_update_props(&transcode_ctx->enc_ctx[i], 
                &transcode_props->xma_enc_props[i])) == TRANSCODE_APP_FAILURE) {
            return TRANSCODE_APP_FAILURE;
        }
    }

    if(!transcode_ctx->non_scal_channels) {
        for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {
            transcode_ctx->enc_ctx[i].la_in_frame = 
                                   &transcode_ctx->xma_app_frame[i + 1];
        }
    }

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_tran_context_init: Initialize transcoder context params to default

Parameters:
transcode_ctx: Transcoder context
-----------------------------------------------------------------------------*/
static void xlnx_tran_context_init(XlnxTranscoderCtx *transcode_ctx)
{

    transcode_ctx->loop_count = 0;
    transcode_ctx->enc_chan_idx = 0;
    transcode_ctx->num_frames = SIZE_MAX;
    transcode_ctx->flush_mode = 0;
    transcode_ctx->eos_count = 0;
    transcode_ctx->out_frame_cnt = 0;
    transcode_ctx->in_frame_cnt = 0;
    transcode_ctx->transcoder_state = TRANSCODE_DEC_READ_FRAME;
    transcode_ctx->la_in_index = 0;
    transcode_ctx->dec_out_index = 0;
    transcode_ctx->num_scal_fullrate = 0;
    transcode_ctx->num_scal_out = 0;
    transcode_ctx->num_enc_channels = 0;
    transcode_ctx->curr_sess_channels = 0;
    transcode_ctx->non_scal_channels = 0;
    transcode_ctx->app_xrm_ctx.device_id = -1;
    transcode_ctx->app_xrm_ctx.reserve_idx = -1;

    transcode_ctx->dec_ctx.out_frame = &transcode_ctx->xma_app_frame[0];
    transcode_ctx->scal_ctx.in_frame = &transcode_ctx->xma_app_frame[0];
    for (int32_t i = 0; i < SCAL_MAX_ABR_CHANNELS; i++) {
        transcode_ctx->scal_ctx.out_frame[i] = 
                        &transcode_ctx->xma_app_frame[i+1];
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

/*-----------------------------------------------------------------------------
xlnx_tran_parser: Function for argument parsing and initialization

Parameters:
argc: Number of arguments
argv: Pointer to the arguments
transcode_ctx: Transcoder context
transcode_props: Transcoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_tran_parser(int32_t argc, char *argv[], 
        XlnxTranscoderCtx *transcode_ctx,
        XlnxTranscoderProperties *transcode_props)
{

    /* Initializing the transcoder context with default values */
    xlnx_tran_context_init(transcode_ctx);
    if(xlnx_tran_parse_args(argc, argv, transcode_ctx) <= 
            TRANSCODE_APP_FAILURE) {
        return TRANSCODE_APP_FAILURE;
    }

    if((xlnx_dec_parse_frame(&transcode_ctx->dec_ctx)) != 
            TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error in decoder parser \n");
        return TRANSCODE_APP_FAILURE;
    }

    if((xlnx_tran_update_props(transcode_ctx, transcode_props)) == 
            TRANSCODE_APP_FAILURE) {
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;
}
