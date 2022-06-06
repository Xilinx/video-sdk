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

#include "xlnx_enc_xma_props.h"

static const char* XLNX_LA_EXT_PARAMS[] = {"ip",
                                           "lookahead_depth",
                                           "enable_hw_in_buf",
                                           "spatial_aq_mode",
                                           "temporal_aq_mode",
                                           "rate_control_mode",
                                           "spatial_aq_gain",
                                           "num_b_frames",
                                           "codec_type",
                                           "latency_logging"};

/**
 * xlnx_enc_free_xma_props: Function to deinitialize XMA encoder properties
 *
 * @param xma_enc_props: XMA encoder properties
 * @return None
 */
void xlnx_enc_free_xma_props(XmaEncoderProperties* xma_enc_props)
{
    if(xma_enc_props->params) {
        if(xma_enc_props->params[0].value) {
            ; // free(xma_enc_props->params[0].value);
        }
        free(xma_enc_props->params);
    }

    return;
}

/**
 * xlnx_enc_create_xma_props: Populate XmaEncoderProperties struct from encoder
 * properties
 *
 * @param enc_props: Encoder properties
 * @param xma_enc_props: XMA encoder properties
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_enc_create_xma_enc_options(XlnxEncoderProperties* enc_props,
                                               XmaParameter* enc_options)
{
    enc_props->enc_options = calloc(1, 4096);
    enc_options->name      = "enc_options";
    enc_options->type      = XMA_STRING;

    const char* format =
        enc_props->bits_per_pixel == 10 ? "NV12_10LE32" : "NV12";

    /* Update encoder options */
    const char* RateCtrlMode = "CONST_QP";
    switch(enc_props->control_rate) {
        case 0:
            RateCtrlMode = "CONST_QP";
            break;
        case 1:
            RateCtrlMode = "CBR";
            break;
        case 2:
            RateCtrlMode = "VBR";
            break;
        case 3:
            RateCtrlMode = "LOW_LATENCY";
            break;
    }

    char    FrameRate[16];
    int32_t fps_den = 1;
    sprintf(FrameRate, "%d/%d", enc_props->fps, fps_den);

    char SliceQP[8];
    if(enc_props->slice_qp == -1) {
        strcpy(SliceQP, "AUTO");
    } else {
        sprintf(SliceQP, "%d", enc_props->slice_qp);
    }

    const char* GopCtrlMode = "DEFAULT_GOP";
    switch(enc_props->gop_mode) {
        case 0:
            GopCtrlMode = "DEFAULT_GOP";
            break;
        case 1:
            GopCtrlMode = "PYRAMIDAL_GOP";
            break;
        case 2:
            GopCtrlMode = "LOW_DELAY_P";
            break;
        case 3:
            GopCtrlMode = "LOW_DELAY_B";
            break;
    }

    const char* GDRMode = "DISABLE";
    switch(enc_props->gdr_mode) {
        case 0:
            GDRMode = "DISABLE";
            break;
        case 1:
            GDRMode = "GDR_VERTICAL";
            break;
        case 2:
            GDRMode = "GDR_HORIZONTAL";
            break;
    }

    const char* Profile = "AVC_BASELINE";
    if(enc_props->codec_id == ENCODER_ID_H264) {
        switch(enc_props->profile) {
            case ENC_H264_BASELINE:
                Profile = "AVC_BASELINE";
                break;
            case ENC_H264_MAIN:
                Profile = "AVC_MAIN";
                break;
            case ENC_H264_HIGH:
                Profile = "AVC_HIGH";
                break;
            case ENC_H264_HIGH_10:
                Profile = "AVC_HIGH10";
                break;
            case ENC_H264_HIGH_10_INTRA:
                Profile = "AVC_HIGH10_INTRA";
                break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Invalid H264 codec profile value %d\n",
                           enc_props->profile);
                return XMA_ERROR;
        }
    } else if(enc_props->codec_id == ENCODER_ID_HEVC) {
        Profile = "HEVC_MAIN";
        switch(enc_props->profile) {
            case ENC_HEVC_MAIN:
                Profile = "HEVC_MAIN";
                break;
            case ENC_HEVC_MAIN_INTRA:
                Profile = "HEVC_MAIN_INTRA";
                break;
            case ENC_HEVC_MAIN_10:
                Profile = "HEVC_MAIN10";
                break;
            case ENC_HEVC_MAIN_10_INTRA:
                Profile = "HEVC_MAIN10_INTRA";
                break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                           "Invalid HEVC codec profile value %d\n",
                           enc_props->profile);
                return XMA_ERROR;
        }
    }

    const char* Level           = "1";
    uint8_t     is_level_common = 1;
    switch(enc_props->level) {
        case 10:
            Level = "1";
            break;
        case 20:
            Level = "2";
            break;
        case 21:
            Level = "2.1";
            break;
        case 30:
            Level = "3";
            break;
        case 31:
            Level = "3.1";
            break;
        case 40:
            Level = "4";
            break;
        case 41:
            Level = "4.1";
            break;
        case 50:
            Level = "5";
            break;
        case 51:
            Level = "5.1";
            break;
        default:
            is_level_common = 0;
            break;
    }
    if(!is_level_common) {
        if(enc_props->codec_id == ENCODER_ID_H264) {
            switch(enc_props->level) {
                case 11:
                    Level = "1.1";
                    break;
                case 12:
                    Level = "1.2";
                    break;
                case 13:
                    Level = "1.3";
                    break;
                case 22:
                    Level = "2.2";
                    break;
                case 32:
                    Level = "3.2";
                    break;
                case 42:
                    Level = "4.2";
                    break;
                case 52:
                    Level = "5.2";
                    break;
            }
        }
    }

    const char* Tier = "MAIN_TIER";
    switch(enc_props->tier) {
        case 0:
            Tier = "MAIN_TIER";
            break;
        case 1:
            Tier = "HIGH_TIER";
            break;
    }

    const char* QPCtrlMode = "UNIFORM_QP";
    switch(enc_props->qp_mode) {
        case 0:
            QPCtrlMode = "UNIFORM_QP";
            break;
        case 1:
            QPCtrlMode = "AUTO_QP";
            break;
        case 2:
            QPCtrlMode = "LOAD_QP | RELATIVE_QP";
            break;
    }

    const char* DependentSlice = "FALSE";
    switch(enc_props->dependent_slice) {
        case 0:
            DependentSlice = "FALSE";
            break;
        case 1:
            DependentSlice = "TRUE";
            break;
    }

    const char* FillerData = "ENABLE";
    switch(enc_props->filler_data) {
        case 0:
            FillerData = "DISABLE";
            break;
        case 1:
            FillerData = "ENABLE";
            break;
    }

    const char* AspectRatio = "ASPECT_RATIO_AUTO";
    switch(enc_props->aspect_ratio) {
        case 0:
            AspectRatio = "ASPECT_RATIO_AUTO";
            break;
        case 1:
            AspectRatio = "ASPECT_RATIO_4_3";
            break;
        case 2:
            AspectRatio = "ASPECT_RATIO_16_9";
            break;
        case 3:
            AspectRatio = "ASPECT_RATIO_NONE";
            break;
    }

    const char* ColorSpace = "COLOUR_DESC_UNSPECIFIED";

    const char* ScalingList = "FLAT";
    switch(enc_props->scal_list) {
        case 0:
            ScalingList = "FLAT";
            break;
        case 1:
            ScalingList = "DEFAULT";
            break;
    }

    const char* LoopFilter = "ENABLE";
    switch(enc_props->loop_filter) {
        case 0:
            LoopFilter = "DISABLE";
            break;
        case 1:
            LoopFilter = "ENABLE";
            break;
    }

    const char* EntropyMode = "MODE_CABAC";
    switch(enc_props->entropy_mode) {
        case 0:
            EntropyMode = "MODE_CAVLC";
            break;
        case 1:
            EntropyMode = "MODE_CABAC";
            break;
    }

    const char* ConstIntraPred = "ENABLE";
    switch(enc_props->constrained_intra_pred) {
        case 0:
            ConstIntraPred = "DISABLE";
            break;
        case 1:
            ConstIntraPred = "ENABLE";
            break;
    }

    const char* LambdaCtrlMode = "DEFAULT_LDA";

    const char* PrefetchBuffer = "DISABLE";
    switch(enc_props->prefetch_buffer) {
        case 0:
            PrefetchBuffer = "DISABLE";
            break;
        case 1:
            PrefetchBuffer = "ENABLE";
            break;
    }

    if(enc_props->tune_metrics) {
        ScalingList = "FLAT";
        QPCtrlMode  = "UNIFORM_QP";
    }

    if(enc_props->codec_id == ENCODER_ID_HEVC) {
        enc_options->length = sprintf(
            enc_props->enc_options,
            "[INPUT]\n"
            "Width = %d\n"
            "Height = %d\n"
            "Format = %s\n"
            "[RATE_CONTROL]\n"
            "RateCtrlMode = %s\n"
            "FrameRate = %s\n"
            "BitRate = %ld\n"
            "MaxBitRate = %ld\n"
            "SliceQP = %s\n"
            "MaxQP = %d\n"
            "MinQP = %d\n"
            "CPBSize = %f\n"
            "InitialDelay = %f\n"
            "[GOP]\n"
            "GopCtrlMode = %s\n"
            "Gop.GdrMode = %s\n"
            "Gop.Length = %d\n"
            "Gop.NumB = %d\n"
            "Gop.FreqIDR = %d\n"
            "[SETTINGS]\n"
            "Profile = %s\n"
            "Level = %s\n"
            "Tier = %s\n"
            "ChromaMode = CHROMA_4_2_0\n"
            "BitDepth = %d\n"
            "NumSlices = %d\n"
            "QPCtrlMode = %s\n"
            "SliceSize = %d\n"
            "DependentSlice = %s\n"
            "EnableFillerData = %s\n"
            "AspectRatio = %s\n"
            "ColourDescription = %s\n"
            "ScalingList = %s\n"
            "LoopFilter = %s\n"
            "ConstrainedIntraPred = %s\n"
            "LambdaCtrlMode = %s\n"
            "CacheLevel2 = %s\n"
            "NumCore = %d\n",
            enc_props->width, enc_props->height, format, RateCtrlMode,
            FrameRate, enc_props->bit_rate, enc_props->max_bitrate, SliceQP,
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, enc_props->gop_size,
            enc_props->num_bframes, enc_props->idr_period, Profile, Level, Tier,
            enc_props->bits_per_pixel, enc_props->num_slices, QPCtrlMode,
            enc_props->slice_size, DependentSlice, FillerData, AspectRatio,
            ColorSpace, ScalingList, LoopFilter, ConstIntraPred, LambdaCtrlMode,
            PrefetchBuffer, enc_props->num_cores);
    } else {
        enc_options->length = sprintf(
            enc_props->enc_options,
            "[INPUT]\n"
            "Width = %d\n"
            "Height = %d\n"
            "Format = %s\n"
            "[RATE_CONTROL]\n"
            "RateCtrlMode = %s\n"
            "FrameRate = %s\n"
            "BitRate = %ld\n"
            "MaxBitRate = %ld\n"
            "SliceQP = %s\n"
            "MaxQP = %d\n"
            "MinQP = %d\n"
            "CPBSize = %f\n"
            "InitialDelay = %f\n"
            "[GOP]\n"
            "GopCtrlMode = %s\n"
            "Gop.GdrMode = %s\n"
            "Gop.Length = %d\n"
            "Gop.NumB = %d\n"
            "Gop.FreqIDR = %d\n"
            "[SETTINGS]\n"
            "Profile = %s\n"
            "Level = %s\n"
            "ChromaMode = CHROMA_4_2_0\n"
            "BitDepth = %d\n"
            "NumSlices = %d\n"
            "QPCtrlMode = %s\n"
            "SliceSize = %d\n"
            "EnableFillerData = %s\n"
            "AspectRatio = %s\n"
            "ColourDescription = %s\n"
            "ScalingList = %s\n"
            "EntropyMode = %s\n"
            "LoopFilter = %s\n"
            "ConstrainedIntraPred = %s\n"
            "LambdaCtrlMode = %s\n"
            "CacheLevel2 = %s\n"
            "NumCore = %d\n",
            enc_props->width, enc_props->height, format, RateCtrlMode,
            FrameRate, enc_props->bit_rate, enc_props->max_bitrate, SliceQP,
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, enc_props->gop_size,
            enc_props->num_bframes, enc_props->idr_period, Profile, Level,
            enc_props->bits_per_pixel, enc_props->num_slices, QPCtrlMode,
            enc_props->slice_size, FillerData, AspectRatio, ColorSpace,
            ScalingList, EntropyMode, LoopFilter, ConstIntraPred,
            LambdaCtrlMode, PrefetchBuffer, enc_props->num_cores);
    }
    enc_options->value = &enc_props->enc_options;
    return XMA_SUCCESS;
}

