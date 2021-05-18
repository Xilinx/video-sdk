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

#include "xvbm.h"
#include "xlnx_encoder.h"

static struct option enc_options[] =
{
    {FLAG_ENC_CODEC_TYPE,          required_argument, 0, ENC_CODEC_ID_ARG},
    {FLAG_ENC_BITRATE,             required_argument, 0, ENC_BITRATE_ARG},
    {FLAG_ENC_BIT_RATE,            required_argument, 0, ENC_BITRATE_ARG},
    {FLAG_ENC_FPS,                 required_argument, 0, ENC_FPS_ARG},
    {FLAG_ENC_INTRA_PERIOD,        required_argument, 0, ENC_INTRA_PERIOD_ARG},
    {FLAG_ENC_CONTROL_RATE,        required_argument, 0, ENC_CONTROL_RATE_ARG},
    {FLAG_ENC_MAX_BITRATE,         required_argument, 0, ENC_MAX_BITRATE_ARG},
    {FLAG_ENC_SLICE_QP,            required_argument, 0, ENC_SLICE_QP_ARG},
    {FLAG_ENC_MIN_QP,              required_argument, 0, ENC_MIN_QP_ARG},
    {FLAG_ENC_MAX_QP,              required_argument, 0, ENC_MAX_QP_ARG},
    {FLAG_ENC_NUM_BFRAMES,         required_argument, 0, ENC_NUM_BFRAMES_ARG},
    {FLAG_ENC_IDR_PERIOD,          required_argument, 0, ENC_IDR_PERIOD_ARG},
    {FLAG_ENC_PROFILE,             required_argument, 0, ENC_PROFILE_ARG},
    {FLAG_ENC_LEVEL,               required_argument, 0, ENC_LEVEL_ARG},
    {FLAG_ENC_NUM_SLICES,          required_argument, 0, ENC_NUM_SLICES_ARG},
    {FLAG_ENC_QP_MODE,             required_argument, 0, ENC_QP_MODE_ARG},
    {FLAG_ENC_ASPECT_RATIO,        required_argument, 0, ENC_ASPECT_RATIO_ARG},
    {FLAG_ENC_SCALING_LIST,        required_argument, 0, ENC_SCALING_LIST_ARG},
    {FLAG_ENC_LOOKAHEAD_DEPTH,     required_argument, 0, ENC_LOOKAHEAD_DEPTH_ARG},
    {FLAG_ENC_TEMPORAL_AQ,         required_argument, 0, ENC_TEMPORAL_AQ_ARG},
    {FLAG_ENC_SPATIAL_AQ,          required_argument, 0, ENC_SPATIAL_AQ_ARG},
    {FLAG_ENC_SPATIAL_AQ_GAIN,     required_argument, 0, ENC_SPATIAL_AQ_GAIN_ARG},
    {FLAG_ENC_QP,                  required_argument, 0, ENC_QP_ARG},
    {FLAG_ENC_NUM_CORES,           required_argument, 0, ENC_NUM_CORES_ARG},
    {FLAG_ENC_TUNE_METRICS,        required_argument, 0, ENC_TUNE_METRICS_ARG},
    {FLAG_ENC_LATENCY_LOGGING,     required_argument, 0, ENC_LATENCY_LOGGING_ARG},
    {FLAG_ENC_OUTPUT_FILE,         required_argument, 0, ENC_OUTPUT_FILE_ARG},
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
        return TRANSCODE_APP_FAILURE;
    for (cnt = 0; cnt < enc_no_keys; cnt++)
    {
        XlnxEncProfileLookup enc_profile_key = enc_prof_lookuptable[cnt];
        if(strcmp(enc_profile_key.key, profile_key) == 0)
            return enc_profile_key.value;
    }

    return TRANSCODE_APP_FAILURE;

}

