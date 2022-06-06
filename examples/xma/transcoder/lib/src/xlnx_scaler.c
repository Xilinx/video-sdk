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

#include "xlnx_scaler.h"

static struct option scal_options[] = {
    {FLAG_SCAL_FILTER, no_argument, 0, SCAL_FILTER_ARG},
    {FLAG_SCAL_CONVERT_INPUT, no_argument, 0, SCAL_CONVERT_INPUT_ARG},
    {FLAG_SCAL_NUM_OUTPUTS, required_argument, 0, SCAL_NUM_OUTPUTS_ARG},
    {FLAG_SCAL_DEVICE_ID, required_argument, 0, SCAL_DEVICE_ID_ARG},
    {FLAG_SCAL_ENABLE_PIPELINE, required_argument, 0, SCAL_ENABLE_PIPELINE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("1"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("1"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("1"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("2"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("2"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("2"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("3"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("3"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("3"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("4"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("4"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("4"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("5"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("5"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("5"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("6"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("6"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("6"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("7"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("7"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("7"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_OUTPUT_WIDTH("8"), required_argument, 0, SCAL_OUTPUT_WIDTH_ARG},
    {FLAG_SCAL_OUTPUT_HEIGHT("8"), required_argument, 0,
     SCAL_OUTPUT_HEIGHT_ARG},
    {FLAG_SCAL_OUTPUT_RATE("8"), required_argument, 0, SCAL_OUTPUT_RATE_ARG},
    {FLAG_SCAL_LATENCY_LOGGING, required_argument, 0, SCAL_LATENCY_LOGGING_ARG},
    {FLAG_SCAL_MAX, required_argument, 0, SCAL_MAX_ARG},
    {0, 0, 0, 0}};

/**
 * xlnx_scal_validate_rate_config: Validates the session configuration
 *
 * @param scal_ctx: Scaler context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_scal_validate_rate_config(XlnxScalerProperties* scal_props)
{
    int32_t i, ret;
    int32_t count = 0;

    /* All outputs @half-rate not supported */
    for(i = 0; i < scal_props->nb_outputs; ++i) {
        if(strcmp(scal_props->out_rate[i], "half") == 0) {
            count += 1;
        } else if(strcmp(scal_props->out_rate[i], "full") != 0) {
            return -2;
        }
    }
    ret = ((scal_props->nb_outputs == count) ? XMA_APP_ERROR : XMA_APP_SUCCESS);
    return (ret);
}

/**
 * xlnx_scal_num_sessions: Calculates the number of scaler sessions.
 *
 * @param scal_ctx: Scaler context
 * @return Number of scaler output sessions
 */
static int32_t xlnx_scal_num_sessions(XlnxScalerProperties* scal_props)
{
    int32_t i;
    int32_t count = 1;

    /* default = 1 session - full rate
       However if Mix out_rate is found then 2 sessions will
       be created to allow for frame drops.
     */
    for(i = 0; i < scal_props->nb_outputs; ++i) {
        if(strcmp(scal_props->out_rate[i], "full") != 0) {
            count = 2;
            break;
        }
    }
    return (count);
}

/**
 * xlnx_scal_num_full_rate: Calculates the number of scaler full rate sessions.
 *
 * @param scal_ctx: Scaler context
 * @return Number of scaler full-rate output sessions
 */
static int32_t xlnx_scal_num_full_rate(XlnxScalerProperties* scal_props)
{
    int32_t i;
    int32_t count                 = 0;
    bool    have_gotten_half_rate = 0;

    for(i = 0; i < scal_props->nb_outputs; ++i) {
        if(strcmp(scal_props->out_rate[i], "full") == 0) {
            if(have_gotten_half_rate) {
                xma_logmsg(
                    XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    " Full rate specified after half rate! Full rate outputs"
                    " must preceed half rates. Output id %d\n",
                    i);
                return XMA_APP_ERROR;
            }
            count += 1;
        } else {
            have_gotten_half_rate = 1;
        }
    }
    return (count);
}

/**
 * xlnx_scal_update_props: Updates xma scaler properties and options that will
 * be sent to xma plugin
 *
 * @param scal_ctx: Scaler context
 * @param xma_scal_props: XMA scaler properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_scal_update_props(XlnxScalerCtx*       scal_ctx,
                               XmaScalerProperties* xma_scal_props)
{

    int32_t               ret        = XMA_APP_ERROR;
    XlnxScalerProperties* scal_props = &scal_ctx->scal_props;

    /* validate rate configuration params */
    ret = xlnx_scal_validate_rate_config(scal_props);
    if(ret == -1) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "In Scaler, all outputs at half-rate not supported\n");
        return XMA_ERROR;
    } else if(ret == -2) {
        xma_logmsg(
            XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
            "In Scaler, outputs rate config shall be 'half' or 'full'\n");
        return XMA_ERROR;
    }

    /* determine num sessions to create */
    scal_ctx->num_sessions = xlnx_scal_num_sessions(scal_props);

    /* All-rate session includes all outputs */
    scal_ctx->session_nb_outputs[SCAL_SESSION_ALL_RATE] =
        scal_props->nb_outputs;

    if(scal_ctx->num_sessions > 1) {
        scal_ctx->session_nb_outputs[SCAL_SESSION_FULL_RATE] =
            xlnx_scal_num_full_rate(scal_props);
        if(scal_ctx->session_nb_outputs[SCAL_SESSION_FULL_RATE] < 0) {
            return XMA_APP_ERROR;
        }

        /* 2 sessions with half input frame-rate each */
        scal_props->fr_num /= 2;
    } else {
        scal_ctx->session_nb_outputs[SCAL_SESSION_FULL_RATE] =
            scal_props->nb_outputs;
    }

    if(scal_props->bits_per_pixel == BITS_PER_PIXEL_8) {
        scal_props->xma_fmt_type = XMA_VCU_NV12_FMT_TYPE;
    } else if(scal_props->bits_per_pixel == BITS_PER_PIXEL_10) {
        scal_props->xma_fmt_type = XMA_VCU_NV12_10LE32_FMT_TYPE;
    }

    xlnx_scal_get_xma_props(scal_props, xma_scal_props);

    return ret;
}

/**
 * xlnx_scal_context_init: Initializes the scaler context with default values.
 *
 * @param scal_ctx: Scaler context
 */
void xlnx_scal_context_init(XlnxScalerCtx* scal_ctx)
{

    scal_ctx->scal_props.nb_outputs        = 0;
    scal_ctx->frames_out                   = 0;
    scal_ctx->session_nb_outputs[0]        = 0;
    scal_ctx->session_nb_outputs[1]        = 0;
    scal_ctx->scal_props.dev_index         = DEFAULT_DEVICE_ID;
    scal_ctx->scal_props.out_width[0]      = 1280;
    scal_ctx->scal_props.out_height[0]     = 720;
    scal_ctx->scal_props.out_width[1]      = 852;
    scal_ctx->scal_props.out_height[1]     = 480;
    scal_ctx->scal_props.out_width[2]      = 640;
    scal_ctx->scal_props.out_height[2]     = 360;
    scal_ctx->scal_props.out_width[3]      = 424;
    scal_ctx->scal_props.out_height[3]     = 240;
    scal_ctx->scal_props.enable_pipeline   = 0;
    scal_ctx->scal_props.log_level         = 0;
    scal_ctx->scal_props.p_mixrate_session = 0;
    scal_ctx->scal_props.latency_logging   = 0;
    scal_ctx->session_frame                = 0;
    scal_ctx->scal_props.fr_num            = 25;
    scal_ctx->scal_props.fr_den            = 1;

    for(int32_t i = 0; i < SCAL_MAX_ABR_CHANNELS; i++) {
        memset(scal_ctx->out_frame[i], 0, sizeof(XmaFrame));
        scal_ctx->out_frame[i]->data[0].buffer_type = XMA_DEVICE_BUFFER_TYPE;
        strcpy(scal_ctx->scal_props.out_rate[i], "full");
    }

    return;
}

/**
 * scaler_session: Creates scaler session
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param scal_ctx: Scaler context
 * @param xma_scal_props: XMA scaler properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_scal_session(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XlnxScalerCtx*        scal_ctx,
                          XmaScalerProperties*  xma_scal_props)
{

    for(int32_t count = 0; count < scal_ctx->num_sessions; ++count) {

        scal_ctx->xrm_scalres_count = count;
        xma_scal_props->num_outputs = scal_ctx->session_nb_outputs[count];

        if(xlnx_scal_cu_alloc(app_xrm_ctx, xma_scal_props,
                              &scal_ctx->scal_cu_list_res[count]) !=
           XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Error in scaler CU allocation\n");
            return XMA_APP_ERROR;
        }

        scal_ctx->session[count] = xma_scaler_session_create(xma_scal_props);
        if(!scal_ctx->session) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to create scaler session\n");
            return XMA_APP_ERROR;
        }
        scal_ctx->scal_props.p_mixrate_session =
            (uint64_t)scal_ctx->session[count];
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_validate_arguments: Validates scaler arguments
 *
 * @param scal_props: Scaler properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t
    xlnx_scal_validate_arguments(const XlnxScalerProperties* scal_props)
{
    if((scal_props->nb_outputs < 0) ||
       (scal_props->nb_outputs > SCAL_MAX_ABR_CHANNELS)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid number of scaler outputs %d\n",
                   scal_props->nb_outputs);
        return XMA_APP_ERROR;
    }
    if(scal_props->dev_index < -1 ||
       scal_props->dev_index > XLNX_MAX_DEVICE_COUNT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported device ID %d\n", scal_props->dev_index);
        return XMA_APP_ERROR;
    }
    for(int32_t i = 0; i < scal_props->nb_outputs; i++) {
        if((scal_props->out_width[i] < SCAL_MIN_INPUT_WIDTH) ||
           (scal_props->out_width[i] > SCAL_MAX_INPUT_WIDTH)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Invalid scaler width %d\n", scal_props->out_width[i]);
            return XMA_APP_ERROR;
        }

        if((scal_props->out_height[i] < SCAL_MIN_INPUT_HEIGHT) ||
           (scal_props->out_height[i] > SCAL_MAX_INPUT_HEIGHT)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Invalid scaler height %d\n", scal_props->out_height[i]);
            return XMA_APP_ERROR;
        }

        if((strcmp(scal_props->out_rate[i], "half") != 0) &&
           (strcmp(scal_props->out_rate[i], "full") != 0)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Invalid scaler output rate %s\n",
                       scal_props->out_rate[i]);
            return XMA_APP_ERROR;
        }
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_scal_parse_args: Parses the command line arguments of scaler
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param scal_ctx: Scaler context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_scal_parse_args(int32_t argc, char* argv[],
                             XlnxScalerCtx* scal_ctx, int32_t* param_flag)
{
    int32_t               option_index;
    int32_t               flag       = 0;
    int8_t                abr_idx    = -1;
    int32_t               ret        = XMA_APP_SUCCESS;
    XlnxScalerProperties* scal_props = &scal_ctx->scal_props;
    char                  value[6];
    char                  abr_props[15];

    flag = getopt_long_only(argc, argv, "", scal_options, &option_index);
    if(flag == -1) {
        return XMA_APP_ERROR;
    }

    while(flag != SCAL_MAX_ARG) {
        switch(flag) {
            case SCAL_FILTER_ARG:
                scal_ctx->scaler_enable = 1;
                break;

            case SCAL_NUM_OUTPUTS_ARG:
                ret = xlnx_utils_set_int_arg(&scal_props->nb_outputs, optarg,
                                             FLAG_SCAL_NUM_OUTPUTS);
                break;

            case SCAL_DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&scal_props->dev_index, optarg,
                                             FLAG_SCAL_DEVICE_ID);
                break;

            case SCAL_CONVERT_INPUT_ARG:
                ret                     = XMA_APP_SUCCESS;
                scal_ctx->convert_input = true;
                break;

            case SCAL_OUTPUT_WIDTH_ARG:
                abr_idx++;
                sprintf(value, "%d", abr_idx);
                sprintf(abr_props, "%s%s%s", OUTPUT, value, WIDTH);
                ret = xlnx_utils_set_int_arg(&scal_props->out_width[abr_idx],
                                             optarg, abr_props);
                break;

            case SCAL_OUTPUT_HEIGHT_ARG:
                sprintf(value, "%d", abr_idx);
                sprintf(abr_props, "%s%s%s", OUTPUT, value, HEIGHT);
                ret = xlnx_utils_set_int_arg(&scal_props->out_height[abr_idx],
                                             optarg, abr_props);
                break;

            case SCAL_OUTPUT_RATE_ARG:
                strcpy(scal_props->out_rate[abr_idx], optarg);
                break;

            case SCAL_ENABLE_PIPELINE_ARG:
                ret = xlnx_utils_set_uint_arg(&scal_props->enable_pipeline,
                                             optarg, FLAG_SCAL_ENABLE_PIPELINE);
                break;

            case SCAL_LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&scal_props->latency_logging,
                                             optarg, FLAG_SCAL_LATENCY_LOGGING);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Error in parsing scaler arguments %d\n", flag);
                return XMA_APP_ERROR;
        }

        flag = getopt_long_only(argc, argv, "", scal_options, &option_index);
        if(flag == -1 || ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
    }
    *param_flag = flag;

    return xlnx_scal_validate_arguments(scal_props);
}

/**
 * xlnx_scal_send_frame: Sends YUV input to the scaler
 *
 * @param scal_ctx: Scaler context
 * @param curr_session: Scaler session
 * @return XMA_APP_SUCCESS or ERROR
 */
static int32_t xlnx_scal_send_frame(XlnxScalerCtx*    scal_ctx,
                                    XmaScalerSession* curr_session)
{
    int32_t   ret        = XMA_APP_ERROR;
    XmaFrame* send_frame = scal_ctx->send_frame;
    if(scal_ctx->convert_input) {
        /* scal_ctx->in_frame == dec out frame */
        if(scal_ctx->in_frame->data[0].buffer != NULL) {
            if(xlnx_utils_copy_dev_frame_to_host_frame(
                   scal_ctx->in_frame, send_frame) != XMA_APP_SUCCESS) {
                return XMA_APP_ERROR;
            }
            /* This will check/decrement the ref cnt before freeing. */
            xvbm_buffer_pool_entry_free(scal_ctx->in_frame->data[0].buffer);
        } else {
            free(send_frame->data[0].buffer);
            send_frame->data[0].buffer = NULL;
        }
    }
    ret = xma_scaler_session_send_frame(curr_session, send_frame);
    return ret;
}

/**
 * xlnx_scal_recv_frame: Receives Scaler output
 *
 * @param scal_ctx: Scaler context
 * @param curr_session: Scaler session
 * @return XMA_APP_SUCCESS or ERROR
 */
static int32_t xlnx_scal_recv_frame(XlnxScalerCtx*    scal_ctx,
                                    XmaScalerSession* curr_session)
{

    int32_t ret = XMA_APP_ERROR;

    ret = xma_scaler_session_recv_frame_list(curr_session, scal_ctx->out_frame);

    return ret;
}

/**
 * xlnx_scal_process_frame: Process a scaler frame
 *
 * @param scal_ctx: Scaler context
 * @return The result of recv if send was successful, the result of send if
 * it was not success or error, XMA_APP_ERROR on error.
 */
int32_t xlnx_scal_process_frame(XlnxScalerCtx* scal_ctx)
{

    int32_t           ret = XMA_APP_ERROR;
    int32_t           session_type;
    XmaScalerSession* curr_session;

    if(scal_ctx->num_sessions > 1) {
        /* Odd Frame = SCAL_SESSION_FULL_RATE,
           Even Frame = SCAL_SESSION_ALL_RATE */
        session_type =
            ((scal_ctx->session_frame & 0x01) ? SCAL_SESSION_FULL_RATE :
                                                SCAL_SESSION_ALL_RATE);
        scal_ctx->session_frame =
            (scal_ctx->session_frame + 1) % SCAL_MAX_SESSIONS;
    } else {
        session_type = SCAL_SESSION_ALL_RATE;
    }
    curr_session = scal_ctx->session[session_type];

    ret = xlnx_scal_send_frame(scal_ctx, curr_session);
    if((ret == XMA_SUCCESS) || (ret == XMA_FLUSH_AGAIN)) {

        if((ret = xlnx_scal_recv_frame(scal_ctx, curr_session)) <=
           XMA_APP_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Failed to receive scaler frame list\n");
            return XMA_APP_ERROR;
        }
    } else if(ret <= XMA_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Failed to send scaler frame, error %d\n", ret);
        return XMA_APP_ERROR;
    }
    return ret;
}

/**
 * xlnx_scal_deinit: Deinitialize scaler module
 *
 * @param xrm_ctx: XRM context
 * @param scal_ctx: Scaler context
 * @param xma_scal_props: XMA scaler properties
 * @return XMA_APP_SUCCESS or ERROR
 */
int32_t xlnx_scal_deinit(xrmContext xrm_ctx, XlnxScalerCtx* scal_ctx,
                         XmaScalerProperties* xma_scal_props)
{

    int32_t ret = XMA_APP_ERROR;

    for(int32_t i = 0; i < scal_ctx->num_sessions; i++) {
        if(scal_ctx->session[i] != NULL) {
            ret = xma_scaler_session_destroy(scal_ctx->session[i]);
        }
    }

    for(int32_t i = 0; i < scal_ctx->num_sessions; i++)
        ret = xrmCuListReleaseV2(xrm_ctx, &scal_ctx->scal_cu_list_res[i]);

    xlnx_scal_free_xma_props(xma_scal_props);

    return ret;
}
