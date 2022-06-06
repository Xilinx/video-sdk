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

#include "xlnx_transcoder_xrm_interface.h"

/**
 * xlnx_tran_fill_pool_props: Updates transcoder CU pool properties
 *
 * @param transcode_cu_pool_prop: Encoder CU pool properties
 * @param transcode_Load: Transcoder load
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t
    xlnx_tran_fill_pool_props(xrmCuPoolPropertyV2* transcode_cu_pool_prop,
                              XlnxTranscoderLoad*  transcode_Load)
{
    transcode_cu_pool_prop->cuListNum = 1;

    xlnx_update_pool_props("DECODER", transcode_Load->dec_load, 1,
                           transcode_cu_pool_prop);

    xlnx_update_pool_props("SCALER", transcode_Load->scal_load, 0,
                           transcode_cu_pool_prop);

    xlnx_update_pool_props("ENCODER", transcode_Load->enc_load,
                           transcode_Load->enc_num, transcode_cu_pool_prop);

    if(transcode_Load->la_load) {
        xlnx_update_pool_props("LOOKAHEAD", transcode_Load->la_load, 0,
                               transcode_cu_pool_prop);
    }

    return XMA_APP_SUCCESS;
}

/**
 * Calculate the transcoder load and updates the CU pool properties.
 * @param app_xrm_ctx The transcoder xrm context
 * @param transcode_props Struct containing the decoder, scaler, and list of
 * encoder xma properties.
 * @return XMA_APP_SUCCESS on success or XMA_APP_ERROR on error.
 */
static int32_t xlnx_tran_load_calc(XlnxTranscoderXrmCtx*     app_xrm_ctx,
                                   XlnxTranscoderProperties* transcode_props,
                                   xrmCuPoolPropertyV2* transcode_cu_pool_prop)
{
    XlnxTranscoderLoad* transcode_load = &app_xrm_ctx->transcode_load;
    transcode_load->dec_load           = xlnx_xrm_load_calc(
        app_xrm_ctx->xrm_ctx, &transcode_props->xma_dec_props, "DECODER");
    if(transcode_load->dec_load < 0) {
        return XMA_APP_ERROR;
    }
    if(app_xrm_ctx->num_scal_out) {
        transcode_load->scal_load = xlnx_xrm_load_calc(
            app_xrm_ctx->xrm_ctx, &transcode_props->xma_scal_props, "SCALER");

        if(app_xrm_ctx->num_fullrate_out > 0) {
            transcode_props->xma_scal_props.num_outputs =
                app_xrm_ctx->num_fullrate_out;
            transcode_load->scal_load +=
                xlnx_xrm_load_calc(app_xrm_ctx->xrm_ctx,
                                   &transcode_props->xma_scal_props, "SCALER");
        }
        if(transcode_load->scal_load < 0) {
            return XMA_APP_ERROR;
        }
    }

    transcode_load->enc_num = app_xrm_ctx->num_enc_channels;
    for(int32_t i = 0; i < app_xrm_ctx->num_enc_channels; i++) {
        XmaEncoderProperties* xma_enc_props =
            &transcode_props->xma_enc_props[i];
        transcode_load->enc_load +=
            xlnx_xrm_load_calc(app_xrm_ctx->xrm_ctx, xma_enc_props, "ENCODER");
        if(transcode_load->enc_load < 0) {
            return XMA_APP_ERROR;
        }

        /* If LA is enabled, calculate the load to reserve the CU*/
        if(app_xrm_ctx->la_enable) {
            XmaFilterProperties filter_props;

            /* Update the lookahead props that are needed for
               libxmaPropsTOjson */
            filter_props.input.width  = xma_enc_props->width;
            filter_props.input.height = xma_enc_props->height;
            filter_props.input.framerate.numerator =
                xma_enc_props->framerate.numerator;
            filter_props.input.framerate.denominator =
                xma_enc_props->framerate.denominator;

            transcode_load->la_load += xlnx_xrm_load_calc(
                app_xrm_ctx->xrm_ctx, &filter_props, "LOOKAHEAD");
        }
    }
    xlnx_tran_fill_pool_props(transcode_cu_pool_prop, transcode_load);
    return XMA_APP_SUCCESS;
}

/**
 * If the device id specified has not been seen, then add it to the unique list
 * and increment the index.
 * @param dev_index The device id to be checked/added
 * @param seen_list List to track what we have seen. False if unseen.
 * @param unique_dev_list List of unique devices we have encountered
 * @param unique_dev_index The current index of the list (Number of unique ids
 * we have gotten)
 * @return XMA_APP_SUCCESS on succes, XMA_APP_ERROR when dev index is out of
 * range.
 */
