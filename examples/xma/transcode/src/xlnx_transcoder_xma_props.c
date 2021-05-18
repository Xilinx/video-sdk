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

#include "xlnx_transcoder_xma_props.h"

static const char *XLNX_LA_EXT_PARAMS[] = {
    "ip",
    "lookahead_depth",
    "enable_hw_in_buf",
    "spatial_aq_mode",
    "temporal_aq_mode",
    "rate_control_mode",
    "spatial_aq_gain",
    "num_b_frames",
    "codec_type",
    "latency_logging"
};

/*-----------------------------------------------------------------------------
xlnx_dec_xma_params_update: Update decoder custom params

Parameters:
dec_props: Decoder properties
xma_dec_props: XMA decoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static void xlnx_dec_xma_params_update(XlnxDecoderProperties *dec_props,
                                       XmaDecoderProperties *xma_dec_props)
{

    int32_t param_cnt = 0;
    xma_dec_props->param_cnt = MAX_DEC_PARAMS;
    xma_dec_props->params = 
       (XmaParameter *)malloc(xma_dec_props->param_cnt * sizeof(XmaParameter));

    xma_dec_props->params[param_cnt].name = "bitdepth";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(xma_dec_props->bits_per_pixel);
    xma_dec_props->params[param_cnt].value  = &(xma_dec_props->bits_per_pixel);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "codec_type";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->codec_type);
    xma_dec_props->params[param_cnt].value  = &(dec_props->codec_type);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "low_latency";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->low_latency);
    xma_dec_props->params[param_cnt].value  = &(dec_props->low_latency);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "entropy_buffers_count";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->entropy_buffers_count);
    xma_dec_props->params[param_cnt].value  = &(dec_props->entropy_buffers_count);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "zero_copy";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->zero_copy);
    xma_dec_props->params[param_cnt].value  = &(dec_props->zero_copy);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "profile";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->profile);
    xma_dec_props->params[param_cnt].value  = &(dec_props->profile);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "level";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->level);
    xma_dec_props->params[param_cnt].value  = &(dec_props->level);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "chroma_mode";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->chroma_mode);
    xma_dec_props->params[param_cnt].value  = &(dec_props->chroma_mode);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "scan_type";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->scan_type);
    xma_dec_props->params[param_cnt].value  = &(dec_props->scan_type);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "latency_logging";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->latency_logging);
    xma_dec_props->params[param_cnt].value  = &(dec_props->latency_logging);
    param_cnt++;

    xma_dec_props->params[param_cnt].name = "splitbuff_mode";
    xma_dec_props->params[param_cnt].type   = XMA_UINT32;
    xma_dec_props->params[param_cnt].length = sizeof(dec_props->splitbuff_mode);
    xma_dec_props->params[param_cnt].value  = &(dec_props->splitbuff_mode);
    param_cnt++;

    return;
}

/*-----------------------------------------------------------------------------
xlnx_scal_xma_params_update: Update scaler custom params

Parameters:
scal_props: Scaler properties
xma_scal_props: XMA scaler properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static void xlnx_scal_xma_params_update(XlnxScalerProperties *scal_props, 
                                        XmaScalerProperties *xma_scal_props)
{

    int32_t param_cnt = 0;
    xma_scal_props->param_cnt = SCAL_MAX_PARAMS;
    xma_scal_props->params = 
     (XmaParameter *)malloc(xma_scal_props->param_cnt * sizeof(XmaParameter));

    xma_scal_props->params[param_cnt].name  = "enable_pipeline";
    xma_scal_props->params[param_cnt].type = XMA_UINT32;
    xma_scal_props->params[param_cnt].length = sizeof(scal_props->enable_pipeline);
    xma_scal_props->params[param_cnt].value  = &(scal_props->enable_pipeline);
    param_cnt++;

    xma_scal_props->params[param_cnt].name  = "logLevel";
    xma_scal_props->params[param_cnt].type = XMA_UINT32;
    xma_scal_props->params[param_cnt].length = sizeof(scal_props->log_level);
    xma_scal_props->params[param_cnt].value  = &(scal_props->log_level);
    param_cnt++;

    xma_scal_props->params[param_cnt].name  = "MixRate";
    xma_scal_props->params[param_cnt].type = XMA_UINT64;
    xma_scal_props->params[param_cnt].length = 
                                      sizeof(scal_props->p_mixrate_session);
    xma_scal_props->params[param_cnt].value  = &(scal_props->p_mixrate_session);
    param_cnt++;

    xma_scal_props->params[param_cnt].name  = "latency_logging";
    xma_scal_props->params[param_cnt].type = XMA_UINT32;
    xma_scal_props->params[param_cnt].length = sizeof(scal_props->latency_logging);
    xma_scal_props->params[param_cnt].value  = &(scal_props->latency_logging);
    param_cnt++;

    return;
}

/*-----------------------------------------------------------------------------
xlnx_enc_xma_params_update: Update encoder custom params

Parameters:
enc_props: Encoder properties
xma_enc_props: XMA encoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
static int32_t xlnx_enc_xma_params_update(XlnxEncoderProperties *enc_props, 
                                          XmaEncoderProperties *xma_enc_props)
{
    int32_t param_cnt = 0;

    xma_enc_props->params[param_cnt].name   = "enc_options";
    xma_enc_props->params[param_cnt].type   = XMA_STRING;
    xma_enc_props->params[param_cnt].length = strlen(enc_props->enc_options);
    xma_enc_props->params[param_cnt].value  = &(enc_props->enc_options);
    param_cnt++;

    xma_enc_props->params[param_cnt].name   = "latency_logging";
    xma_enc_props->params[param_cnt].type   = XMA_UINT32;
    xma_enc_props->params[param_cnt].length = 
                                      sizeof(enc_props->latency_logging);
    xma_enc_props->params[param_cnt].value  = &(enc_props->latency_logging);
    param_cnt++;

    xma_enc_props->params[param_cnt].name = "enable_hw_in_buf";
    xma_enc_props->params[param_cnt].type = XMA_UINT32;
    xma_enc_props->params[param_cnt].length = 
                                      sizeof(enc_props->enable_hw_buf);
    xma_enc_props->params[param_cnt].value  = &enc_props->enable_hw_buf;
    param_cnt++;

    return TRANSCODE_APP_SUCCESS;

}

/*-----------------------------------------------------------------------------
xlnx_la_xma_params_update:  Update lookahead custom params

Parameters:
la_props: Lookahead properties
xma_la_props: XMA lookahead filter properties

Return:
None
-----------------------------------------------------------------------------*/
static void xlnx_la_xma_params_update(XlnxLookaheadProperties *la_props, 
                                      XmaFilterProperties *xma_la_props)
{

    XmaParameter *extn_params = NULL;
    uint32_t param_cnt = 0;
    extn_params = (XmaParameter *)calloc(1, 
                  XLNX_LA_MAX_NUM_EXT_PARAMS * sizeof(XmaParameter));

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamIntraPeriod];
    extn_params[param_cnt].user_type = EParamIntraPeriod;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->gop_size;
    param_cnt++;

    extn_params[param_cnt].name = (char *)XLNX_LA_EXT_PARAMS[EParamLADepth];
    extn_params[param_cnt].user_type = EParamLADepth;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->lookahead_depth;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamEnableHwInBuf];
    extn_params[param_cnt].user_type = EParamEnableHwInBuf;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->enable_hw_buf;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamSpatialAQMode];
    extn_params[param_cnt].user_type = EParamSpatialAQMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->spatial_aq_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamTemporalAQMode];
    extn_params[param_cnt].user_type = EParamTemporalAQMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->temporal_aq_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamRateControlMode];
    extn_params[param_cnt].user_type = EParamRateControlMode;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->rate_control_mode;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamSpatialAQGain];
    extn_params[param_cnt].user_type = EParamSpatialAQGain;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->spatial_aq_gain;
    param_cnt++;

    extn_params[param_cnt].name = 
                            (char *)XLNX_LA_EXT_PARAMS[EParamNumBFrames];
    extn_params[param_cnt].user_type = EParamNumBFrames;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->num_bframes;
    param_cnt++;

    extn_params[param_cnt].name = (char *)XLNX_LA_EXT_PARAMS[EParamCodecType];
    extn_params[param_cnt].user_type = EParamCodecType;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->codec_type;
    param_cnt++;

    extn_params[param_cnt].name = 
        (char *)XLNX_LA_EXT_PARAMS[EParamLatencyLogging];
    extn_params[param_cnt].user_type = EParamLatencyLogging;
    extn_params[param_cnt].type = XMA_UINT32;
    extn_params[param_cnt].length = sizeof(int);
    extn_params[param_cnt].value = &la_props->latency_logging;
    param_cnt++;

    xma_la_props->param_cnt = param_cnt;
    xma_la_props->params = &extn_params[0];

    return;
}

