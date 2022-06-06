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

#include "xlnx_encoder.h"
#include "xvbm.h"

static struct option enc_options[] = {
    {FLAG_ENC_CODEC_TYPE, required_argument, 0, ENC_CODEC_ID_ARG},
    {FLAG_ENC_DEVICE_ID, required_argument, 0, ENC_DEVICE_ID_ARG},
    {FLAG_XVBM_CONVERT, no_argument, 0, ENC_CONVERT_INPUT_ARG},
    {FLAG_ENC_BITRATE, required_argument, 0, ENC_BITRATE_ARG},
    {FLAG_ENC_BIT_RATE, required_argument, 0, ENC_BITRATE_ARG},
    {FLAG_ENC_FPS, required_argument, 0, ENC_FPS_ARG},
    {FLAG_ENC_INTRA_PERIOD, required_argument, 0, ENC_INTRA_PERIOD_ARG},
    {FLAG_ENC_CONTROL_RATE, required_argument, 0, ENC_CONTROL_RATE_ARG},
    {FLAG_ENC_MAX_BITRATE, required_argument, 0, ENC_MAX_BITRATE_ARG},
    {FLAG_ENC_SLICE_QP, required_argument, 0, ENC_SLICE_QP_ARG},
    {FLAG_ENC_MIN_QP, required_argument, 0, ENC_MIN_QP_ARG},
    {FLAG_ENC_MAX_QP, required_argument, 0, ENC_MAX_QP_ARG},
    {FLAG_ENC_NUM_BFRAMES, required_argument, 0, ENC_NUM_BFRAMES_ARG},
    {FLAG_ENC_IDR_PERIOD, required_argument, 0, ENC_IDR_PERIOD_ARG},
    {FLAG_DYN_IDR, required_argument, 0, DYNAMIC_IDR},
    {FLAG_ENC_PROFILE, required_argument, 0, ENC_PROFILE_ARG},
    {FLAG_ENC_LEVEL, required_argument, 0, ENC_LEVEL_ARG},
    {FLAG_ENC_NUM_SLICES, required_argument, 0, ENC_NUM_SLICES_ARG},
    {FLAG_ENC_QP_MODE, required_argument, 0, ENC_QP_MODE_ARG},
    {FLAG_ENC_ASPECT_RATIO, required_argument, 0, ENC_ASPECT_RATIO_ARG},
    {FLAG_ENC_SCALING_LIST, required_argument, 0, ENC_SCALING_LIST_ARG},
    {FLAG_ENC_LOOKAHEAD_DEPTH, required_argument, 0, ENC_LOOKAHEAD_DEPTH_ARG},
    {FLAG_ENC_TEMPORAL_AQ, required_argument, 0, ENC_TEMPORAL_AQ_ARG},
    {FLAG_ENC_SPATIAL_AQ, required_argument, 0, ENC_SPATIAL_AQ_ARG},
    {FLAG_ENC_SPATIAL_AQ_GAIN, required_argument, 0, ENC_SPATIAL_AQ_GAIN_ARG},
    {FLAG_ENC_QP, required_argument, 0, ENC_QP_ARG},
    {FLAG_ENC_NUM_CORES, required_argument, 0, ENC_NUM_CORES_ARG},
    {FLAG_ENC_TUNE_METRICS, required_argument, 0, ENC_TUNE_METRICS_ARG},
    {FLAG_ENC_LATENCY_LOGGING, required_argument, 0, ENC_LATENCY_LOGGING_ARG},
    {FLAG_ENC_EXPERT_OPTIONS, required_argument, 0, ENC_EXPERT_OPTIONS_ARG},
    {FLAG_ENC_OUTPUT_FILE, required_argument, 0, ENC_OUTPUT_FILE_ARG},
    {0, 0, 0, 0}};

XlnxEncProfileLookup enc_prof_h264_lookup[] = {
    {"baseline", ENC_H264_BASELINE},
    {"main", ENC_H264_MAIN},
    {"high", ENC_H264_HIGH},
    {"high-10", ENC_H264_HIGH_10},
    {"high-10-intra", ENC_H264_HIGH_10_INTRA}};

XlnxEncProfileLookup enc_prof_hevc_lookup[] = {
    {"main", ENC_HEVC_MAIN},
    {"main-intra", ENC_HEVC_MAIN_INTRA},
    {"main-10", ENC_HEVC_MAIN_10},
    {"main-10-intra", ENC_HEVC_MAIN_10_INTRA}};

#define ENC_PROF_H264_KEYS                                                     \
    (sizeof(enc_prof_h264_lookup) / sizeof(XlnxEncProfileLookup))
#define ENC_PROF_HEVC_KEYS                                                     \
    (sizeof(enc_prof_hevc_lookup) / sizeof(XlnxEncProfileLookup))

/**
 * xlnx_enc_key_from_string: maps string input to the encoder profile value
 *
 * @param enc_prof_lookuptable : encoder's lookup table
 * @param profile_key: encoder's profile in string format
 * @param enc_no_keys: number of keys in the lookup table
 * @return The encoder profile key value or XMA_APP_ERROR on error
 */
