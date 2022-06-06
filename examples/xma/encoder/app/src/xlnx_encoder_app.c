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

#include "xlnx_encoder_app.h"

bool signal_caught;

/**
 * xlnx_enc_signal_handler: Signal handler function
 *
 * @param signum: Signal number
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
void xlnx_enc_signal_handler(int32_t signum)
{
    switch(signum) {
        case SIGTERM:
        case SIGINT:
        case SIGABRT:
        case SIGHUP:
        case SIGQUIT:
            signal_caught = true;
            break;
    }
}

/**
 * xlnx_enc_set_signal_handler: Signal handler initialization.
 *
 * @param None
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_set_signal_handler()
{

    struct sigaction action;
    action.sa_handler = xlnx_enc_signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGHUP, &action, NULL);
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_app_close: Deinitialize encoder application
 *
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @param xma_la_props: XMA lookahead properties
 * @return None
 */
static void xlnx_enc_app_close(XlnxEncoderAppCtx* ctx)
{
    xlnx_enc_deinit(&ctx->enc_ctx);
    xlnx_la_deinit(&ctx->la_ctx);
    if(ctx->in_file) {
        fclose(ctx->in_file);
    }
    if(ctx->out_file) {
        fclose(ctx->out_file);
    }
    if(ctx->enc_dyn_params.dyn_params_lib) {
        (*(ctx->enc_dyn_params.dyn_params_obj.xlnx_enc_deinit_dyn_params))(
            ctx->enc_dyn_params.dynamic_param_handle);
        dlclose(ctx->enc_dyn_params.dyn_params_lib);
    }

    return;
}