/*-----------------------------------------------------------------------------
xlnx_enc_validate_arguments: Validates encoder arguments

Parameters:
enc_ctx: Encoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_validate_arguments(XlnxEncoderProperties *enc_props)
{

    if(enc_props->bit_rate <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid bit-rate param %d \n", enc_props->bit_rate);
        return TRANSCODE_APP_FAILURE;
    }

    if(enc_props->fps <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid frame rate %d \n", enc_props->fps);
        return TRANSCODE_APP_FAILURE;
    }

    if(enc_props->gop_size < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid intra period %d \n", enc_props->gop_size);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->control_rate < 0) || (enc_props->control_rate > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid rate control mode %d \n", enc_props->control_rate);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->max_bitrate <= 0) || (enc_props->max_bitrate > 
                ENC_SUPPORTED_MAX_BITRATE)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid max btirate %ld \n", enc_props->max_bitrate);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->slice_qp < -1) || 
            (enc_props->slice_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid slice qp %d \n", enc_props->slice_qp);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->min_qp < ENC_SUPPORTED_MIN_QP) || 
            (enc_props->min_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid min qp %d \n", enc_props->min_qp);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->max_qp < ENC_SUPPORTED_MIN_QP) || 
            (enc_props->max_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid max qp %d \n", enc_props->max_qp);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->qp_mode < 0) || (enc_props->qp_mode > 2)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid qp mode %d \n", enc_props->qp_mode);
        return TRANSCODE_APP_FAILURE;
    }

    if(enc_props->idr_period < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Invalid IDR period %d \n", enc_props->idr_period);
        return TRANSCODE_APP_FAILURE;
    }

    if(enc_props->gop_size < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Invalid GOP size %d \n", enc_props->gop_size);
        return TRANSCODE_APP_FAILURE;
    }

    if(enc_props->num_bframes < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Invalid number of B frames %d \n", enc_props->num_bframes);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->aspect_ratio < 0) || (enc_props->aspect_ratio > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid aspect ratio %d \n", enc_props->aspect_ratio);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->scaling_list != 0) && (enc_props->scaling_list != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid scaling list value %d \n", enc_props->scaling_list);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH) || 
            (enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid LA depth %d \n", enc_props->lookahead_depth);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->temporal_aq != 0) && (enc_props->temporal_aq != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid temporal aq value %d \n", enc_props->temporal_aq);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->spatial_aq != 0) && (enc_props->spatial_aq != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid spatial aq value %d \n", enc_props->spatial_aq);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->spatial_aq_gain < 0) || 
            (enc_props->spatial_aq_gain > ENC_MAX_SPAT_AQ_GAIN)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Invalid spatial aq gain %d \n", enc_props->spatial_aq_gain);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->num_cores < 0) && (enc_props->num_cores > 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Invalid number of cores %d \n", enc_props->num_cores);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->tune_metrics != 0) && (enc_props->tune_metrics != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "Invalid value for Tune metrics %d \n", enc_props->tune_metrics);
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc_device_id: Allocates CU for encoder based on device 
                                  index

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_enc_props: Encoder XMA properties
encode_cu_list_res: Encoder CU list resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_cu_alloc_device_id(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                    XmaEncoderProperties *xma_enc_props,
                    xrmCuListResource *encode_cu_list_res, int32_t enc_load)
{

    xrmCuProperty encode_cu_hw_prop, encode_cu_sw_prop;

    int32_t ret = -1;

    memset(&encode_cu_hw_prop, 0, sizeof(xrmCuProperty));
    memset(&encode_cu_sw_prop, 0, sizeof(xrmCuProperty));
    memset(encode_cu_list_res, 0, sizeof(xrmCuListResource));

    strcpy(encode_cu_hw_prop.kernelName, "encoder");
    strcpy(encode_cu_hw_prop.kernelAlias, "ENCODER_MPSOC");
    encode_cu_hw_prop.devExcl = false;
    encode_cu_hw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_load);

    strcpy(encode_cu_sw_prop.kernelName, "kernel_vcu_encoder");
    encode_cu_sw_prop.devExcl = false;
    encode_cu_sw_prop.requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);

    ret = xrmCuAllocFromDev(app_xrm_ctx->xrm_ctx, app_xrm_ctx->device_id, 
        &encode_cu_hw_prop, &encode_cu_list_res->cuResources[0]);

    if (ret <= TRANSCODE_APP_FAILURE)
    {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
            "xrm failed to allocate encoder resources on device %d\n", 
            app_xrm_ctx->device_id );
        return ret;
    }
    else
    {
        ret = xrmCuAllocFromDev(app_xrm_ctx->xrm_ctx, app_xrm_ctx->device_id, 
          &encode_cu_sw_prop, &encode_cu_list_res->cuResources[1]);

        if (ret <= TRANSCODE_APP_FAILURE)
        {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "xrm failed to allocate encoder resources on device %d\n", 
                app_xrm_ctx->device_id );
            return ret;
        }
    }
    app_xrm_ctx->enc_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
                    encode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = encode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = encode_cu_list_res->cuResources[1].cuId;
    xma_enc_props->channel_id = encode_cu_list_res->cuResources[1].channelId;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc_reserve_id: Allocates CU for encoder based on 
                                   reservation index

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_enc_props: Encoder XMA properties
encode_cu_list_res: Encoder CU list resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_cu_alloc_reserve_id(XlnxTranscoderXrmCtx *app_xrm_ctx,
                    XmaEncoderProperties *xma_enc_props,
                    xrmCuListResource *encode_cu_list_res, int32_t enc_load)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    /* XRM encoder allocation */
    xrmCuListProperty encode_cu_list_prop;

    memset(&encode_cu_list_prop, 0, sizeof(xrmCuListProperty));
    memset(encode_cu_list_res, 0, sizeof(xrmCuListResource));

    encode_cu_list_prop.cuNum = 2;
    strcpy(encode_cu_list_prop.cuProps[0].kernelName, "encoder");
    strcpy(encode_cu_list_prop.cuProps[0].kernelAlias, "ENCODER_MPSOC");
    encode_cu_list_prop.cuProps[0].devExcl = false;
    encode_cu_list_prop.cuProps[0].requestLoad = XRM_PRECISION_1000000_BIT_MASK(enc_load);
    encode_cu_list_prop.cuProps[0].poolId = app_xrm_ctx->reserve_idx;

    strcpy(encode_cu_list_prop.cuProps[1].kernelName, "kernel_vcu_encoder");
    encode_cu_list_prop.cuProps[1].devExcl = false;
    encode_cu_list_prop.cuProps[1].requestLoad = XRM_PRECISION_1000000_BIT_MASK(XRM_MAX_CU_LOAD_GRANULARITY_1000000);
    encode_cu_list_prop.cuProps[1].poolId = app_xrm_ctx->reserve_idx;

    ret = xrmCuListAlloc(app_xrm_ctx->xrm_ctx, &encode_cu_list_prop, 
                         encode_cu_list_res);
    if (ret != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Failed to allocate encoder cu from reserve id \n");
        return ret;
    }
    app_xrm_ctx->enc_res_in_use = 1;

    /* Set XMA plugin SO and device index */
    xma_enc_props->plugin_lib = 
        encode_cu_list_res->cuResources[0].kernelPluginFileName;
    xma_enc_props->dev_index = encode_cu_list_res->cuResources[0].deviceId;
    /* XMA to select the ddr bank based on xclbin meta data */
    xma_enc_props->ddr_bank_index = -1;
    xma_enc_props->cu_index = encode_cu_list_res->cuResources[1].cuId;
    xma_enc_props->channel_id = encode_cu_list_res->cuResources[1].channelId;

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_cu_alloc: Allocates CU for encoder