/**
 * xlnx_enc_xma_params_update: Update encoder custom params
 *
 * @param enc_props: Encoder properties
 * @param xma_enc_props: XMA encoder properties
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_enc_fill_custom_xma_params(XlnxEncoderProperties* enc_props,
                                               XmaParameter* custom_xma_params)
{
    int32_t param_cnt = 0;
    if(xlnx_enc_create_xma_enc_options(
           enc_props, &custom_xma_params[param_cnt]) != XMA_SUCCESS) {
        return XMA_ERROR;
    }
    param_cnt++;

    custom_xma_params[param_cnt].name   = "latency_logging";
    custom_xma_params[param_cnt].type   = XMA_UINT32;
    custom_xma_params[param_cnt].length = sizeof(enc_props->latency_logging);
    custom_xma_params[param_cnt].value  = &(enc_props->latency_logging);
    param_cnt++;

    custom_xma_params[param_cnt].name   = "enable_hw_in_buf";
    custom_xma_params[param_cnt].type   = XMA_UINT32;
    custom_xma_params[param_cnt].length = sizeof(enc_props->enable_hw_buf);
    custom_xma_params[param_cnt].value  = &enc_props->enable_hw_buf;
    param_cnt++;
    return XMA_SUCCESS;
}

static int32_t
    xlnx_enc_create_basic_xma_props(XlnxEncoderProperties* enc_props,
                                    XmaEncoderProperties*  xma_enc_props)
{
    /* Initialize encoder properties */
    xma_enc_props->hwencoder_type = XMA_MULTI_ENCODER_TYPE;
    strcpy(xma_enc_props->hwvendor_string, "MPSoC");
    xma_enc_props->dev_index = enc_props->device_id;
    xma_enc_props->param_cnt = ENC_MAX_PARAMS;
    xma_enc_props->params = (XmaParameter*)calloc(1, xma_enc_props->param_cnt *
                                                         sizeof(XmaParameter));
    xma_enc_props->bits_per_pixel = enc_props->bits_per_pixel;
    xma_enc_props->format         = enc_props->bits_per_pixel == 10 ?
                                XMA_VCU_NV12_10LE32_FMT_TYPE :
                                XMA_VCU_NV12_FMT_TYPE;
    xma_enc_props->width                 = enc_props->width;
    xma_enc_props->height                = enc_props->height;
    xma_enc_props->rc_mode               = enc_props->custom_rc;
    xma_enc_props->lookahead_depth       = enc_props->lookahead_depth;
    xma_enc_props->framerate.numerator   = enc_props->fps;
    xma_enc_props->framerate.denominator = 1;
    return XMA_SUCCESS;
}