/**
 * xlnx_enc_create_app_ctx: Create the encoder app context - create encoder
 * context with encoder and lookahead settings, open input/output files, set app
 * level encoding options.
 *
 * @param arguments: The arguments struct containing enc settings, lookahead
 * settings,
 * @param and app level encoding information
 * @param ctx: The encoder app context to be created.
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_create_app_ctx(XlnxEncoderArguments* arguments,
                                       XlnxEncoderAppCtx*    ctx)
{
    ctx->in_file = fopen(arguments->input_file, "rb");
    if(!ctx->in_file) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unable to open input file %s for reading\n",
                   arguments->input_file);
        return XMA_APP_ERROR;
    }
    ctx->pix_fmt  = arguments->pix_fmt;
    ctx->out_file = fopen(arguments->output_file, "wb");
    if(!ctx->out_file) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Unable to open output file %s for writing\n",
                   arguments->output_file);
        return XMA_APP_ERROR;
    }
    ctx->loop_count           = arguments->loop_count;
    ctx->num_frames_to_encode = arguments->num_frames;
    ctx->enc_ctx.dynamic_idr  = &arguments->dynamic_idr;

    if(xlnx_enc_create_enc_ctx(&arguments->enc_props, &ctx->enc_ctx) !=
       XMA_APP_SUCCESS) {

        return XMA_APP_ERROR;
    }

    xlnx_enc_copy_enc_props_to_la(arguments);

    if(xlnx_la_create_ctx(&arguments->la_props, &ctx->la_ctx) !=
       XMA_APP_SUCCESS) {

        return XMA_APP_ERROR;
    }

    if(!ctx->la_ctx.bypass) {
        ctx->enc_input_xframe = (XmaFrame*)calloc(1, sizeof(XmaFrame));
    }

    return XMA_APP_SUCCESS;
}

static int32_t xlnx_enc_start_fpga_session(XlnxEncoderAppCtx* ctx)
{
    XlnxEncoderCtx*   enc_ctx = &ctx->enc_ctx;
    XlnxLookaheadCtx* la_ctx  = &ctx->la_ctx;
    /* Reserving the CU resource and perform XMA initialize */
    if(xlnx_enc_device_init(&enc_ctx->enc_xrm_ctx, &enc_ctx->xma_enc_props) !=
       XMA_APP_SUCCESS) {

        return XMA_APP_ERROR;
    }
    if(xlnx_enc_cu_alloc(&enc_ctx->enc_xrm_ctx, &enc_ctx->xma_enc_props) !=
       XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    if(!la_ctx->bypass) {
        if(xlnx_la_cu_alloc(&enc_ctx->enc_xrm_ctx, &la_ctx->xma_la_props) !=
           XMA_APP_SUCCESS) {
            return XMA_APP_ERROR;
        }
    }

    enc_ctx->enc_session = xma_enc_session_create(&enc_ctx->xma_enc_props);
    if(!enc_ctx->enc_session) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Failed to create encoder session\n");
        return XMA_APP_ERROR;
    }
    if(!la_ctx->bypass) {
        la_ctx->filter_session =
            xma_filter_session_create(&la_ctx->xma_la_props);
        if(!la_ctx->filter_session) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Failed to create look ahead session\n");
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_print_total_fps: Print the total performance of the encoder.
 *
 * @param ctx: Encoder app context
 * @return None
 */
void xlnx_enc_print_total_fps(XlnxEncoderAppCtx* ctx)
{
    size_t num_frames_encoded =
        min(ctx->enc_ctx.num_frames_received, ctx->num_frames_to_encode);
    double time_taken = xlnx_utils_get_total_time(&ctx->timer);
    fprintf(stderr, "\nFrames Encoded: %zu, Time Elapsed: %.03lf\r\n",
            num_frames_encoded, time_taken);
    fprintf(stderr, "Total FPS: %.03lf\r\n", num_frames_encoded / time_taken);
}

/**
 * xlnx_enc_print_segment_fps: Calculate and print fps for every second
 *
 * @param ctx: Encoder app context
 * @return None
 */
static void xlnx_enc_print_segment_fps(XlnxEncoderAppCtx* ctx)
{

    double segment_time = xlnx_utils_get_segment_time(&ctx->timer);
    if(segment_time < 1) {
        return;
    }
    size_t num_frames_encoded = ctx->enc_ctx.num_frames_received;
    fprintf(stderr, "\rFrame=%5zu Current FPS=%.03f Total FPS=%.03f \r",
            num_frames_encoded,
            (num_frames_encoded - ctx->timer.last_displayed_frame) /
                segment_time,
            (float)num_frames_encoded / xlnx_utils_get_total_time(&ctx->timer));
    fflush(stderr);
    ctx->timer.last_displayed_frame = ctx->enc_ctx.num_frames_received;
    xlnx_utils_set_segment_time(&ctx->timer);
}

static int32_t xlnx_enc_read_frame(XlnxEncoderAppCtx* enc_app_ctx)
{
    const XlnxEncoderProperties* enc_props    = enc_app_ctx->enc_ctx.enc_props;
    XmaFrame*                    input_xframe = &enc_app_ctx->la_input_xframe;
    if(input_xframe->data[0].refcount <= 0) {
        xlnx_enc_create_xframe(enc_app_ctx->enc_ctx.enc_props, input_xframe);
    }
    int ret = xlnx_utils_read_frame(enc_props->width, enc_props->height,
                                    enc_app_ctx->pix_fmt, enc_app_ctx->in_file,
                                    &input_xframe->data[0].buffer,
                                    &input_xframe->data[1].buffer);
    if(ret == XMA_APP_EOF) {
        if(enc_app_ctx->loop_count > 0 || enc_app_ctx->loop_count == -1) {
            fseek(enc_app_ctx->in_file, 0, SEEK_SET);
            if(enc_app_ctx->loop_count != -1) {
                enc_app_ctx->loop_count--;
            }
            ret = xlnx_utils_read_frame(
                enc_props->width, enc_props->height, enc_app_ctx->pix_fmt,
                enc_app_ctx->in_file, &input_xframe->data[0].buffer,
                &input_xframe->data[1].buffer);
        }
    }
    if(ret == XMA_APP_EOF) {
        int num_planes = xma_frame_planes_get(&input_xframe->frame_props);
        for(int plane_id = 0; plane_id < num_planes; plane_id++) {
            free(input_xframe->data[plane_id].buffer);
            input_xframe->data[plane_id].buffer = NULL;
        }
        if(enc_app_ctx->enc_ctx.num_frames_received == 0) {
            long int num_frames_in_src =
                ftell(enc_app_ctx->in_file) /
                ((enc_props->width * enc_props->height * 3) >> 1);
            if(num_frames_in_src == 0) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "No frames in input file!\n");
                return XMA_APP_ERROR;
            } else if(num_frames_in_src < enc_props->lookahead_depth) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Source file must have more frames than lookahead "
                           "depth! Num frames in src: %ld; lookahead depth: %d\n",
                           num_frames_in_src, enc_props->lookahead_depth);
                return XMA_APP_ERROR;
            }
        }
    }
    return ret;
}

