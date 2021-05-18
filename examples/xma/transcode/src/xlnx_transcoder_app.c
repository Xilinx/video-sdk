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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <termios.h>

#include "xlnx_transcoder.h"
#include "xlnx_transcoder_parser.h"
#include "xlnx_transcoder_xrm_interface.h"

int32_t transcode_stop = 0;

/*-----------------------------------------------------------------------------
xlnx_tran_signal_handler: Signal handler function

Parameters:
signum: Signal number
-----------------------------------------------------------------------------*/
void xlnx_tran_signal_handler (int32_t signum)
{
    switch (signum)
    {
        case SIGTERM:
        case SIGINT:
        case SIGABRT:
        case SIGHUP:
        case SIGQUIT:
            transcode_stop = TRANSCODE_APP_STOP;
            break;
    }
}


/*-----------------------------------------------------------------------------
xlnx_tran_set_signal_handler: Signal handler initialization.

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_tran_set_signal_handler()
{

    struct sigaction action;
    action.sa_handler = xlnx_tran_signal_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGHUP, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGQUIT, &action, NULL);
    return TRANSCODE_APP_SUCCESS;
}


/*------------------------------------------------------------------------------
xlnx_tran_print_total_fps: Print the total performance of the transcoder.

Parameters:
transcode_ctx: Transcoder context

Return:
None
------------------------------------------------------------------------------*/
static void xlnx_tran_print_total_fps(XlnxTranscoderCtx *transcode_ctx)
{
    double time_taken = xlnx_utils_get_total_time(&transcode_ctx->app_timer);

    fprintf(stderr, "\nFrames Transcoded: %zu, Time Elapsed: %.03lf\r\n", 
            transcode_ctx->out_frame_cnt, time_taken);
    fprintf(stderr, "Total FPS: %.03lf\r\n",
            transcode_ctx->out_frame_cnt / time_taken);
}

/*-----------------------------------------------------------------------------
xlnx_tran_print_segment_fps: Calculate and print fps for every second

Parameters:
transcode_ctx: Transcoder context

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_tran_print_segment_fps(XlnxTranscoderCtx *transcode_ctx)
{

    double segment_time = xlnx_utils_get_segment_time(&transcode_ctx->app_timer);
    if(segment_time < 1) {
        return;
    }

    fprintf(stderr, "\rFrame=%5zu Total FPS=%.03f Current FPS=%.03f\r", 
            transcode_ctx->out_frame_cnt, (float) transcode_ctx->out_frame_cnt / 
            xlnx_utils_get_total_time(&transcode_ctx->app_timer), 
            (transcode_ctx->out_frame_cnt -
            transcode_ctx->app_timer.last_displayed_frame) / segment_time);
    fflush(stderr);
    transcode_ctx->app_timer.last_displayed_frame = transcode_ctx->out_frame_cnt;
    xlnx_utils_set_segment_time(&transcode_ctx->app_timer);

    return;
}

/*-----------------------------------------------------------------------------
xlnx_tran_app_close: Deinitialize transcoder application

Parameters:
transcode_ctx: Transcoder context
-----------------------------------------------------------------------------*/
static void xlnx_tran_app_close(XlnxTranscoderCtx *transcode_ctx,
                                XlnxTranscoderProperties *transcode_props)
{

    /* Deinitialize all the modules and release the resources */
    xlnx_dec_deinit(transcode_ctx->app_xrm_ctx.xrm_ctx, 
                    &transcode_ctx->dec_ctx, &transcode_props->xma_dec_props);

    if(transcode_ctx->num_scal_out) {
        xlnx_scal_deinit(transcode_ctx->app_xrm_ctx.xrm_ctx, 
                &transcode_ctx->scal_ctx, &transcode_props->xma_scal_props);
    }

    for(int32_t i = 0; i < transcode_ctx->num_enc_channels; i++) {
        xlnx_enc_deinit(transcode_ctx->app_xrm_ctx.xrm_ctx, 
            &transcode_ctx->enc_ctx[i], &transcode_props->xma_enc_props[i],
            &transcode_props->xma_la_props[i]);
    }

    xlnx_tran_xrm_deinit(&transcode_ctx->app_xrm_ctx);
    return;
}


/*-----------------------------------------------------------------------------
 ******************** Transcoder Application ***********************************
 -----------------------------------------------------------------------------*/
int32_t main(int32_t argc,  char *argv[])
{

    XlnxTranscoderCtx transcode_ctx;
    XlnxTranscoderProperties transcode_props;
    int32_t ret = TRANSCODE_APP_SUCCESS;

    if (argc < 2) {
        fprintf(stderr, "%s \n", xlnx_tran_get_help());
        return -1;
    }

    memset(&transcode_ctx, 0, sizeof(transcode_ctx));

    /* Parsing command line and updating decoder, scaler and encoder context */
    ret = xlnx_tran_parser(argc, argv, &transcode_ctx, &transcode_props);
    if(ret != TRANSCODE_APP_SUCCESS) {
        return TRANSCODE_APP_FAILURE;
    }

    /* Reserving the CU resource and perform XMA initialize */
    ret = xlnx_tran_device_init(&transcode_ctx, &transcode_props);
    if(ret != TRANSCODE_APP_SUCCESS) {
        xlnx_tran_app_close(&transcode_ctx, &transcode_props);
        return TRANSCODE_APP_FAILURE;
    }

    /* Session creation for decoder, scaler, LA and encoder */
    ret = xlnx_tran_session_create(&transcode_ctx, &transcode_props);
    if(ret != TRANSCODE_APP_SUCCESS) {
        xlnx_tran_app_close(&transcode_ctx, &transcode_props);
        return TRANSCODE_APP_FAILURE;
    }

    /* Setting signal handler for transcoder */
    xlnx_tran_set_signal_handler();

    xlnx_utils_set_non_blocking(1);

    xlnx_utils_start_tracking_time(&transcode_ctx.app_timer);

    do {

        /* Checking for user key Q to quit the application*/
        if((ret = xlnx_utils_was_q_pressed()) == TRANSCODE_APP_STOP) {
            transcode_stop = TRANSCODE_APP_STOP;
        }

        ret = xlnx_tran_frame_process(&transcode_ctx, &transcode_stop);

        /* FPS calculation and printing for every 1 second */
        xlnx_tran_print_segment_fps(&transcode_ctx);

    }while(ret == TRANSCODE_APP_SUCCESS);

    /* FPS calculation for the complete stream */
    xlnx_tran_print_total_fps(&transcode_ctx);

    xlnx_utils_set_non_blocking(0);

    xlnx_tran_app_close(&transcode_ctx, &transcode_props);

    return 0;
}