/**
 * xlnx_enc_update_props: Updates xma encoder properties and options that will
 * be sent to xma plugin
 *
 * @param enc_ctx: Encoder context
 * @param xma_enc_props: XMA encoder properties
 * @return XMA_SUCCESS or XMA_ERROR
 */
static int32_t xlnx_enc_update_props(XlnxEncoderProperties* enc_props)
{
    /* Enable custom rate control when rate control is set to CBR and
    lookahead is set, disable when expert option lookahead-rc-off is set. */
    if((enc_props->control_rate == 1) && (enc_props->lookahead_depth > 1)) {
        enc_props->custom_rc = 1;
    }

    /* Enable Adaptive Quantization by default, if lookahead is enabled */
    if(enc_props->lookahead_depth >= 1 && (enc_props->tune_metrics == 0)) {
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE,
                   "Setting qp mode to 2, as the lookahead params are set\n");
        enc_props->qp_mode = 2;
    } else if((enc_props->lookahead_depth == 0) ||
              (enc_props->tune_metrics == 1)) {
        if(enc_props->temporal_aq) {
            enc_props->temporal_aq = 0;
        }

        if(enc_props->spatial_aq) {
            enc_props->spatial_aq = 0;
        }
    }

    /* LA bypass condition check */
    if((enc_props->lookahead_depth == 0) && (enc_props->spatial_aq == 0)) {
        enc_props->enable_hw_buf = 0;
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
        xma_logmsg(XMA_INFO_LOG, XLNX_ENC_APP_MODULE,
                   "Setting IDR period to GOP size\n");
    }
    return XMA_SUCCESS;
}