/*-----------------------------------------------------------------------------
xlnx_dec_get_xma_props:  Populate XmaDecoderProperties struct from decoder 
                    properties

Parameters:
dec_props: decoder properties
xma_dec_props: XMA decoder properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_dec_get_xma_props(XlnxDecoderProperties *dec_props,
                              XmaDecoderProperties *xma_dec_props)
{

    strcpy(xma_dec_props->hwvendor_string, "MPSoC");

    xma_dec_props->hwdecoder_type  = XMA_MULTI_DECODER_TYPE;
    xma_dec_props->width = dec_props->width;
    xma_dec_props->height = dec_props->height;
    xma_dec_props->framerate.numerator = dec_props->fps;
    xma_dec_props->framerate.denominator = 1;
    xma_dec_props->bits_per_pixel = dec_props->bit_depth;

    xlnx_dec_xma_params_update(dec_props, xma_dec_props);
    return;
}

/*-----------------------------------------------------------------------------
xlnx_scal_get_xma_props:  Populate XmaScalerProperties struct from scaler properties

Parameters:
scal_props: scaler properties
xma_scal_props: XMA scaler properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_scal_get_xma_props(XlnxScalerProperties *scal_props, 
                             XmaScalerProperties *xma_scal_props)
{

    xma_scal_props->hwscaler_type = XMA_POLYPHASE_SCALER_TYPE;

    strcpy(xma_scal_props->hwvendor_string, "Xilinx");
    xma_scal_props->num_outputs   = scal_props->nb_outputs;

    xma_scal_props->input.format = XMA_VCU_NV12_FMT_TYPE;
    xma_scal_props->input.width  = scal_props->in_width;
    xma_scal_props->input.height = scal_props->in_height;
    xma_scal_props->input.stride = ALIGN (scal_props->in_width, 
            TRANSCODE_WIDTH_ALIGN);
    xma_scal_props->input.framerate.numerator = scal_props->fr_num;
    xma_scal_props->input.framerate.denominator = scal_props->fr_den;

    for(int32_t i = 0; i < scal_props->nb_outputs; i++) {

        xma_scal_props->output[i].format = XMA_VCU_NV12_FMT_TYPE;
        xma_scal_props->output[i].bits_per_pixel = 8;
        xma_scal_props->output[i].width  = scal_props->out_width[i];
        xma_scal_props->output[i].height = scal_props->out_height[i];
        xma_scal_props->output[i].stride = ALIGN (scal_props->out_width[i], 
                TRANSCODE_WIDTH_ALIGN);
        xma_scal_props->output[i].coeffLoad = 0;
        xma_scal_props->output[i].framerate.numerator = scal_props->fr_num;
        xma_scal_props->output[i].framerate.denominator = scal_props->fr_den;
    }

    xlnx_scal_xma_params_update(scal_props, xma_scal_props);
    return;
}

/*-----------------------------------------------------------------------------
xlnx_enc_get_xma_props: Populate XmaEncoderProperties struct from encoder 
                        properties

Parameters:
enc_props: Encoder properties
xma_enc_props: XMA encoder properties

Return:
TRANSCODE_APP_SUCCESS or TRANSCODE_APP_FAILURE
-----------------------------------------------------------------------------*/
int32_t xlnx_enc_get_xma_props(XlnxEncoderProperties *enc_props, 
                               XmaEncoderProperties *xma_enc_props)
{

    /* Initialize encoder properties */
    xma_enc_props->hwencoder_type = XMA_MULTI_ENCODER_TYPE;
    strcpy(xma_enc_props->hwvendor_string, "MPSoC");
    xma_enc_props->param_cnt = ENC_MAX_EXT_PARAMS;
    xma_enc_props->params = (XmaParameter *)calloc(1, 
                            xma_enc_props->param_cnt * sizeof(XmaParameter));

    xma_enc_props->format = XMA_VCU_NV12_FMT_TYPE;
    xma_enc_props->bits_per_pixel  = 8;
    xma_enc_props->width = enc_props->width;
    xma_enc_props->height = enc_props->height;
    xma_enc_props->rc_mode =  enc_props->custom_rc;

    switch(xma_enc_props->rc_mode) {
        case 0 : 
            break;

        case 1 : 
            if (enc_props->lookahead_depth < ENC_MIN_LOOKAHEAD_DEPTH ||
                    enc_props->lookahead_depth > ENC_MAX_LOOKAHEAD_DEPTH) {
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE,
                        "Error: Provided LA Depth %d is invalid !\n", 
                        enc_props->lookahead_depth);
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "If RC mode is 1, the LA depth must lie between %d - %d\n",
                    ENC_MIN_LOOKAHEAD_DEPTH, ENC_MAX_LOOKAHEAD_DEPTH);
                return TRANSCODE_APP_FAILURE;
            } else {
                xma_enc_props->lookahead_depth = enc_props->lookahead_depth;
            }
            xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Encoder custom RC mode is enabled with LA depth = %d \n", 
                    xma_enc_props->lookahead_depth);
            break;

        default: 
            xma_enc_props->rc_mode = 0;
            xma_logmsg(XMA_INFO_LOG, XLNX_TRANSCODER_APP_MODULE, 
                    "Rate control mode is default\n");
            break;
    }

    xma_enc_props->framerate.numerator   = enc_props->fps;
    xma_enc_props->framerate.denominator = 1;

    /* Update encoder options */
    const char* RateCtrlMode = "CONST_QP";
    switch (enc_props->control_rate) {
        case 0: RateCtrlMode = "CONST_QP"; break;
        case 1: RateCtrlMode = "CBR"; break;
        case 2: RateCtrlMode = "VBR"; break;
        case 3: RateCtrlMode = "LOW_LATENCY"; break;
    }

    char FrameRate[16];
    int32_t fps_den = 1;
    sprintf(FrameRate, "%d/%d", enc_props->fps, fps_den);

    char SliceQP[8];
    if (enc_props->slice_qp == -1)
        strcpy (SliceQP, "AUTO");
    else
        sprintf(SliceQP, "%d", enc_props->slice_qp);

    const char* GopCtrlMode = "DEFAULT_GOP";
    switch (enc_props->gop_mode) {
        case 0: GopCtrlMode = "DEFAULT_GOP"; break;
        case 1: GopCtrlMode = "PYRAMIDAL_GOP"; break;
        case 2: GopCtrlMode = "LOW_DELAY_P"; break;
        case 3: GopCtrlMode = "LOW_DELAY_B"; break;
    }

    const char* GDRMode = "DISABLE";
    switch (enc_props->gdr_mode) {
        case 0: GDRMode = "DISABLE"; break;
        case 1: GDRMode = "GDR_VERTICAL"; break;
        case 2: GDRMode = "GDR_HORIZONTAL"; break;
    }

    const char* Profile = "AVC_BASELINE";
    if(enc_props->codec_id == ENCODER_ID_H264) {
        switch (enc_props->profile) {
            case ENC_H264_BASELINE: Profile = "AVC_BASELINE"; break;
            case ENC_H264_MAIN: Profile = "AVC_MAIN"; break;
            case ENC_H264_HIGH: Profile = "AVC_HIGH"; break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                        "Invalid H264 codec profile value %d \n", 
                        enc_props->profile);
                return TRANSCODE_APP_FAILURE;
        }
    } else if(enc_props->codec_id == ENCODER_ID_HEVC){
        Profile = "HEVC_MAIN";
        switch (enc_props->profile) {
            case ENC_HEVC_MAIN: Profile = "HEVC_MAIN"; break;
            case ENC_HEVC_MAIN_INTRA: Profile = "HEVC_MAIN_INTRA"; break;
            default:
                xma_logmsg(XMA_ERROR_LOG, XLNX_TRANSCODER_APP_MODULE, 
                        "Invalid HEVC codec profile value %d \n", 
                        enc_props->profile);
                return TRANSCODE_APP_FAILURE;
        }
    }

    const char* Level = "1";
    switch (enc_props->level) {
        case 10: Level = "1"; break;
        case 11: Level = "1.1"; break;
        case 12: Level = "1.2"; break;
        case 13: Level = "1.3"; break;
        case 20: Level = "2"; break;
        case 21: Level = "2.1"; break;
        case 22: Level = "2.2"; break;
        case 30: Level = "3"; break;
        case 31: Level = "3.1"; break;
        case 32: Level = "3.2"; break;
        case 40: Level = "4"; break;
        case 41: Level = "4.1"; break;
        case 42: Level = "4.2"; break;
        case 50: Level = "5"; break;
        case 51: Level = "5.1"; break;
        case 52: Level = "5.2"; break;
    }

    const char* Tier = "MAIN_TIER";
    switch (enc_props->tier) {
        case 0: Tier = "MAIN_TIER"; break;
        case 1: Tier = "HIGH_TIER"; break;
    }

    const char* QPCtrlMode = "UNIFORM_QP";
    switch (enc_props->qp_mode) {
        case 0: QPCtrlMode = "UNIFORM_QP"; break;
        case 1: QPCtrlMode = "AUTO_QP"; break;
        case 2: QPCtrlMode = "LOAD_QP | RELATIVE_QP"; break;
    }

    const char* DependentSlice = "FALSE";
    switch (enc_props->dependent_slice) {
        case 0: DependentSlice = "FALSE"; break;
        case 1: DependentSlice = "TRUE"; break;
    }

    const char* FillerData = "ENABLE";
    switch (enc_props->filler_data) {
        case 0: FillerData = "DISABLE"; break;
        case 1: FillerData = "ENABLE"; break;
    }

    const char* AspectRatio = "ASPECT_RATIO_AUTO";
    switch (enc_props->aspect_ratio) {
        case 0: AspectRatio = "ASPECT_RATIO_AUTO"; break;
        case 1: AspectRatio = "ASPECT_RATIO_4_3"; break;
        case 2: AspectRatio = "ASPECT_RATIO_16_9"; break;
        case 3: AspectRatio = "ASPECT_RATIO_NONE"; break;
    }

    const char* ColorSpace = "COLOUR_DESC_UNSPECIFIED";

    const char* ScalingList = "FLAT";
    switch (enc_props->scaling_list) {
        case 0: ScalingList = "FLAT"; break;
        case 1: ScalingList = "DEFAULT"; break;
    }

    const char* LoopFilter = "ENABLE";
    switch (enc_props->loop_filter) {
        case 0: LoopFilter = "DISABLE"; break;
        case 1: LoopFilter = "ENABLE"; break;
    }

    const char* EntropyMode = "MODE_CABAC";
    switch (enc_props->entropy_mode) {
        case 0: EntropyMode = "MODE_CAVLC"; break;
        case 1: EntropyMode = "MODE_CABAC"; break;
    }

    const char* ConstIntraPred = "ENABLE";
    switch (enc_props->constrained_intra_pred) {
        case 0: ConstIntraPred = "DISABLE"; break;
        case 1: ConstIntraPred = "ENABLE"; break;
    }

    const char* LambdaCtrlMode = "DEFAULT_LDA";

    const char* PrefetchBuffer = "DISABLE";
    switch (enc_props->prefetch_buffer) {
        case 0: PrefetchBuffer = "DISABLE"; break;
        case 1: PrefetchBuffer = "ENABLE"; break;
    }

    if (enc_props->tune_metrics){
        ScalingList = "FLAT";
        QPCtrlMode = "UNIFORM_QP";
    }

    if(enc_props->codec_id == ENCODER_ID_HEVC) {
        sprintf (enc_props->enc_options, "[INPUT]\n"
                "Width = %d\n"
                "Height = %d\n"
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
                "BitDepth = 8\n"
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
            enc_props->width, enc_props->height, RateCtrlMode, FrameRate, 
            enc_props->bit_rate, enc_props->max_bitrate, SliceQP, 
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, 
            enc_props->gop_size, enc_props->num_bframes, 
            enc_props->idr_period, Profile, Level, Tier, 
            enc_props->num_slices, QPCtrlMode, enc_props->slice_size, 
            DependentSlice, FillerData, AspectRatio, ColorSpace, 
            ScalingList, LoopFilter, ConstIntraPred, LambdaCtrlMode, 
            PrefetchBuffer, enc_props->num_cores);
    }
    else {
        sprintf (enc_props->enc_options, "[INPUT]\n"
                "Width = %d\n"
                "Height = %d\n"
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
                "BitDepth = 8\n"
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
            enc_props->width, enc_props->height, RateCtrlMode, FrameRate, 
            enc_props->bit_rate, enc_props->max_bitrate, SliceQP, 
            enc_props->max_qp, enc_props->min_qp, enc_props->cpb_size,
            enc_props->initial_delay, GopCtrlMode, GDRMode, 
            enc_props->gop_size, enc_props->num_bframes, 
            enc_props->idr_period, Profile, Level, enc_props->num_slices,
            QPCtrlMode, enc_props->slice_size, FillerData, AspectRatio, 
            ColorSpace, ScalingList, EntropyMode, LoopFilter, 
            ConstIntraPred, LambdaCtrlMode, PrefetchBuffer, 
            enc_props->num_cores);
    }

    xlnx_enc_xma_params_update(enc_props, xma_enc_props);

    return TRANSCODE_APP_SUCCESS;
}