static int xlnx_enc_key_from_string(XlnxEncProfileLookup* enc_prof_lookuptable,
                                    char* profile_key, int enc_no_keys)
{
    int cnt;
    if(profile_key == NULL)
        return XMA_APP_ERROR;
    for(cnt = 0; cnt < enc_no_keys; cnt++) {
        XlnxEncProfileLookup enc_profile_key = enc_prof_lookuptable[cnt];
        if(strcmp(enc_profile_key.key, profile_key) == 0)
            return enc_profile_key.value;
    }

    return XMA_APP_ERROR;
}

/**
 * xlnx_enc_validate_arguments: Validates encoder arguments
 *
 * @param enc_ctx: Encoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_validate_arguments(XlnxEncoderProperties* enc_props)
{
    if(enc_props->dev_index < -1 ||
       enc_props->dev_index > XLNX_MAX_DEVICE_COUNT) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported device ID %d\n", enc_props->dev_index);
        return XMA_APP_ERROR;
    }
    if(enc_props->bit_rate <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid bit-rate param %d\n", enc_props->bit_rate);
        return XMA_APP_ERROR;
    }

    if(enc_props->fps <= 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid frame rate %d\n", enc_props->fps);
        return XMA_APP_ERROR;
    }

    if(enc_props->gop_size < 0 || enc_props->gop_size > ENC_MAX_GOP_SIZE) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid intra period %d\n", enc_props->gop_size);
        return XMA_APP_ERROR;
    }

    if((enc_props->control_rate < 0) || (enc_props->control_rate > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid rate control mode %d\n", enc_props->control_rate);
        return XMA_APP_ERROR;
    }

    if((enc_props->max_bitrate <= 0) ||
       (enc_props->max_bitrate > ENC_SUPPORTED_MAX_BITRATE)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid max btirate %ld\n", enc_props->max_bitrate);
        return XMA_APP_ERROR;
    }

    if((enc_props->slice_qp < -1) ||
       (enc_props->slice_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid slice qp %d\n", enc_props->slice_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->min_qp < ENC_SUPPORTED_MIN_QP) ||
       (enc_props->min_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid min qp %d\n", enc_props->min_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->max_qp < ENC_SUPPORTED_MIN_QP) ||
       (enc_props->max_qp > ENC_SUPPORTED_MAX_QP)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid max qp %d\n", enc_props->max_qp);
        return XMA_APP_ERROR;
    }

    if((enc_props->qp_mode < 0) || (enc_props->qp_mode > 2)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid qp mode %d\n", enc_props->qp_mode);
        return XMA_APP_ERROR;
    }

    if(enc_props->idr_period < ENC_DEFAULT_IDR_PERIOD) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid IDR period %d\n", enc_props->idr_period);
        return XMA_APP_ERROR;
    }

    if(enc_props->num_bframes > INT_MAX) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid number of B frames %d\n", enc_props->num_bframes);
        return XMA_APP_ERROR;
    }

    /* Check for valid number of b-frames in different gop-modes */
    switch(enc_props->gop_mode) {
        case 0:
            if(enc_props->num_bframes > 4) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Invalid number of B frames %d for GOP mode %d \n",
                           enc_props->num_bframes, enc_props->gop_mode);
                return XMA_APP_ERROR;
            } else
                break;
        case 1:
            if(!((enc_props->num_bframes == 3) ||
                 (enc_props->num_bframes == 5) ||
                 (enc_props->num_bframes == 7) ||
                 (enc_props->num_bframes == 15))) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Invalid number of B frames %d for GOP mode %d \n",
                           enc_props->num_bframes, enc_props->gop_mode);
                return XMA_APP_ERROR;
            } else
                break;
    }

    if(enc_props->gop_size < 0) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid GOP size %d\n", enc_props->gop_size);
        return XMA_APP_ERROR;
    }

    if((enc_props->aspect_ratio < 0) || (enc_props->aspect_ratio > 3)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid aspect ratio %d\n", enc_props->aspect_ratio);
        return XMA_APP_ERROR;
    }

    if((enc_props->scal_list != 0) && (enc_props->scal_list != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid scaling list value %d\n", enc_props->scal_list);
        return XMA_APP_ERROR;
    }

    if((enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH) ||
       (enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid LA depth %d\n", enc_props->lookahead_depth);
        return XMA_APP_ERROR;
    }

    if((enc_props->temporal_aq != 0) && (enc_props->temporal_aq != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid temporal aq value %d\n", enc_props->temporal_aq);
        return XMA_APP_ERROR;
    }

    if((enc_props->spatial_aq != 0) && (enc_props->spatial_aq != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid spatial aq value %d\n", enc_props->spatial_aq);
        return XMA_APP_ERROR;
    }

    if((enc_props->spatial_aq_gain < 0) ||
       (enc_props->spatial_aq_gain > ENC_MAX_SPAT_AQ_GAIN)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid spatial aq gain %d\n", enc_props->spatial_aq_gain);
        return XMA_APP_ERROR;
    }

    if((enc_props->num_cores < 0) && (enc_props->num_cores > 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid number of cores %d\n", enc_props->num_cores);
        return XMA_APP_ERROR;
    }

    if((enc_props->tune_metrics != 0) && (enc_props->tune_metrics != 1)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid value for Tune metrics %d\n",
                   enc_props->tune_metrics);
        return XMA_APP_ERROR;
    }

    if(enc_props->codec_id == ENCODER_ID_H264) {
        if(enc_props->level < 10 || enc_props->level > 52) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Unsupported H264 Level %d\n", enc_props->level);
            return XMA_APP_ERROR;
        }
    } else if(enc_props->codec_id == ENCODER_ID_HEVC) {
        if(enc_props->level < 10 || enc_props->level > 51) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Unsupported HEVC Level %d\n", enc_props->level);
            return XMA_APP_ERROR;
        }
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_la_init: Lookahead module initialization
 *
 * @param enc_ctx: Encoder context
 * @param app_xrm_ctx: Transcoder XRM context
 * @param xma_la_props: Lookahead XMA properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_la_init(XlnxEncoderCtx*       enc_ctx,
                                XlnxTranscoderXrmCtx* app_xrm_ctx,
                                XmaFilterProperties*  xma_la_props)
{
    int32_t                  ret       = XMA_APP_ERROR;
    XlnxLookaheadProperties* la_props  = &enc_ctx->la_ctx.la_props;
    XlnxEncoderProperties*   enc_props = &enc_ctx->enc_props;

    la_props->dev_index             = enc_props->dev_index;
    la_props->width                 = enc_props->width;
    la_props->height                = enc_props->height;
    la_props->framerate.numerator   = enc_props->fps;
    la_props->framerate.denominator = 1;

    // TODO: Assume 256 aligned for now. Needs to be fixed later
    la_props->stride = XLNX_ALIGN(enc_props->width, ENC_IN_STRIDE_ALIGN);
    la_props->bits_per_pixel = enc_props->bits_per_pixel;

    if(enc_props->gop_size <= 0) {
        la_props->gop_size = ENC_DEFAULT_GOP_SIZE;
    } else {
        la_props->gop_size = enc_props->gop_size;
    }

    la_props->lookahead_depth   = enc_props->lookahead_depth;
    la_props->spatial_aq_mode   = enc_props->spatial_aq;
    la_props->spatial_aq_gain   = enc_props->spatial_aq_gain;
    la_props->temporal_aq_mode  = enc_props->temporal_aq;
    la_props->rate_control_mode = enc_props->custom_rc;
    la_props->num_bframes       = enc_props->num_bframes;
    la_props->latency_logging   = enc_props->latency_logging;

    /* Only NV12 format is supported in this application */
    la_props->xma_fmt_type  = enc_props->xma_fmt_type;
    la_props->enable_hw_buf = 1;

    switch(enc_props->codec_id) {
        case ENCODER_ID_H264:
            la_props->codec_type = LOOKAHEAD_ID_H264;
            break;
        case ENCODER_ID_HEVC:
            la_props->codec_type = LOOKAHEAD_ID_HEVC;
            break;
    }

    ret = xlnx_la_create(&enc_ctx->la_ctx, app_xrm_ctx, xma_la_props);
    if(ret != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error : init_la : create_xlnx_la Failed\n");
        return XMA_APP_ERROR;
    }

    enc_ctx->la_bypass = xlnx_la_in_bypass_mode(&enc_ctx->la_ctx);

    return ret;
}

/**
 * xlnx_enc_context_init: Initializes the encoder context with default values.
 *
 * @param enc_ctx: Encoder context
 */
void xlnx_enc_context_init(XlnxEncoderCtx* enc_ctx)
{

    XlnxEncoderProperties* enc_props = &enc_ctx->enc_props;
    /* Initialize the encoder parameters to default */
    enc_props->dev_index      = DEFAULT_DEVICE_ID;
    enc_props->codec_id       = -1;
    enc_props->width          = ENC_DEFAULT_WIDTH;
    enc_props->height         = ENC_DEFAULT_HEIGHT;
    enc_props->bits_per_pixel = BITS_PER_PIXEL_8;
    enc_props->bit_rate       = ENC_DEFAULT_BITRATE;
    enc_props->fps            = ENC_DEFAULT_FRAMERATE;
    enc_props->gop_size       = ENC_DEFAULT_GOP_SIZE;
    enc_props->slice_qp       = -1;
    enc_props->control_rate   = 1;
    enc_props->custom_rc      = 0;
    enc_props->max_bitrate    = ENC_DEFAULT_MAX_BITRATE;
    enc_props->min_qp         = 0;
    enc_props->max_qp         = ENC_SUPPORTED_MAX_QP;
    enc_props->cpb_size       = 2.0;
    enc_props->initial_delay  = 1.0;
    enc_props->gop_mode       = 0;
    enc_props->gdr_mode       = 0;
    enc_props->num_bframes    = ENC_DEFAULT_NUM_B_FRAMES;
    enc_props->idr_period     = ENC_DEFAULT_IDR_PERIOD;
    /* Assigning the default profile as HEVC profile. If the codec optin
       is H264, this will be updated */
    enc_props->profile                = ENC_HEVC_MAIN;
    enc_props->level                  = ENC_DEFAULT_LEVEL;
    enc_props->tier                   = 0;
    enc_props->num_slices             = 1;
    enc_props->qp_mode                = 1;
    enc_props->aspect_ratio           = 0;
    enc_props->lookahead_depth        = 0;
    enc_props->temporal_aq            = 1;
    enc_props->spatial_aq             = 1;
    enc_props->spatial_aq_gain        = ENC_DEFAULT_SPAT_AQ_GAIN;
    enc_props->scal_list              = 1;
    enc_props->filler_data            = 0;
    enc_props->dependent_slice        = 0;
    enc_props->slice_size             = 0;
    enc_props->entropy_mode           = 1;
    enc_props->loop_filter            = 1;
    enc_props->constrained_intra_pred = 0;
    enc_props->prefetch_buffer        = 1;
    enc_props->enable_hw_buf          = 1;
    enc_props->latency_logging        = 0;
    enc_props->num_cores              = 0;
    enc_props->tune_metrics           = 0;
    enc_props->xma_fmt_type           = XMA_VCU_NV12_FMT_TYPE;
    enc_ctx->flush_frame_sent         = 0;
}

/**
 * xlnx_enc_update_props: Updates xma encoder properties and options that will
 * be sent to xma plugin
 *
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_update_props(XlnxEncoderCtx*       enc_ctx,
                              XmaEncoderProperties* xma_enc_props)
{

    XlnxEncoderProperties* enc_props = &enc_ctx->enc_props;
    enc_props->enc_options           = calloc(1, ENC_MAX_OPTIONS_SIZE);

    /* Enable custom rate control when rate control is set to CBR and
    lookahead is set, disable when expert option lookahead-rc-off is set. */
    if((enc_props->control_rate == 1) && (enc_props->lookahead_depth > 1)) {
        enc_props->custom_rc = 1;
    }

    if((enc_props->lookahead_depth > enc_props->gop_size) ||
       ((enc_props->idr_period >= 0) &&
        (enc_props->lookahead_depth > enc_props->idr_period))) {
        xma_logmsg(
            XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
            "LA Depth %d cannot be more than GOP size(%d) or IDR period(%d)\n",
            enc_props->lookahead_depth, enc_props->gop_size,
            enc_props->idr_period);
        return XMA_APP_ERROR;
    }

    if((enc_props->width > ENC_SUPPORTED_MAX_WIDTH) ||
       (enc_props->height > ENC_SUPPORTED_MAX_WIDTH) ||
       ((enc_props->width * enc_props->height) > ENC_SUPPORTED_MAX_PIXELS)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Input resolution %dx%d exceeds maximum supported "
                   "resolution %dx%d\n",
                   enc_props->width, enc_props->height, ENC_SUPPORTED_MAX_WIDTH,
                   ENC_SUPPORTED_MAX_HEIGHT, ENC_SUPPORTED_MAX_HEIGHT,
                   ENC_SUPPORTED_MAX_WIDTH);
        return XMA_APP_ERROR;
    }

    if((enc_props->width < ENC_SUPPORTED_MIN_WIDTH) || (enc_props->width % 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported width %d\n", enc_props->width);
        return XMA_APP_ERROR;
    }

    if((enc_props->height < ENC_SUPPORTED_MIN_HEIGHT) ||
       (enc_props->height % 4)) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unsupported height %d\n", enc_props->height);
        return XMA_APP_ERROR;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if ((enc_props->lookahead_depth >= 1) && (enc_props->tune_metrics == 0)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Setting qp mode to 2, as the lookahead params are set\n");
        enc_props->qp_mode = 2;
    } else if((enc_props->lookahead_depth == 0) ||
              (enc_props->tune_metrics == 1)) {
        if(enc_props->temporal_aq)
            enc_props->temporal_aq = 0;

        if(enc_props->spatial_aq)
            enc_props->spatial_aq = 0;
    }

    /* When lookahead is enabled and user hasn't specified min-qp value,
       set min-qp to 20 as this gives better R-D performance */
    if(enc_props->lookahead_depth > 0 && enc_props->min_qp == 0) {
        enc_props->min_qp = 20;
    }

    /* Tunes video quality for objective scores by setting flat scaling-list
       and uniform qp-mode */
    if(enc_props->tune_metrics) {
        enc_props->scal_list = 0;
        enc_props->qp_mode   = 0;
    }

    /* Set IDR period to gop-size, when the user has not specified it on
       the command line */
    if(enc_props->idr_period == ENC_DEFAULT_IDR_PERIOD) {
        if(enc_props->gop_size > 0) {
            enc_props->idr_period = enc_props->gop_size;
        }
        xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Setting IDR period to GOP size\n");
    }

    if(enc_props->bits_per_pixel == BITS_PER_PIXEL_8) {
        enc_props->xma_fmt_type = XMA_VCU_NV12_FMT_TYPE;
    } else if(enc_props->bits_per_pixel == BITS_PER_PIXEL_10) {
        enc_props->xma_fmt_type = XMA_VCU_NV12_10LE32_FMT_TYPE;
    }

    return xlnx_enc_get_xma_props(enc_props, xma_enc_props);
}

/**
 * xlnx_utils_get_br_in_kbps: Get value of bit rate
 * @param desination: Where to store the bitrate
 * @param source: User input value
 * @param param_name: Name of the parameter
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_utils_get_br_in_kbps(int64_t* destination, char* source,
                                     char* param_name)
{
    float br_in_kbps = atof(source);
    if(xlnx_utils_check_if_pattern_matches("^-?[0-9]*\\.?[0-9]+[M|m|K|k]*$",
                                           source) == 0) {

        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Unrecognized value "
                   "\"%s\" for argument -%s! Make sure the value is of proper "
                   "type.\n",
                   source, param_name);
        return XMA_APP_ERROR;
    }

    if(xlnx_utils_check_if_pattern_matches("[M|m]+", source)) {
        *destination = br_in_kbps * 1000;
    } else if(xlnx_utils_check_if_pattern_matches("[K|k]+", source)) {
        *destination = br_in_kbps;
    } else {
        *destination = (br_in_kbps / 1000);
    }
    return XMA_APP_SUCCESS;
}

/**
 * Retrieve tokens from a string
 * @param idr_input User's input idr periods
 * @param dyanmic_idr_periods Integer array for idr periods
 * @return Number of dynamic idr values in the user's input string
 */
static int xlnx_enc_retrieve_token(char* idr_input, uint32_t* dyn_idr_arr)
{
    int        count        = 0, ret;
    const char delimiters[] = "( , )";
    char*      token        = strtok(idr_input, delimiters);
    while(token != NULL) {
        ret = xlnx_utils_set_uint_arg(&dyn_idr_arr[count], token, FLAG_DYN_IDR);
        if(ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
        token = strtok(NULL, delimiters);
        count++;
    }
    return count;
}

/**
 * Retrive user's input idr period and save in integer array
 * @param optargs User's input idr periods
 * @param dynamic_idr Struct for dynamic idr parameters
 * @return XMA_APP_SUCCESS on success, XMA_APP_ERROR on error
 */
static int xlnx_enc_get_idr_frames(char* optargs, XlnxDynIdrFrames* dynamic_idr)
{
    int count = 0, i = 0;
    while(optargs[i] != '\0') {
        if(optargs[i] == ',' ||
           (optargs[i] == ' ' &&
            (optargs[i - 1] != ' ' && optargs[i - 1] != ','))) {
            count++;
        }
        i++;
    }
    dynamic_idr->idr_arr = (uint32_t*)calloc(count + 1, sizeof(uint32_t));
    count = xlnx_enc_retrieve_token(optargs, dynamic_idr->idr_arr);
    if(count == XMA_APP_ERROR) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Invalid value for dynamic idr frame");
        return count;
    }
    dynamic_idr->idr_arr_idx = 0;
    dynamic_idr->len_idr_arr = count;

    qsort(dynamic_idr->idr_arr, dynamic_idr->len_idr_arr, sizeof(uint32_t),
          xlnx_utils_compare);

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_parse_expert_options: Parses the encoder expert arguments if present
 * on command line
 * @param optargs: User's input string having the key:value pair
 * @param enc_ctx: Encoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int xlnx_enc_parse_expert_options(char* optargs, XlnxEncoderCtx* enc_ctx)
{
    const char            delimiters[]   = "=";
    char*                 key            = strtok(optargs, delimiters);
    char*                 value          = NULL;
    XlnxEncExpertOptions* expert_options = &enc_ctx->expert_options;
    expert_options->dynamic_params_check = 0;
    if(key != NULL) {
        if(!strcmp(key, "dynamic-params")) {
            value = strtok(NULL, delimiters);
            if(value != NULL) {
                expert_options->dynamic_params_file = value;
                if(access(expert_options->dynamic_params_file, F_OK) != 0) {
                    xma_logmsg(
                        XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Unable to access dynamic encoder input parameters "
                        "file \"%s\"\n",
                        expert_options->dynamic_params_file);
                    return XMA_APP_ERROR;
                }
                expert_options->dynamic_params_check = 1;
            } else {
                xma_logmsg(
                    XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                    "Missing value for expert option with key=%s in command "
                    "line",
                    key);
                return XMA_APP_ERROR;
            }
        } else {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "Incorrect expert option key (%s) in command line", key);
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_load_dyn_params_lib: Loads dynamic params shared library and stores it's
 * symbols in encoder context
 *
 * @param ctx: Encoder context
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_load_dyn_params_lib(XlnxEncoderCtx* ctx)
{
    char* dlret;

    ctx->enc_dyn_params.dyn_params_lib = dlopen(DYN_PARAMS_LIB_NAME, RTLD_NOW);
    if(!ctx->enc_dyn_params.dyn_params_lib) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error loading %s : %s\n", DYN_PARAMS_LIB_NAME, dlerror());
        return XMA_APP_ERROR;
    }
    xma_logmsg(XMA_DEBUG_LOG, XLNX_TRANSCODER_APP_MODULE,
               "Dynamic params plugin path: %s \n", DYN_PARAMS_LIB_NAME);

    ctx->enc_dyn_params.xlnx_enc_init_dyn_params_obj = (InitDynParams)dlsym(
        ctx->enc_dyn_params.dyn_params_lib, XLNX_ENC_INIT_DYN_PARAMS_OBJ);
    dlret = dlerror();
    if(dlret != NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error loading symbol %s from %s plugin: %s\n",
                   XLNX_ENC_INIT_DYN_PARAMS_OBJ, DYN_PARAMS_LIB_NAME, dlret);
        return XMA_APP_ERROR;
    }
    (*(ctx->enc_dyn_params.xlnx_enc_init_dyn_params_obj))(
        &ctx->enc_dyn_params.dyn_params_obj);

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_load_get_dyn_params: Loads dynamic params shared library and stores
 * dynamic encoder parameters in encoder context
 *
 * @param ctx: Encoder context
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_enc_load_get_dyn_params(XlnxEncoderCtx* enc_ctx)
{
    if(enc_ctx->expert_options.dynamic_params_check) {
        if(xlnx_load_dyn_params_lib(enc_ctx) != XMA_APP_SUCCESS) {
            return XMA_APP_ERROR;
        }

        enc_ctx->enc_dyn_params.dynamic_param_handle = (DynparamsHandle)(
            *(enc_ctx->enc_dyn_params.dyn_params_obj.xlnx_enc_get_dyn_params))(
            enc_ctx->expert_options.dynamic_params_file,
            &enc_ctx->enc_dyn_params.dynamic_params_count);

        if(enc_ctx->enc_dyn_params.dynamic_param_handle == NULL) {
            return XMA_APP_ERROR;
        }
    }
    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_parse_args: Parses the command line arguments
 *
 * @param argc: Number of arguments
 * @param argv: Pointer to the arguments
 * @param enc_ctx: Encoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_parse_args(int32_t argc, char* argv[], XlnxEncoderCtx* enc_ctx,
                            int32_t param_flag)
{
    int32_t                flag = 0;
    int32_t                option_index;
    int32_t                ret         = XMA_APP_SUCCESS;
    int32_t                channel_end = 0;
    XlnxEncoderProperties* enc_props   = &enc_ctx->enc_props;

    while(!channel_end) {
        if(param_flag == 0) {
            flag = getopt_long_only(argc, argv, "", enc_options, &option_index);
            if(flag == -1) {
                ret = TRANSCODE_PARSING_DONE;
                break;
            }
        } else {
            flag       = param_flag;
            param_flag = 0;
        }
        switch(flag) {
            case ENC_OUTPUT_FILE_ARG:
                enc_ctx->out_file = open(optarg, O_WRONLY | O_CREAT | O_TRUNC,
                                         S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                if(enc_ctx->out_file == XMA_APP_ERROR) {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Error opening output file %s\n", optarg);
                    return XMA_APP_ERROR;
                }
                channel_end = 1;
                break;

            case ENC_CODEC_ID_ARG:
                if(!strcmp(optarg, "mpsoc_vcu_h264")) {
                    enc_props->codec_id = ENCODER_ID_H264;
                    /* Change the default profile from HEVC_MAIN to H264_HIGH*/
                    enc_props->profile = ENC_H264_HIGH;
                } else if(!strcmp(optarg, "mpsoc_vcu_hevc")) {
                    enc_props->codec_id = ENCODER_ID_HEVC;
                } else {
                    xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                               "Unsupported codec %s\n", optarg);
                    return XMA_APP_ERROR;
                }
                break;

            case ENC_DEVICE_ID_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->dev_index, optarg,
                                             FLAG_ENC_DEVICE_ID);
                break;

            case ENC_CONVERT_INPUT_ARG:
                ret                      = XMA_APP_SUCCESS;
                enc_ctx->convert_input   = true;
                enc_props->enable_hw_buf = false; // host buffer, not hardware
                break;

            case ENC_BITRATE_ARG:
                ret = xlnx_utils_get_br_in_kbps(&enc_props->bit_rate, optarg,
                                                FLAG_ENC_BITRATE);
                break;

            case ENC_FPS_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->fps, optarg,
                                             FLAG_ENC_FPS);
                break;

            case ENC_INTRA_PERIOD_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->gop_size, optarg,
                                             FLAG_ENC_INTRA_PERIOD);
                break;

            case ENC_CONTROL_RATE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->control_rate, optarg,
                                             FLAG_ENC_CONTROL_RATE);
                break;

            case ENC_MAX_BITRATE_ARG:
                ret = xlnx_utils_get_br_in_kbps(&enc_props->max_bitrate, optarg,
                                                FLAG_ENC_MAX_BITRATE);
                break;

            case ENC_SLICE_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->slice_qp, optarg,
                                             FLAG_ENC_SLICE_QP);
                break;

            case ENC_MIN_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->min_qp, optarg,
                                             FLAG_ENC_MIN_QP);
                break;

            case ENC_MAX_QP_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->max_qp, optarg,
                                             FLAG_ENC_MAX_QP);
                break;

            case ENC_NUM_BFRAMES_ARG:
                ret = xlnx_utils_set_uint_arg(&enc_props->num_bframes, optarg,
                                              FLAG_ENC_NUM_BFRAMES);
                break;

            case ENC_IDR_PERIOD_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->idr_period, optarg,
                                             FLAG_ENC_IDR_PERIOD);
                break;

            case DYNAMIC_IDR:
                ret = xlnx_enc_get_idr_frames(optarg, &enc_ctx->dynamic_idr);
                break;

            case ENC_PROFILE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->profile, optarg, "");
                if(ret == XMA_APP_ERROR) {
                    if(enc_props->codec_id == ENCODER_ID_H264) {
                        ret = xlnx_enc_key_from_string(
                            enc_prof_h264_lookup, optarg, ENC_PROF_H264_KEYS);
                        if(ret == XMA_APP_ERROR) {
                            xma_logmsg(XMA_ERROR_LOG,
                                       XLNX_TRANSCODER_APP_MODULE,
                                       "Invalid H264 codec profile value %s\n",
                                       optarg);
                        } else {
                            enc_props->profile = ret;
                        }
                    } else {
                        ret = xlnx_enc_key_from_string(
                            enc_prof_hevc_lookup, optarg, ENC_PROF_HEVC_KEYS);
                        if(ret == XMA_APP_ERROR) {
                            xma_logmsg(XMA_ERROR_LOG,
                                       XLNX_TRANSCODER_APP_MODULE,
                                       "Invalid HEVC codec profile value %s\n",
                                       optarg);
                        } else {
                            enc_props->profile = ret;
                        }
                    }
                }
                break;

            case ENC_LEVEL_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->level, optarg,
                                             FLAG_ENC_LEVEL);
                break;

            case ENC_NUM_SLICES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_slices, optarg,
                                             FLAG_ENC_NUM_SLICES);
                break;

            case ENC_QP_MODE_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->qp_mode, optarg,
                                             FLAG_ENC_QP_MODE);
                break;

            case ENC_ASPECT_RATIO_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->aspect_ratio, optarg,
                                             FLAG_ENC_ASPECT_RATIO);
                break;

            case ENC_SCALING_LIST_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->scal_list, optarg,
                                             FLAG_ENC_SCALING_LIST);
                break;

            case ENC_LOOKAHEAD_DEPTH_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->lookahead_depth,
                                             optarg, FLAG_ENC_LOOKAHEAD_DEPTH);
                break;

            case ENC_TEMPORAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->temporal_aq, optarg,
                                             FLAG_ENC_TEMPORAL_AQ);
                break;

            case ENC_SPATIAL_AQ_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq, optarg,
                                             FLAG_ENC_SPATIAL_AQ);
                break;

            case ENC_SPATIAL_AQ_GAIN_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->spatial_aq_gain,
                                             optarg, FLAG_ENC_SPATIAL_AQ_GAIN);
                break;

            case ENC_NUM_CORES_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->num_cores, optarg,
                                             FLAG_ENC_NUM_CORES);
                break;

            case ENC_TUNE_METRICS_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->tune_metrics, optarg,
                                             FLAG_ENC_TUNE_METRICS);
                break;

            case ENC_LATENCY_LOGGING_ARG:
                ret = xlnx_utils_set_int_arg(&enc_props->latency_logging,
                                             optarg, FLAG_ENC_LATENCY_LOGGING);
                break;

            case ENC_EXPERT_OPTIONS_ARG:
                ret = xlnx_enc_parse_expert_options(optarg, enc_ctx);
                if(ret != XMA_APP_ERROR) {
                    ret = xlnx_enc_load_get_dyn_params(enc_ctx);
                }
                break;

            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                           "Failed to parse encoder arguments %d\n", flag);
                return XMA_APP_ERROR;
        }

        if(ret == XMA_APP_ERROR) {
            return XMA_APP_ERROR;
        }
    }

    ret |= xlnx_enc_validate_arguments(enc_props);
    return ret;
}