/**
 * xlnx_enc_create_xma_props: Create the xma encoder properties using the given
 * xilinx encoder properties.
 *
 * @param enc_props: Encoder properties
 * @param xma_enc_props: XMA encoder properties to be created
 * @return XMA_SUCCESS or XMA_ERROR
 */
int32_t xlnx_enc_create_xma_props(XlnxEncoderProperties* enc_props,
                                  XmaEncoderProperties*  xma_enc_props)
{
    xlnx_enc_update_props(enc_props);
    xlnx_enc_create_basic_xma_props(enc_props, xma_enc_props);
    if(xlnx_enc_fill_custom_xma_params(enc_props, xma_enc_props->params) !=
       XMA_SUCCESS) {
        return XMA_ERROR;
    }
    return XMA_SUCCESS;
}

/**
 * xlnx_la_free_xma_props: Function to deinitialize XMA lookahead properties
 *
 * @param xma_la_props: XMA lookahead properties
 * @return None
 */
void xlnx_la_free_xma_props(XmaFilterProperties* xma_la_props)
{
    if(xma_la_props->params) {
        free(xma_la_props->params);
    }
    return;
}

/**
 * xlnx_la_xma_params_update:  Update lookahead custom params
 *
 * @param la_props: Lookahead properties
 * @param xma_la_props: XMA lookahead filter properties
 * @return None
 */