static int add_dev_if_unseen(int dev_index, bool* seen_list,
                             int* unique_dev_list, int* unique_dev_index)
{
    if(dev_index > XLNX_MAX_DEVICE_COUNT || dev_index < DEFAULT_DEVICE_ID) {
        return XMA_APP_ERROR;
    }
    if(seen_list[dev_index] == false) {
        unique_dev_list[*unique_dev_index] = dev_index;
        (*unique_dev_index)++;
        seen_list[dev_index] = true;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Get the list of unique devices specified in the xma properties
 * @param trnscode_props Struct containing the xma decode, scale, encode props
 * which each have device indeces
 * @param num_channels Number of encoder channels/size of the xma_enc_props list
 * @param unique_devs The unique devices encountered from going through the dev
 * indeces in the xma props
 * @param num_unique_devs The number of unique devices encountered
 */
static int get_unique_devices(XlnxTranscoderProperties* transcode_props,
                              int num_channels, int* unique_devs,
                              int* num_unique_devs)
{
    bool seen_list[XLNX_MAX_DEVICE_COUNT];
    memset(seen_list, false, XLNX_MAX_DEVICE_COUNT * sizeof(bool));
    int curr_dev_index = transcode_props->xma_dec_props.dev_index;
    if(add_dev_if_unseen(curr_dev_index, &seen_list[0], unique_devs,
                         num_unique_devs) != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Decoder device index %d is out of range!\n",
                   curr_dev_index);
        return XMA_APP_ERROR;
    }
    curr_dev_index = transcode_props->xma_scal_props.dev_index;
    if(transcode_props->xma_scal_props.num_outputs > 0) {
        if(add_dev_if_unseen(curr_dev_index, &seen_list[0], unique_devs,
                             num_unique_devs) != XMA_APP_SUCCESS) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Scaler device index %d is out of range!\n",
                       curr_dev_index);
            return XMA_APP_ERROR;
        }
    }
    for(int i = 0; i < num_channels; i++) {
        curr_dev_index = transcode_props->xma_enc_props[i].dev_index;
        if(add_dev_if_unseen(curr_dev_index, &seen_list[0], unique_devs,
                             num_unique_devs)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Encoder device index %d is out of range!\n",
                       curr_dev_index);
            return XMA_ERROR;
        }
    }
    if(*num_unique_devs > XLNX_MAX_DEVS_PER_CMD) {
        xma_logmsg(
            XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
            "%d devices given, but only %d devices supported per command!\n",
            *num_unique_devs, XLNX_MAX_DEVS_PER_CMD);
        return XMA_ERROR;
    }
    return XMA_APP_SUCCESS;
}

/**
 * Allocate xrm resources and initialize the devices
 * @param app_xrm_ctx The transcoder xrm context
 * @param transcode_props Struct containing the decoder, scaler, and list of
 * encoder xma properties.
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
int32_t xlnx_tran_xma_init(XlnxTranscoderXrmCtx*     app_xrm_ctx,
                           XlnxTranscoderProperties* transcode_props)
{
    xrmCuPoolPropertyV2 transcode_cu_pool_prop;
    int32_t             ret = XMA_APP_ERROR;

    memset(&transcode_cu_pool_prop, 0, sizeof(transcode_cu_pool_prop));

    /* Create XRM context */
    app_xrm_ctx->xrm_ctx = xlnx_xrm_create_context();
    if(app_xrm_ctx->xrm_ctx == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "create local XRM context failed\n");
        return ret;
    }

    /* Calculate encoder load based on encoder properties */
    ret = xlnx_tran_load_calc(app_xrm_ctx, transcode_props,
                              &transcode_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Transcoder load calculation failed %d\n", ret);
        xrmDestroyContext(app_xrm_ctx->xrm_ctx);
        return ret;
    }
    int unique_devs[1 + (transcode_props->xma_scal_props.num_outputs > 0) +
                    app_xrm_ctx->num_enc_channels];
    int num_unique_devs = 0;
    if(get_unique_devices(transcode_props, app_xrm_ctx->num_enc_channels,
                          &unique_devs[0],
                          &num_unique_devs) != XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }
    ret = xlnx_xrm_device_init(app_xrm_ctx->xrm_ctx, &unique_devs[0],
                               num_unique_devs, &transcode_cu_pool_prop);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Transcoder device initialization failed %d\n", ret);
        return ret;
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_dec_cu_alloc: Allocates decoder CU
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_dec_props: Decoder XMA properties
 * @param decode_cu_list_res: Decoder CU list resource
 * @return XMA_APP_SUCCESS on success, otherwise XMA_APP_ERROR
 */
int32_t xlnx_dec_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XmaDecoderProperties* xma_dec_props,
                          xrmCuListResourceV2*  decode_cu_list_res)
{

    int32_t ret = XMA_APP_ERROR;

    ret = xlnx_xrm_cu_alloc(
        app_xrm_ctx->xrm_ctx, "DECODER", app_xrm_ctx->transcode_load.dec_load,
        xma_dec_props->dev_index, app_xrm_ctx->reserve_idx, decode_cu_list_res);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xrm_allocation: failed to allocate decoder cu\n");
        return XMA_APP_ERROR;
    }

    app_xrm_ctx->dec_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_dec_props->plugin_lib =
        decode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_dec_props->dev_index = decode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_dec_props->ddr_bank_index = -1;
    xma_dec_props->cu_index       = decode_cu_list_res->cuResources[1].cuId;
    xma_dec_props->channel_id = decode_cu_list_res->cuResources[1].channelId;

    return ret;
}