/**
 * xlnx_enc_set_if_idr_frame: Set frames as IDR frames at run time
 *
 * @param enc_ctx: Encoder context
 * @param input_xframe: Lookahead input frame
 * @param enc_frame_cnt: Encoder frame number
 */
static void xlnx_enc_set_if_idr_frame(XlnxEncoderCtx* enc_ctx,
                                      XmaFrame*       input_xframe,
                                      uint32_t        enc_frame_cnt)
{
    uint32_t* idr_arr     = enc_ctx->dynamic_idr->idr_arr;
    size_t    len_idr_arr = enc_ctx->dynamic_idr->len_idr_arr;
    uint32_t  i           = enc_ctx->dynamic_idr->idr_arr_idx;

    input_xframe->is_idr = 0;
    for(; i < len_idr_arr; i++) {
        if(idr_arr[i] == enc_frame_cnt) {
            input_xframe->is_idr = 1;
            break;
        } else if(idr_arr[i] > enc_frame_cnt) {
            break;
        }
    }

    enc_ctx->dynamic_idr->idr_arr_idx = i;
}

/**
 * xlnx_get_runtime_dyn_params: Gets encoder runtime dynamic parameters
 *
 * @param enc_app_ctx: Encoder app context
 * @param la_in_frame: Lookahead input frame
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_get_runtime_dyn_params(XlnxEncoderAppCtx* enc_app_ctx,
                                           XmaFrame*          la_input_xframe)
{
    XlnxEncoderCtx*   enc_ctx        = &enc_app_ctx->enc_ctx;
    XlnxLookaheadCtx* la_ctx         = &enc_app_ctx->la_ctx;
    EncDynParams*     enc_dyn_params = &enc_app_ctx->enc_dyn_params;
    uint32_t          enc_frame_cnt;

    if(enc_ctx->enc_props->lookahead_depth > 0) {
        enc_frame_cnt = la_ctx->num_frames_sent;
    } else {
        enc_frame_cnt = enc_ctx->num_frames_sent;
    }

    /* Check for Dynamic IDR option */
    if(enc_ctx->dynamic_idr->len_idr_arr) {
        xlnx_enc_set_if_idr_frame(enc_ctx, la_input_xframe, enc_frame_cnt);
    }

    /* Check if dynamic encoder parameters are present and add them as frame
    side data */
    if((enc_dyn_params->dynamic_params_count > 0) &&
       (enc_dyn_params->dynamic_params_index <
        enc_dyn_params->dynamic_params_count)) {
        uint32_t dyn_frame_num = (*(
            enc_dyn_params->dyn_params_obj.xlnx_enc_get_dyn_param_frame_num))(
            enc_dyn_params->dynamic_param_handle,
            enc_dyn_params->dynamic_params_index);

        if(dyn_frame_num == (enc_frame_cnt)) {
            uint32_t num_b_frames = (*(
                enc_dyn_params->dyn_params_obj.xlnx_enc_get_runtime_b_frames))(
                enc_dyn_params->dynamic_param_handle,
                enc_dyn_params->dynamic_params_index);
            /* Dynamic b-frames have to be less than or equal to number of
            B-frames specified on the command line or default value, whichever
            is set at the beginning of encoding*/
            if(num_b_frames > enc_ctx->enc_props->num_bframes) {
                xma_logmsg(
                    XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                    "Dynamic b-frames=%d at frameNum=%d cannot be greater than "
                    "initial number of b-frames=%d\n",
                    num_b_frames, dyn_frame_num,
                    enc_ctx->enc_props->num_bframes);
                return XMA_APP_ERROR;
            }

            /* If tune-metrics is enabled, then reset all the AQ parameters */
            if(enc_ctx->enc_props->tune_metrics) {
                (*(enc_dyn_params->dyn_params_obj
                       .xlnx_enc_reset_runtime_aq_params))(
                    enc_dyn_params->dynamic_param_handle,
                    enc_dyn_params->dynamic_params_index);
            }

            if((*(enc_dyn_params->dyn_params_obj.xlnx_enc_add_dyn_params))(
                   enc_dyn_params->dynamic_param_handle, la_input_xframe,
                   enc_dyn_params->dynamic_params_index) != XMA_APP_SUCCESS) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Failed to add dynamic parameters as side data\n");
                return XMA_APP_ERROR;
            }

            enc_dyn_params->dynamic_params_index++;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_frame_process: Process an input YUV frame
 *
 * @param enc_ctx: Encoder context
 * @param enc_stop: Encoder stop flag
 * @return XMA_SUCCESS, XMA_ERROR, or XMA_EOS
 */
static int32_t xlnx_enc_frame_process(XlnxEncoderAppCtx* enc_app_ctx)
{
    XlnxEncoderCtx*   enc_ctx          = &enc_app_ctx->enc_ctx;
    XlnxLookaheadCtx* la_ctx           = &enc_app_ctx->la_ctx;
    XmaFrame*         la_input_xframe  = &enc_app_ctx->la_input_xframe;
    XmaFrame*         enc_input_xframe = enc_app_ctx->enc_input_xframe;

    /* Read input from file into la input xframe */
    int32_t ret = xlnx_enc_read_frame(enc_app_ctx);
    if(ret <= XMA_APP_ERROR) {
        return ret;
    }

    if((ret = xlnx_get_runtime_dyn_params(enc_app_ctx, la_input_xframe)) !=
       XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "xlnx_get_runtime_dyn_params failed with error %d\n", ret);
        return ret;
    }

    /* Process la input frame. Output written into enc input frame */
    ret = xlnx_la_process_frame(la_ctx, la_input_xframe, &enc_input_xframe);
    if(ret == XMA_ERROR || ret == XMA_SEND_MORE_DATA) {
        return ret;
    } else if(ret == XMA_EOS) {
        la_ctx->bypass = 1;
    }

    int32_t recv_size = 0;
    ret = xlnx_enc_process_frame(enc_ctx, enc_input_xframe, &recv_size);
    /* num_frames_received incremented in xlnx_enc_process_frame. Therefore if
    check should be '<=' not '<' */
    if(ret == XMA_SUCCESS && enc_app_ctx->enc_ctx.num_frames_received <=
                                 enc_app_ctx->num_frames_to_encode) {
        /* Encoder output frame received */
        fwrite(enc_ctx->output_xma_buffer.data.buffer, 1, recv_size,
               enc_app_ctx->out_file);
    }

    if(enc_input_xframe && !enc_ctx->flush_sent) {
        /* Let the lookahead plugin know it can stop storing its buffer */
        xlnx_la_release_frame(la_ctx, enc_input_xframe);
    }
    return ret;
}