static void xlnx_la_fill_custom_xma_params(XlnxLookaheadProperties* la_props,
                                           XmaParameter* custom_params)
{
    uint32_t param_cnt = 0;
    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamIntraPeriod];
    custom_params[param_cnt].user_type = EParamIntraPeriod;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->gop_size;
    param_cnt++;

    custom_params[param_cnt].name = (char*)XLNX_LA_EXT_PARAMS[EParamLADepth];
    custom_params[param_cnt].user_type = EParamLADepth;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->lookahead_depth;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamEnableHwInBuf];
    custom_params[param_cnt].user_type = EParamEnableHwInBuf;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->enable_hw_buf;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamSpatialAQMode];
    custom_params[param_cnt].user_type = EParamSpatialAQMode;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->spatial_aq_mode;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamTemporalAQMode];
    custom_params[param_cnt].user_type = EParamTemporalAQMode;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->temporal_aq_mode;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamRateControlMode];
    custom_params[param_cnt].user_type = EParamRateControlMode;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->rate_control_mode;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamSpatialAQGain];
    custom_params[param_cnt].user_type = EParamSpatialAQGain;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->spatial_aq_gain;
    param_cnt++;

    custom_params[param_cnt].name = (char*)XLNX_LA_EXT_PARAMS[EParamNumBFrames];
    custom_params[param_cnt].user_type = EParamNumBFrames;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->num_bframes;
    param_cnt++;

    custom_params[param_cnt].name = (char*)XLNX_LA_EXT_PARAMS[EParamCodecType];
    custom_params[param_cnt].user_type = EParamCodecType;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->codec_type;
    param_cnt++;

    custom_params[param_cnt].name =
        (char*)XLNX_LA_EXT_PARAMS[EParamLatencyLogging];
    custom_params[param_cnt].user_type = EParamLatencyLogging;
    custom_params[param_cnt].type      = XMA_UINT32;
    custom_params[param_cnt].length    = sizeof(int);
    custom_params[param_cnt].value     = &la_props->latency_logging;
    param_cnt++;
    return;
}

/**
 * Uses the la props to set the xma filter input properties for the lookahead
 *
 * @param la_props The lookahead properties used to set the input filter
 * properties
 * @param in_props The input xma filter port properties to be set.
 *
 * @return XMA_SUCCESS
 */
static int32_t xlnx_la_create_xma_in_props(XlnxLookaheadProperties* la_props,
                                           XmaFilterPortProperties* in_props)
{
    /* Setup lookahead input port properties */
    memset(in_props, 0, sizeof(XmaFilterPortProperties));
    in_props->format         = la_props->xma_fmt_type;
    in_props->bits_per_pixel = la_props->bits_per_pixel;
    in_props->width          = la_props->width;
    in_props->height         = la_props->height;
    in_props->stride         = xlnx_utils_get_valid_bytes_in_line(
        la_props->bits_per_pixel, la_props->width);
    in_props->framerate.numerator   = la_props->framerate.numerator;
    in_props->framerate.denominator = la_props->framerate.denominator;
    return XMA_SUCCESS;
}