/**
 * xlnx_scal_cu_alloc: Allocates CU for scaler
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_scal_props: Scaler properties
 * @param scal_cu_res: Scaler CU resource
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_scal_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                           XmaScalerProperties*  xma_scal_props,
                           xrmCuListResourceV2*  scal_cu_list_res)
{

    int scal_load = 0;
    int ret       = XMA_APP_ERROR;

    scal_load =
        xlnx_xrm_load_calc(app_xrm_ctx->xrm_ctx, xma_scal_props, "SCALER");
    if(scal_load <= 0) {
        return XMA_APP_ERROR;
    }

    ret = xlnx_xrm_cu_alloc(app_xrm_ctx->xrm_ctx, "SCALER", scal_load,
                            xma_scal_props->dev_index, app_xrm_ctx->reserve_idx,
                            scal_cu_list_res);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xrm_allocation: failed to allocate scaler cu\n");
        return XMA_APP_ERROR;
    }

    app_xrm_ctx->scal_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_scal_props->plugin_lib =
        scal_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_scal_props->dev_index      = scal_cu_list_res->cuResources[0].deviceId;
    xma_scal_props->ddr_bank_index = -1;
    xma_scal_props->cu_index       = scal_cu_list_res->cuResources[0].cuId;
    xma_scal_props->channel_id     = scal_cu_list_res->cuResources[0].channelId;

    /* XMA to select the ddr bank based on xclbin meta data */
    xma_scal_props->ddr_bank_index = -1;

    return ret;
}

/**
 * xlnx_la_allocate_xrm_cu: Allocates XRM CU for lookahead
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_la_props: XMA lookahead properties
 * @param lookahead_cu_res: Lookahead CU resource
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_la_allocate_xrm_cu(XlnxTranscoderXrmCtx* app_xrm_ctx,
                                XmaFilterProperties*  xma_la_props,
                                xrmCuListResourceV2*  lookahead_cu_list_res)
{

    /* XRM lookahead allocation */
    int     la_load = 0;
    int32_t ret     = XMA_APP_ERROR;

    la_load =
        xlnx_xrm_load_calc(app_xrm_ctx->xrm_ctx, xma_la_props, "LOOKAHEAD");
    if(la_load <= 0) {
        return XMA_APP_ERROR;
    }

    ret = xlnx_xrm_cu_alloc(app_xrm_ctx->xrm_ctx, "LOOKAHEAD", la_load,
                            xma_la_props->dev_index, app_xrm_ctx->reserve_idx,
                            lookahead_cu_list_res);

    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xrm_allocation: failed to allocate lookahead cu\n");
        return XMA_APP_ERROR;
    }

    app_xrm_ctx->lookahead_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_la_props->plugin_lib =
        lookahead_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_la_props->dev_index = lookahead_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_la_props->ddr_bank_index = -1;
    xma_la_props->cu_index       = lookahead_cu_list_res->cuResources[0].cuId;
    xma_la_props->channel_id = lookahead_cu_list_res->cuResources[0].channelId;

    return ret;
}

/**
 * xlnx_enc_cu_alloc: Allocates CU for encoder
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_enc_props: Encoder XMA properties
 * @param encode_cu_list_res: Encoder CU list resource
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_cu_alloc(XlnxTranscoderXrmCtx* app_xrm_ctx,
                          XmaEncoderProperties* xma_enc_props,
                          xrmCuListResourceV2*  encode_cu_list_res)
{

    int     enc_load = 0;
    int32_t ret      = XMA_APP_ERROR;

    /* XRM encoder plugin load calculation */
    enc_load =
        xlnx_xrm_load_calc(app_xrm_ctx->xrm_ctx, xma_enc_props, "ENCODER");
    if(enc_load <= 0) {
        return XMA_APP_ERROR;
    }

    ret = xlnx_xrm_cu_alloc(app_xrm_ctx->xrm_ctx, "ENCODER", enc_load,
                            xma_enc_props->dev_index, app_xrm_ctx->reserve_idx,
                            encode_cu_list_res);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "xrm_allocation: failed to allocate encoder cu\n");
        return XMA_APP_ERROR;
    }

    app_xrm_ctx->enc_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib =
        encode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = encode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index       = encode_cu_list_res->cuResources[1].cuId;
    xma_enc_props->channel_id = encode_cu_list_res->cuResources[1].channelId;

    return ret;
}

/**
 * xlnx_tran_xrm_deinit: Deinitializes XRM resources
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @return None
 */
void xlnx_tran_xrm_deinit(XlnxTranscoderXrmCtx* app_xrm_ctx)
{

    xlnx_xrm_deinit(app_xrm_ctx->xrm_ctx, app_xrm_ctx->reserve_idx);

    return;
}