/**
 * enc_session: Creates encoder session
 *
 * @param app_xrm_ctx: Transcoder XRM context
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @param xma_la_props: XMA lookahead properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_session(XlnxTranscoderXrmCtx* app_xrm_ctx,
                         XlnxEncoderCtx*       enc_ctx,
                         XmaEncoderProperties* xma_enc_props,
                         XmaFilterProperties*  xma_la_props)
{

    /* Lookahead session creation*/
    if(xlnx_enc_la_init(enc_ctx, app_xrm_ctx, xma_la_props) !=
       XMA_APP_SUCCESS) {
        return XMA_APP_ERROR;
    }

    if(xlnx_enc_cu_alloc(app_xrm_ctx, xma_enc_props,
                         &enc_ctx->encode_cu_list_res) != XMA_APP_SUCCESS) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Error in encoder CU allocation\n");
        return XMA_APP_ERROR;
    }

    /* Encoder session creation */
    enc_ctx->enc_session = xma_enc_session_create(xma_enc_props);
    if(enc_ctx->enc_session == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                   "Encoder init failed\n");
        return XMA_APP_ERROR;
    }

    return XMA_APP_SUCCESS;
}

/**
 * xlnx_enc_send_frame: Sends YUV input to the encoder
 *
 * @param enc_ctx: Encoder context
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_send_frame(XlnxEncoderCtx* enc_ctx)
{
    int32_t   ret        = XMA_APP_ERROR;
    XmaFrame* send_frame = enc_ctx->send_frame;
    if((ret = xma_enc_session_send_frame(enc_ctx->enc_session, send_frame)) <=
       XMA_APP_ERROR) {
        XvbmBufferHandle xvbm_handle =
            (XvbmBufferHandle)(enc_ctx->in_frame->data[0].buffer);
        if(xvbm_handle) {
            xvbm_buffer_pool_entry_free(xvbm_handle);
        }
    } else if(ret == XMA_SUCCESS || ret == XMA_SEND_MORE_DATA) {
        enc_ctx->num_frames_sent++;
        /* Necessary to set to NULL for convert usecase. Otherwise it will write
         * raw data to freed buffer. */
        send_frame->data[0].buffer = NULL;
    }

    return ret;
}