/*-----------------------------------------------------------------------------
xlnx_la_get_xma_props: Populate XmaFilterProperties struct from lookahead 
                       properties

Parameters:
la_props: Lookahead properties
xma_la_props: XMA lookahead filter properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_la_get_xma_props(XlnxLookaheadProperties *la_props, 
                           XmaFilterProperties *xma_la_props)
{

    XmaFilterPortProperties *in_props;
    XmaFilterPortProperties *out_props;

    /* Setup lookahead properties */
    memset(xma_la_props, 0, sizeof(XmaFilterProperties));
    xma_la_props->hwfilter_type = XMA_2D_FILTER_TYPE;
    strcpy(xma_la_props->hwvendor_string, "Xilinx");

    /* Setup lookahead input port properties */
    in_props = &xma_la_props->input;
    memset(in_props, 0, sizeof(XmaFilterPortProperties));
    in_props->format = la_props->xma_fmt_type;
    in_props->bits_per_pixel = la_props->bits_per_pixel;
    in_props->width = la_props->width;
    in_props->height = la_props->height;
    in_props->stride = la_props->stride;
    in_props->framerate.numerator = la_props->framerate.numerator;
    in_props->framerate.denominator = la_props->framerate.denominator;

    /* Setup lookahead output port properties */
    out_props = &xma_la_props->output;
    memset(out_props, 0, sizeof(XmaFilterPortProperties));
    out_props->format = la_props->xma_fmt_type;
    out_props->bits_per_pixel = la_props->bits_per_pixel;
    out_props->width = 
                   XLNX_ENC_LINE_ALIGN((in_props->width), 64) >> XLNX_SCLEVEL1;
    out_props->height = 
                  XLNX_ENC_LINE_ALIGN((in_props->height), 64) >> XLNX_SCLEVEL1;
    out_props->framerate.numerator = la_props->framerate.numerator;
    out_props->framerate.denominator = la_props->framerate.denominator;

    xlnx_la_xma_params_update(la_props, xma_la_props);
    return;
}