static int32_t xlnx_enc_encode_file(XlnxEncoderAppCtx* ctx)
{
    /* Setting signal handler for SIGINT, SIGTERM and SIGHUP */
    xlnx_enc_set_signal_handler();
    xlnx_utils_set_non_blocking(1);
    xlnx_utils_start_tracking_time(&ctx->timer);
    /* Encoder loop */
    int ret = XMA_APP_SUCCESS;
    do {
        if(xlnx_utils_was_q_pressed() || signal_caught ||
           ctx->enc_ctx.num_frames_received >= ctx->num_frames_to_encode) {

            fseek(ctx->in_file, 0, SEEK_END);
            ctx->loop_count = 0;
        }
        ret = xlnx_enc_frame_process(ctx);
        xlnx_enc_print_segment_fps(ctx);
    } while(ret != XMA_ERROR && ret != XMA_EOS);
    xma_logmsg(XMA_NOTICE_LOG, XLNX_ENC_APP_MODULE,
               "Encoding of input stream completed\n");
    xlnx_utils_set_non_blocking(0);
    return ret == XMA_ERROR ? XMA_ERROR : XMA_SUCCESS;
}

/**
 * xlnx_load_dyn_params_lib: Loads dynamic params shared library and stores it's
 * symbols in encoder app context
 *
 * @param ctx: Encoder app context
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_load_dyn_params_lib(XlnxEncoderAppCtx* ctx)
{
    char* dlret;

    ctx->enc_dyn_params.dyn_params_lib = dlopen(DYN_PARAMS_LIB_NAME, RTLD_NOW);
    if(!ctx->enc_dyn_params.dyn_params_lib) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Error loading %s "
                   ": %s\n",
                   DYN_PARAMS_LIB_NAME, dlerror());
        return XMA_APP_ERROR;
    }
    xma_logmsg(XMA_DEBUG_LOG, XLNX_ENC_APP_MODULE,
               "Dynamic params plugin path:"
               " %s \n",
               DYN_PARAMS_LIB_NAME);

    ctx->enc_dyn_params.xlnx_enc_init_dyn_params_obj = (InitDynParams)dlsym(
        ctx->enc_dyn_params.dyn_params_lib, XLNX_ENC_INIT_DYN_PARAMS_OBJ);
    dlret = dlerror();
    if(dlret != NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                   "Error loading symbol "
                   "%s from %s plugin: %s\n",
                   XLNX_ENC_INIT_DYN_PARAMS_OBJ, DYN_PARAMS_LIB_NAME, dlret);
        return XMA_APP_ERROR;
    }

    (*(ctx->enc_dyn_params.xlnx_enc_init_dyn_params_obj))(
        &ctx->enc_dyn_params.dyn_params_obj);

    return XMA_APP_SUCCESS;
}

/**
 * main: Main function for xma encoder app
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t main(int32_t argc, char* argv[])
{
    XlnxEncoderArguments arguments;
    if(xlnx_enc_get_arguments(argc, argv, &arguments) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    XlnxEncoderAppCtx ctx;
    memset(&ctx, 0, sizeof(ctx));

    if(xlnx_enc_create_app_ctx(&arguments, &ctx) != XMA_APP_SUCCESS) {
        xlnx_enc_app_close(&ctx);
        return XMA_APP_ERROR;
    }

    if(arguments.expert_options.dynamic_params_check) {
        if(xlnx_load_dyn_params_lib(&ctx) != XMA_APP_SUCCESS) {
            return XMA_APP_ERROR;
        }
        ctx.enc_dyn_params.dynamic_param_handle = (DynparamsHandle)(
            *(ctx.enc_dyn_params.dyn_params_obj.xlnx_enc_get_dyn_params))(
            arguments.expert_options.dynamic_params_file,
            &ctx.enc_dyn_params.dynamic_params_count);

        if(ctx.enc_dyn_params.dynamic_param_handle == NULL) {
            return XMA_APP_ERROR;
        }
    }

    if(xlnx_enc_start_fpga_session(&ctx) != XMA_APP_SUCCESS) {
        xlnx_enc_app_close(&ctx);
        return XMA_APP_ERROR;
    }

    xlnx_enc_encode_file(&ctx);
    xlnx_enc_print_total_fps(&ctx);
    xlnx_enc_app_close(&ctx);
    return 0;
}