/**
 * xlnx_enc_recv_frame: Receives encoder output
 *
 * @param enc_ctx: Encoder context
 * @param _enc_out_size: Encoder output size
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_recv_frame(XlnxEncoderCtx* enc_ctx,
                                   int32_t*        enc_out_size)
{

    int32_t ret = XMA_APP_ERROR;
    ret = xma_enc_session_recv_data(enc_ctx->enc_session, enc_ctx->xma_buffer,
                                    enc_out_size);

    return ret;
}

/**
 * xlnx_enc_process_frame: Encoder process frame
 *
 * @param enc_ctx: Encoder context
 * @param _enc_null_frame: Encoder null frame flag
 * @param _enc_out_size: Encoder output size
 * @return The result of send_frame if unsuccessful, the result of recv if
 * send was successful, XMA_APP_ERROR on error.
 */
int32_t xlnx_enc_process_frame(XlnxEncoderCtx* enc_ctx, int32_t enc_null_frame,
                               int32_t* enc_out_size)
{
    int32_t ret = XMA_APP_SUCCESS;

    if(enc_null_frame) {
        enc_ctx->send_frame->is_last_frame = 1;
        enc_ctx->send_frame->pts           = -1;
    }

    if(!enc_ctx->flush_frame_sent) {
        do {
            ret = xlnx_enc_send_frame(enc_ctx);

            if(enc_ctx->send_frame && !enc_null_frame) {
                /* Release the LA output frame(encoder input frame), once
                encoder consumes it. This will be reused by LA module */
                xlnx_la_release_frame(&enc_ctx->la_ctx, enc_ctx->send_frame);
            }
        } while(ret == XMA_FLUSH_AGAIN);
    }

    if(enc_null_frame) {
        enc_ctx->flush_frame_sent = 1;
    }

    if(ret == XMA_APP_SUCCESS) {
        if((ret = xlnx_enc_recv_frame(enc_ctx, enc_out_size)) <=
           XMA_APP_ERROR) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                       "xma_enc_session_recv_data failed with error %d\n", ret);
            return XMA_APP_ERROR;
        }
    }

    return ret;
}

/**
 * xlnx_enc_deinit: Deinitialize encoder module
 *
 * @param xrm_ctx: XRM context
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @param xma_la_props: XMA lookahead properties
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_deinit(xrmContext xrm_ctx, XlnxEncoderCtx* enc_ctx,
                        XmaEncoderProperties* xma_enc_props,
                        XmaFilterProperties*  xma_la_props)
{

    int32_t ret = XMA_APP_ERROR;
    if(enc_ctx->enc_session != NULL) {
        ret = xma_enc_session_destroy(enc_ctx->enc_session);
    }

    xlnx_enc_free_xma_props(xma_enc_props);

    if(enc_ctx->dynamic_idr.len_idr_arr) {
        free(enc_ctx->dynamic_idr.idr_arr);
    }

    close(enc_ctx->out_file);
    xlnx_la_close(xrm_ctx, &enc_ctx->la_ctx, xma_la_props);

    ret         = xrmCuListReleaseV2(xrm_ctx, &enc_ctx->encode_cu_list_res);

    return ret;
}