Parameters:
app_xrm_ctx: Transcoder XRM context
xma_enc_props: Encoder XMA properties
encode_cu_list_res: Encoder CU list resource

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_cu_alloc(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                                 XmaEncoderProperties *xma_enc_props,
                                 xrmCuListResource *encode_cu_list_res)
{

    char pluginName[XRM_MAX_NAME_LEN];
    int func_id = 0, enc_load=0;
    int32_t ret = TRANSCODE_APP_FAILURE;
    xrmPluginFuncParam param;

    /* XRM encoder plugin load calculation */
    memset(&param, 0, sizeof(xrmPluginFuncParam));
    void *handle;
    void (*convertXmaPropsToJson)(void* props, char* funcName, char* jsonJob);

    handle = dlopen("/opt/xilinx/xrm/plugin/libxmaPropsTOjson.so", RTLD_NOW );

    convertXmaPropsToJson = dlsym(handle, "convertXmaPropsToJson");
    (*convertXmaPropsToJson) (xma_enc_props, "ENCODER",param.input);
    dlclose(handle);

    strcpy(pluginName, "xrmU30EncPlugin");

    if (xrmExecPluginFunc(app_xrm_ctx->xrm_ctx, pluginName, func_id, &param) != 
        XRM_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Encoder XRM plugin function failed \n");
    }
    else {
        enc_load = atoi((char*)(strtok(param.output, " ")));
    }

    if(app_xrm_ctx->device_id >= 0) {
        ret = xlnx_enc_cu_alloc_device_id(app_xrm_ctx, xma_enc_props, 
                                          encode_cu_list_res, enc_load);
    }
    else {
        ret = xlnx_enc_cu_alloc_reserve_id(app_xrm_ctx, xma_enc_props, 
                                           encode_cu_list_res, enc_load);
    }

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_la_init: Lookahead module initialization

Parameters:
enc_ctx: Encoder context
app_xrm_ctx: Transcoder XRM context
xma_la_props: Lookahead XMA properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_la_init(XlnxEncoderCtx *enc_ctx, 
                           XlnxTranscoderXrmCtx *app_xrm_ctx,
                           XmaFilterProperties  *xma_la_props)
{
    int32_t ret = TRANSCODE_APP_FAILURE;
    XlnxLookaheadProperties *la_props = &enc_ctx->la_ctx.la_props;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    la_props->width = enc_props->width;
    la_props->height = enc_props->height;
    la_props->framerate.numerator = enc_props->fps;
    la_props->framerate.denominator = 1;

    //TODO: Assume 256 aligned for now. Needs to be fixed later
    la_props->stride = ALIGN(enc_props->width, TRANSCODE_WIDTH_ALIGN);
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
    la_props->enable_hw_buf = 1;

    switch (enc_props->codec_id) {
        case ENCODER_ID_H264:
            la_props->codec_type = LOOKAHEAD_ID_H264;
            break;
        case ENCODER_ID_HEVC:
            la_props->codec_type = LOOKAHEAD_ID_HEVC;
            break;
    }

    ret = xlnx_la_create(&enc_ctx->la_ctx, app_xrm_ctx, xma_la_props);
    if (ret != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error : init_la : create_xlnx_la Failed \n");
        return TRANSCODE_APP_FAILURE;
    }

    enc_ctx->la_bypass = xlnx_la_in_bypass_mode(&enc_ctx->la_ctx);

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_context_init: Initializes the encoder context with default values.

Parameters:
enc_ctx: Encoder context
-----------------------------------------------------------------------------*/
void xlnx_enc_context_init(XlnxEncoderCtx *enc_ctx)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;
    /* Initialize the encoder parameters to default */
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
    enc_props->idr_period = ENC_DEFAULT_GOP_SIZE;
    /* Assigning the default profile as HEVC profile. If the codec optin 
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
    enc_props->enable_hw_buf = 1;
    enc_props->latency_logging = 0;
    enc_props->num_cores = 0;
    enc_props->tune_metrics = 0;
    enc_ctx->flush_frame_sent = 0;


}

/*-----------------------------------------------------------------------------
xlnx_enc_update_props: Updates xma encoder properties and options that will be 
sent to xma plugin

Parameters:
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_update_props(XlnxEncoderCtx *enc_ctx, 
        XmaEncoderProperties *xma_enc_props)
{

    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;
    enc_props->enc_options = calloc(1, ENC_MAX_OPTIONS_SIZE);

    /* Enable custom rate control when rate control is set to CBR and 
    lookahead is set, disable when expert option lookahead-rc-off is set. */
    if((enc_props->control_rate == 1) && (enc_props->lookahead_depth > 1)) {
        enc_props->custom_rc = 1;
    }

    if (enc_props->lookahead_depth)
    {
        if((enc_props->width > ENC_MAX_LA_INPUT_WIDTH) || 
            (enc_props->height > ENC_MAX_LA_INPUT_WIDTH) ||
            ((enc_props->width * enc_props->height) > ENC_MAX_LA_PIXELS)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Input to encoder for lookahead usecase %dx%d exceeds maximum supported resolution %dx%d \n", 
                    enc_props->width, enc_props->height, ENC_MAX_LA_INPUT_WIDTH, 
                    ENC_MAX_LA_INPUT_HEIGHT, ENC_MAX_LA_INPUT_HEIGHT, ENC_MAX_LA_INPUT_WIDTH);
            return TRANSCODE_APP_FAILURE;
        }
    }
    else
    {
        if((enc_props->width > ENC_SUPPORTED_MAX_WIDTH) || 
            (enc_props->height > ENC_SUPPORTED_MAX_WIDTH) ||
            ((enc_props->width * enc_props->height) > ENC_SUPPORTED_MAX_PIXELS)) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Input resolution %dx%d exceeds maximum supported resolution %dx%d \n", 
                    enc_props->width, enc_props->height, ENC_SUPPORTED_MAX_WIDTH, ENC_SUPPORTED_MAX_HEIGHT, 
                    ENC_SUPPORTED_MAX_HEIGHT, ENC_SUPPORTED_MAX_WIDTH);
            return TRANSCODE_APP_FAILURE;
        }
    }

    if((enc_props->width < ENC_SUPPORTED_MIN_WIDTH) || (enc_props->width%4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                   "Unsupported width %d \n", enc_props->width);
        return TRANSCODE_APP_FAILURE;
    }

    if((enc_props->height < ENC_SUPPORTED_MIN_HEIGHT) || (enc_props->height%4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                   "Unsupported height %d \n", enc_props->height);
        return TRANSCODE_APP_FAILURE;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if (enc_props->lookahead_depth >= 1 && (enc_props->temporal_aq == 1 || 
                enc_props->spatial_aq == 1) && (enc_props->tune_metrics == 0)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
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
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Setting qp mode to 2, as the lookahead params are set \n");
        enc_props->qp_mode = 2;
    }
    else if (enc_props->lookahead_depth == 0) {
        if (enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if (enc_props->spatial_aq)
            enc_props->spatial_aq = 0;
    }

    /* Set IDR period to gop-size, when the user has not specified it on 
       the command line */
    if (enc_props->idr_period == -1)
    {
        if (enc_props->gop_size > 0){
            enc_props->idr_period = enc_props->gop_size;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                "Setting IDR period to GOP size \n");
    }

    return xlnx_enc_get_xma_props(enc_props, xma_enc_props);
}


/*-----------------------------------------------------------------------------
xlnx_utils_get_br_in_kbps: Get value of bit rate
Parameters:
    desination: Where to store the bitrate
    source: User input value
    param_name: Name of the parameter
Return:
    TRANSCODE_APP_SUCCESS if success
    TRANSCODE_APP_FAILURE if failure
-----------------------------------------------------------------------------*/
static int xlnx_utils_get_br_in_kbps(int64_t* destination, char* source, 
                                     char* param_name)
{
    float br_in_kbps = atof(source);
    if(xlnx_utils_check_if_pattern_matches("^-?[0-9]*\\.?[0-9]+[M|m|K|k]*$", 
       source) == 0) {
       
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, "Unrecognized value "
                   "\"%s\" for argument -%s! Make sure the value is of proper "
                   "type.\n", source, param_name);
        return TRANSCODE_APP_FAILURE;
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
    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_parse_args: Parses the command line arguments

Parameters:
argc: Number of arguments
argv: Pointer to the arguments
enc_ctx: Encoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_parse_args(int32_t argc, char* argv[], 
        XlnxEncoderCtx *enc_ctx, int32_t param_flag)
{
    int32_t flag = 0;;
    int32_t option_index;
    int32_t ret = TRANSCODE_APP_SUCCESS;
    int32_t channel_end = 0;
    XlnxEncoderProperties *enc_props = &enc_ctx->enc_props;

    while(!channel_end)
    {
        if(param_flag == 0) {
            flag = getopt_long_only(argc, argv, "", enc_options, &option_index);
            if(flag == -1) {
                ret = TRANSCODE_PARSING_DONE;
                break;
            }
        }
        else {
            flag = param_flag;
            param_flag = 0;
        }
        switch (flag)
        {
            case ENC_OUTPUT_FILE_ARG:
                enc_ctx->out_file = open(optarg, 
                            O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | 
                            S_IWUSR | S_IRGRP | S_IROTH);
                if(enc_ctx->out_file == TRANSCODE_APP_FAILURE) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                            "Error opening output file %s \n", optarg);
                    return TRANSCODE_APP_FAILURE;
                }
                channel_end = 1;
                break;

            case ENC_CODEC_ID_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264")) {
                    enc_props->codec_id = ENCODER_ID_H264;
                    /* Change the default profile from HEVC_MAIN to H264_HIGH*/
                    enc_props->profile = ENC_H264_HIGH;
                }
                else if(!strcmp(optarg, "mpsoc_vcu_hevc")) {
                    enc_props->codec_id = ENCODER_ID_HEVC;
                }
                else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                            "Unsupported codec %s \n", optarg);
                    return TRANSCODE_APP_FAILURE;
                }
                break;

            case ENC_BITRATE_ARG:
                ret = xlnx_utils_get_br_in_kbps(&enc_props->bit_rate, optarg, FLAG_ENC_BITRATE);
                break;

            case ENC_FPS_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->fps, optarg, FLAG_ENC_FPS);
                break;

            case ENC_INTRA_PERIOD_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->gop_size, optarg, FLAG_ENC_INTRA_PERIOD);
                break;

            case ENC_CONTROL_RATE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->control_rate, optarg, FLAG_ENC_CONTROL_RATE);
                break;

            case ENC_MAX_BITRATE_ARG:
                ret = xlnx_utils_get_br_in_kbps(&enc_props->max_bitrate, optarg, FLAG_ENC_MAX_BITRATE);
                break;

            case ENC_SLICE_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->slice_qp, optarg, FLAG_ENC_SLICE_QP);
                break;

            case ENC_MIN_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->min_qp, optarg, FLAG_ENC_MIN_QP);
                break;

            case ENC_MAX_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->max_qp, optarg, FLAG_ENC_MAX_QP);
                break;

            case ENC_NUM_BFRAMES_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->num_bframes, optarg, FLAG_ENC_NUM_BFRAMES);
                break;

            case ENC_IDR_PERIOD_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->idr_period, optarg, FLAG_ENC_IDR_PERIOD);
                break;

            case ENC_PROFILE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->profile, optarg, "");
                if (ret == TRANSCODE_APP_FAILURE) {
                    if(enc_props->codec_id == ENCODER_ID_H264) {
                        ret = xlnx_enc_key_from_string(enc_prof_h264_lookup, 
                                                       optarg, ENC_PROF_H264_KEYS);
                        if (ret == TRANSCODE_APP_FAILURE)
                            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                                       "Invalid H264 codec profile value %s \n", 
                                       optarg);
                        else
                            enc_props->profile = ret;
                    }
                    else {
                        ret = xlnx_enc_key_from_string(enc_prof_hevc_lookup, 
                                                       optarg, ENC_PROF_HEVC_KEYS);
                        if (ret == TRANSCODE_APP_FAILURE)
                            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                                       "Invalid HEVC codec profile value %s \n", 
                                       optarg);
                        else
                            enc_props->profile = ret;
                    }
                }
                break;

            case ENC_LEVEL_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->level, optarg, FLAG_ENC_LEVEL);
                break;

            case ENC_NUM_SLICES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_slices, optarg, FLAG_ENC_NUM_SLICES);
                break;

            case ENC_QP_MODE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->qp_mode, optarg, FLAG_ENC_QP_MODE);
                break;

            case ENC_ASPECT_RATIO_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->aspect_ratio, optarg, FLAG_ENC_ASPECT_RATIO);
                break;

            case ENC_SCALING_LIST_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->scaling_list, optarg, FLAG_ENC_SCALING_LIST);
                break;

            case ENC_LOOKAHEAD_DEPTH_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->lookahead_depth, optarg, FLAG_ENC_LOOKAHEAD_DEPTH);
                break;

            case ENC_TEMPORAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->temporal_aq, optarg, FLAG_ENC_TEMPORAL_AQ);
                break;

            case ENC_SPATIAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq, optarg, FLAG_ENC_SPATIAL_AQ);
                break;

            case ENC_SPATIAL_AQ_GAIN_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq_gain, optarg, FLAG_ENC_SPATIAL_AQ_GAIN);
                break;

            case ENC_NUM_CORES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_cores, optarg, FLAG_ENC_NUM_CORES);
                break;

            case ENC_TUNE_METRICS_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->tune_metrics, optarg, FLAG_ENC_TUNE_METRICS);
                break;

            case ENC_LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->latency_logging, optarg, FLAG_ENC_LATENCY_LOGGING);
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Failed to parse encoder arguments %d \n", flag);
                return TRANSCODE_APP_FAILURE;
        }

        if(ret == TRANSCODE_APP_FAILURE || ret == RET_ERROR) {
            return TRANSCODE_APP_FAILURE;
        }
    }

    ret |= xlnx_enc_validate_arguments(enc_props);
    return ret;
}