/**
 * Uses the la props to set the xma filter output properties for the lookahead
 *
 * @param la_props  The lookahead properties used to set the output filter
 * properties
 * @param out_props The output xma filter port properties to be set.
 *
 * @return XMA_SUCCESS
 */
static int32_t xlnx_la_create_xma_out_props(XlnxLookaheadProperties* la_props,
                                            XmaFilterPortProperties* out_props)
{
    memset(out_props, 0, sizeof(XmaFilterPortProperties));
    out_props->format         = la_props->xma_fmt_type;
    out_props->bits_per_pixel = la_props->bits_per_pixel;
    out_props->width =
        XLNX_ALIGN((la_props->width), XLNX_LOOKAHEAD_OUT_ALIGN) >>
        XLNX_SCLEVEL1;
    out_props->height =
        XLNX_ALIGN((la_props->height), XLNX_LOOKAHEAD_OUT_ALIGN) >>
        XLNX_SCLEVEL1;
    out_props->framerate.numerator   = la_props->framerate.numerator;
    out_props->framerate.denominator = la_props->framerate.denominator;
    return XMA_SUCCESS;
}

/**
 * xlnx_la_create_xma_props: Populate XmaFilterProperties struct from lookahead
 * properties
 *
 * @param la_props: Lookahead properties
 * @param xma_la_props: XMA lookahead filter properties
 * @return XMA_SUCCESS or XMA_ERROR
 */
int32_t xlnx_la_create_xma_props(XlnxLookaheadProperties* la_props,
                                 XmaFilterProperties*     xma_la_props)
{
    /* Setup lookahead properties */
    memset(xma_la_props, 0, sizeof(XmaFilterProperties));
    xma_la_props->hwfilter_type = XMA_2D_FILTER_TYPE;
    strcpy(xma_la_props->hwvendor_string, "Xilinx");
    xma_la_props->dev_index = la_props->device_id;
    xma_la_props->param_cnt = XLNX_LA_MAX_PARAMS;
    xma_la_props->params    = (XmaParameter*)calloc(1, xma_la_props->param_cnt *
                                                        sizeof(XmaParameter));
    xlnx_la_create_xma_in_props(la_props, &xma_la_props->input);
    xlnx_la_create_xma_out_props(la_props, &xma_la_props->output);
    xlnx_la_fill_custom_xma_params(la_props, xma_la_props->params);
    return XMA_SUCCESS;
}

/**
 * Create a host buffer xma frame using the encoder properties.
 * @param enc_props The encoder properties used to create the xma frame
 * @param xframe The frame to be created.
 * @return XMA_SUCCESS or XMA_ERROR
 */
int32_t xlnx_enc_create_xframe(const XlnxEncoderProperties* enc_props,
                               XmaFrame*                    xframe)
{
    XmaFrameProperties* fprops = &xframe->frame_props;
    fprops->bits_per_pixel     = enc_props->bits_per_pixel;
    fprops->format = xlnx_utils_get_xma_vcu_format(enc_props->bits_per_pixel);
    fprops->width  = enc_props->width;
    fprops->height = enc_props->height;
    fprops->linesize[0] = xlnx_utils_get_valid_bytes_in_line(
        enc_props->bits_per_pixel, enc_props->width);
    fprops->linesize[1] = fprops->linesize[0];
    size_t plane_size   = fprops->linesize[0] * fprops->height;
    for(int8_t plane_id = 0; plane_id < xma_frame_planes_get(fprops);
        plane_id++) {

        if(plane_id > 0) {
            plane_size = fprops->linesize[0] * fprops->height / 2;
        }
        xframe->data[plane_id].refcount    = 1;
        xframe->data[plane_id].buffer_type = XMA_HOST_BUFFER_TYPE;
        xframe->data[plane_id].is_clone    = false;
        if(posix_memalign(&xframe->data[plane_id].buffer, BUFFER_ALLOC_ALIGN,
                          plane_size) != 0) {
            xma_logmsg(XMA_ERROR_LOG, XLNX_ENC_APP_MODULE,
                       "Unable to allocate xma frame buffer for encoding!\n",
                       enc_props->profile);
        }
    }
    return XMA_SUCCESS;
}