/*-----------------------------------------------------------------------------
xlnx_dec_free_xma_props: Function to deinitialize XMA decoder properties

Parameters:
xma_dec_props: XMA decoder properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_dec_free_xma_props(XmaDecoderProperties *xma_dec_props)
{
    if(xma_dec_props->params)
        free(xma_dec_props->params);

    return;
}

/*-----------------------------------------------------------------------------
xlnx_scal_free_xma_props: Function to deinitialize XMA scaler properties

Parameters:
xma_scal_props: XMA scaler properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_scal_free_xma_props(XmaScalerProperties *xma_scal_props)
{
    if(xma_scal_props->params)
        free(xma_scal_props->params);

    return;
}

/*-----------------------------------------------------------------------------
xlnx_enc_free_xma_props: Function to deinitialize XMA encoder properties

Parameters:
xma_enc_props: XMA encoder properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_enc_free_xma_props(XmaEncoderProperties *xma_enc_props)
{
    if(xma_enc_props->params)
        free(xma_enc_props->params);

    return;
}

/*-----------------------------------------------------------------------------
xlnx_la_free_xma_props: Function to deinitialize XMA lookahead properties

Parameters:
xma_la_props: XMA lookahead properties

Return:
None
-----------------------------------------------------------------------------*/
void xlnx_la_free_xma_props(XmaFilterProperties *xma_la_props)
{
    if(xma_la_props->params)
        free(xma_la_props->params);

    return;
}