/*-----------------------------------------------------------------------------
enc_session: Creates encoder session

Parameters:
app_xrm_ctx: Transcoder XRM context
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties
xma_la_props: XMA lookahead properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_session(XlnxTranscoderXrmCtx *app_xrm_ctx, 
                         XlnxEncoderCtx *enc_ctx, 
                         XmaEncoderProperties *xma_enc_props,
                         XmaFilterProperties  *xma_la_props)
{

    /* Lookahead session creation*/
    if(xlnx_enc_la_init(enc_ctx, app_xrm_ctx, xma_la_props) != 
                        TRANSCODE_APP_SUCCESS) {
        return TRANSCODE_APP_FAILURE;
    }

    if(xlnx_enc_cu_alloc(app_xrm_ctx, xma_enc_props, 
        &enc_ctx->encode_cu_list_res) != TRANSCODE_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Error in encoder CU allocation \n");
        return TRANSCODE_APP_FAILURE;
    }

    /* Encoder session creation */
    enc_ctx->enc_session = xma_enc_session_create(xma_enc_props);
    if(enc_ctx->enc_session == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                "Encoder init failed \n");
        return TRANSCODE_APP_FAILURE;
    }

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_enc_send_frame: Sends YUV input to the encoder

Parameters:
enc_ctx: Encoder context

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_send_frame(XlnxEncoderCtx *enc_ctx)
{
    int32_t ret = TRANSCODE_APP_FAILURE;
    if((ret = xma_enc_session_send_frame(enc_ctx->enc_session, 
            enc_ctx->enc_in_frame)) <= TRANSCODE_APP_FAILURE) {
        XvbmBufferHandle xvbm_handle = 
            (XvbmBufferHandle)(enc_ctx->enc_in_frame->data[0].buffer);
        if (xvbm_handle) {
            xvbm_buffer_pool_entry_free(xvbm_handle);
        }
    }
    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_recv_frame: Receives encoder output

Parameters:
enc_ctx: Encoder context
_enc_out_size: Encoder output size

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_recv_frame(XlnxEncoderCtx *enc_ctx, 
                                   int32_t *enc_out_size)
{

    int32_t ret = TRANSCODE_APP_FAILURE;
    ret = xma_enc_session_recv_data(enc_ctx->enc_session, 
                                    enc_ctx->xma_buffer, enc_out_size);

    return ret;
}


/*-----------------------------------------------------------------------------
xlnx_enc_process_frame: Encoder process frame

Parameters:
enc_ctx: Encoder context
_enc_null_frame: Encoder null frame flag
_enc_out_size: Encoder output size

Return:
return of xma apis of send_frame or recv_data
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_process_frame(XlnxEncoderCtx *enc_ctx, int32_t enc_null_frame, 
                          int32_t *enc_out_size)
{
    int32_t ret = TRANSCODE_APP_SUCCESS;

    if(enc_null_frame) {
        enc_ctx->enc_in_frame->is_last_frame = 1;
        enc_ctx->enc_in_frame->pts = -1;
    }

    if(!enc_ctx->flush_frame_sent) {
        ret = xlnx_enc_send_frame(enc_ctx);

        if (enc_ctx->enc_in_frame && !enc_null_frame) {
            /* Release the LA output frame(encoder input frame), once 
               encoder consumes it. This will be reused by LA module */
            xlnx_la_release_frame(&enc_ctx->la_ctx, enc_ctx->enc_in_frame);
        }
    }

    if(ret == TRANSCODE_APP_SUCCESS) {
        if((ret = xlnx_enc_recv_frame(enc_ctx, enc_out_size)) <= 
            TRANSCODE_APP_FAILURE) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "xma_enc_session_recv_data failed with error %d \n", ret);
            return TRANSCODE_APP_FAILURE;
        }
    }

    if(enc_null_frame) {
        enc_ctx->flush_frame_sent = 1;
    }

    return ret;
}

/*-----------------------------------------------------------------------------
xlnx_enc_deinit: Deinitialize encoder module

Parameters:
xrm_ctx: XRM context
enc_ctx: Encoder context
xma_enc_props: XMA encoder properties
xma_la_props: XMA lookahead properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_deinit(xrmContext *xrm_ctx, XlnxEncoderCtx *enc_ctx, 
                        XmaEncoderProperties *xma_enc_props,
                        XmaFilterProperties *xma_la_props)
{

    int32_t ret = TRANSCODE_APP_FAILURE;

    ret = xrmCuListRelease(xrm_ctx, &enc_ctx->encode_cu_list_res);

    if(enc_ctx->enc_session != NULL) {
        ret = xma_enc_session_destroy(enc_ctx->enc_session);
    }

    xlnx_enc_free_xma_props(xma_enc_props);

    close(enc_ctx->out_file);
    xlnx_la_close(xrm_ctx, &enc_ctx->la_ctx, xma_la_props);

    return ret;
}
