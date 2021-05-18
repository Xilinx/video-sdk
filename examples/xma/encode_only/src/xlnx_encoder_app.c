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

#include "xlnx_lookahead.h"
#include "xlnx_encoder.h"
#include "xlnx_app_utils.h"
#include "xlnx_enc_xrm_interface.h"
#include "xlnx_enc_constants.h"

int32_t enc_stop;

/*-----------------------------------------------------------------------------
xlnx_enc_signal_handler: Signal handler function

Parameters:
signum: Signal number

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
void xlnx_enc_signal_handler (int32_t signum)
{
    switch (signum)
    {
        case SIGTERM:
        case SIGINT:
        case SIGABRT:
        case SIGHUP:
        case SIGQUIT:
            enc_stop = ENC_APP_STOP;
            break;
    }
}

/*-----------------------------------------------------------------------------
xlnx_enc_set_signal_handler: Signal handler initialization.

Parameters:
None

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_set_signal_handler()
{

    struct sigaction action;
    action.sa_handler = xlnx_enc_signal_handler;
    sigemptyset (&action.sa_mask);
    action.sa_flags = 0;
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGHUP, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGQUIT, &action, NULL);
    return ENC_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_app_close: Deinitialize encoder application

Parameters:
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties
xma_la_props: XMA lookahead properties

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_enc_app_close(XlnxEncoderCtx *enc_ctx, 
                          XmaEncoderProperties *xma_enc_props,
                          XmaFilterProperties  *xma_la_props)
{
    xlnx_enc_deinit(enc_ctx, xma_enc_props);
    xlnx_la_deinit(&enc_ctx->la_ctx, xma_la_props);
    xlnx_enc_xrm_deinit(&enc_ctx->enc_xrm_ctx);

    return;
}

/*------------------------------------------------------------------------------
xlnx_enc_print_total_fps: Print the total performance of the encoder.

Parameters:
enc_ctx: Encoder context

Return:
None
------------------------------------------------------------------------------*/
void xlnx_enc_print_total_fps(XlnxEncoderCtx *enc_ctx)
{
    double time_taken = xlnx_utils_get_total_time(&enc_ctx->enc_timer);

    fprintf(stderr, "\nFrames Encoded: %zu, Time Elapsed: %.03lf\r\n", 
            enc_ctx->out_frame_cnt, time_taken);
    fprintf(stderr, "Total FPS: %.03lf\r\n",
            enc_ctx->out_frame_cnt / time_taken);
}

/*-----------------------------------------------------------------------------
xlnx_enc_print_segment_fps: Calculate and print fps for every second

Parameters:
enc_ctx: Encoder context

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_enc_print_segment_fps(XlnxEncoderCtx *enc_ctx)
{

    double segment_time = xlnx_utils_get_segment_time(&enc_ctx->enc_timer);
    if(segment_time < 1) {
        return;
    }

    fprintf(stderr, "\rFrame=%5zu Total FPS=%.03f Current FPS=%.03f\r", 
      enc_ctx->out_frame_cnt, (float) enc_ctx->out_frame_cnt / 
      xlnx_utils_get_total_time(&enc_ctx->enc_timer), (enc_ctx->out_frame_cnt -
       enc_ctx->enc_timer.last_displayed_frame)/segment_time);
    fflush(stderr);

    enc_ctx->enc_timer.last_displayed_frame = enc_ctx->out_frame_cnt;
    xlnx_utils_set_segment_time(&enc_ctx->enc_timer);

    return;
}

/*-----------------------------------------------------------------------------
main: Main function for xma encoder app

Parameters:
argc: Number of arguments
argv: Pointer to the arguments

Return:
ENC_APP_SUCCESS or ENC_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t main(int32_t argc,  char *argv[])
{

    int32_t ret = ENC_APP_SUCCESS;

    /* Encoder context creation */
    XlnxEncoderCtx enc_ctx;
    XmaEncoderProperties xma_enc_props;
    XmaFilterProperties  xma_la_props;

    if (argc < 2) {
        printf( "%s\n", xlnx_enc_get_help());
        return ENC_APP_FAILURE;
    }

    memset(&enc_ctx, 0, sizeof(enc_ctx));

    /* Parsing the encoder arguments and update context and XMA properties */
    if((ret = xlnx_enc_parser(argc, argv, &enc_ctx,  &xma_enc_props)) != 
        ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                   "Encoder parsing failed \n");
        return -1;
    }

    /* Reserving the CU resource and perform XMA initialize */
    if((ret = xlnx_enc_device_init(&enc_ctx.enc_xrm_ctx, &xma_enc_props, 
              enc_ctx.enc_props.lookahead_depth)) != ENC_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE, 
                "Device Init failed with error %d \n", ret);
        xlnx_enc_app_close(&enc_ctx, &xma_enc_props, &xma_la_props);
        return -1;
    }

    /* Lookahead session creation*/
    if((ret = xlnx_enc_la_init(&enc_ctx, &xma_la_props)) != ENC_APP_SUCCESS) {
        xlnx_enc_app_close(&enc_ctx, &xma_enc_props, &xma_la_props);
        return -1;
    }

    if((ret = xlnx_enc_create_session(&enc_ctx, &xma_enc_props)) != 
                                                            ENC_APP_SUCCESS) {
        xlnx_enc_app_close(&enc_ctx, &xma_enc_props, &xma_la_props);
        return -1;
    }

    /* Setting signal handler for SIGINT, SIGTERM and SIGHUP */
    xlnx_enc_set_signal_handler();

    xlnx_utils_set_non_blocking(1);

    enc_stop = 0;
    xlnx_utils_start_tracking_time(&enc_ctx.enc_timer);

    /* Encoder loop */
    do {

        /* Checking for user key Q to quit the application*/
        if((ret = xlnx_utils_was_q_pressed()) != ENC_APP_SUCCESS) {
            enc_stop = ENC_APP_STOP;
        }

        ret = xlnx_enc_frame_process(&enc_ctx, &enc_stop);

        xlnx_enc_print_segment_fps(&enc_ctx);

    }while(ret == ENC_APP_SUCCESS);

    xma_logmsg(XMA_NOTICE_LOG, XLNX_ENC_APP_MODULE, 
            "Encoding of input stream completed \n");
    xlnx_utils_set_non_blocking (0);

    xlnx_enc_app_close(&enc_ctx, &xma_enc_props, &xma_la_props);

    xlnx_enc_print_total_fps(&enc_ctx);

    return 0;
}
