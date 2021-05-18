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

#ifndef _XLNX_TRANSCODER_CONSTANTS_H_
#define _XLNX_TRANSCODER_CONSTANTS_H_

#include <stdio.h>

#define XLNX_TRANSCODER_APP_MODULE  "xlnx_transcoder_app"

#define TRANSCODE_APP_SUCCESS     0
#define TRANSCODE_APP_STOP        1
#define TRANSCODE_PARSING_DONE    2

#define TRANSCODE_APP_FAILURE     (-1)

/* Scaler constants */
#define SCAL_MAX_ABR_CHANNELS    8
#define SCAL_MAX_PARAMS          4
#define SCAL_RATE_STRING_LEN     8
#define SCAL_MAX_INPUT_WIDTH     3840
#define SCAL_MAX_INPUT_HEIGHT    2160
#define SCAL_MIN_INPUT_WIDTH     64
#define SCAL_MIN_INPUT_HEIGHT    64

/* Transcoder constants */
#define TRANSCODE_MAX_ABR_CHANNELS        (SCAL_MAX_ABR_CHANNELS + 1)
#define MAX_ARG_SIZE                      64
#define TRANSCODE_WIDTH_ALIGN             256
#define TRANSCODE_HEIGHT_ALIGN            64

#define TRANSCODE_DEC_FLUSH_MODE          1
#define TRANSCODE_SCAL_FLUSH_MODE         2
#define TRANSCODE_LA_FLUSH_MODE           3
#define TRANSCODE_ENC_FLUSH_MODE          4

/* Encoder constants */
#define ENC_MIN_LOOKAHEAD_DEPTH    0
#define ENC_MAX_LOOKAHEAD_DEPTH    20
#define ENC_SUPPORTED_MIN_WIDTH    64
#define ENC_SUPPORTED_MAX_WIDTH    3840
#define ENC_MAX_LA_INPUT_WIDTH     1920
#define ENC_DEFAULT_WIDTH          1920
#define ENC_SUPPORTED_MIN_HEIGHT   64
#define ENC_SUPPORTED_MAX_HEIGHT   2160
#define ENC_MAX_LA_INPUT_HEIGHT    1080
#define ENC_DEFAULT_HEIGHT         1080
#define ENC_DEFAULT_BITRATE        5000
#define ENC_DEFAULT_MAX_BITRATE    (ENC_DEFAULT_BITRATE)
#define ENC_SUPPORTED_MAX_BITRATE  35000000
#define ENC_DEFAULT_FRAMERATE      25
#define ENC_DEFAULT_GOP_SIZE       120
#define ENC_SUPPORTED_MAX_QP       51
#define ENC_SUPPORTED_MIN_QP       0
#define ENC_DEFAULT_LEVEL          10
#define ENC_DEFAULT_SPAT_AQ_GAIN   50
#define ENC_MAX_SPAT_AQ_GAIN       100
#define ENC_DEFAULT_NUM_B_FRAMES   2
#define ENC_MAX_OPTIONS_SIZE       2048
#define ENC_MAX_EXT_PARAMS         3

/* H264 Encoder supported profiles */
#define ENC_H264_BASELINE   66
#define ENC_H264_MAIN       77
#define ENC_H264_HIGH       100

#define ENC_SUPPORTED_MAX_PIXELS   ((ENC_SUPPORTED_MAX_WIDTH) * (ENC_SUPPORTED_MAX_HEIGHT))

#define ENC_MAX_LA_PIXELS          ((ENC_MAX_LA_INPUT_WIDTH) * (ENC_MAX_LA_INPUT_HEIGHT))

#define XLNX_ENC_LINE_ALIGN(x,LINE_SIZE) (((((size_t)x) + \
                    ((size_t)LINE_SIZE - 1)) & (~((size_t)LINE_SIZE - 1))))


/* Decoder constants */
#define MAX_DEC_PARAMS    11
#define DEC_INPUT_EOF     1
#define DEC_MAX_OUT_BUFFERS    20

/* Lookahead constants */
#define XLNX_SCLEVEL1 2
#define XLNX_LA_MAX_NUM_EXT_PARAMS 10
#define XLNX_MAX_LOOKAHEAD_DEPTH 20

typedef enum
{
    ENCODER_ID_H264 = 0,
    ENCODER_ID_HEVC
}XlnxEncoderCodecID;

typedef enum
{
    LOOKAHEAD_ID_H264 = 0,
    LOOKAHEAD_ID_HEVC
}XlnxLookaheadCodecID;

typedef enum
{
    DECODER_ID_H264 = 0,
    DECODER_ID_HEVC
}XlnxDecoderCodecID;

/* Transcoder states */
typedef enum {

    TRANSCODE_DEC_READ_FRAME = 0,
    TRANSCODE_DEC_SEND_INPUT,
    TRANSCODE_DEC_SEND_LAST_FRAME,
    TRANSCODE_DEC_GET_OUTPUT,
    TRANSCODE_DEC_FLUSH,
    TRANSCODE_SCAL_PROCESS_FRAME,
    TRANSCODE_SCAL_FLUSH,
    TRANSCODE_LA_PROCESS,
    TRANSCODE_LA_FLUSH,
    TRANSCODE_ENC_PROCESS_FRAME,
    TRANSCODE_ENC_NULL_FRAME,
    TRANSCODE_ENC_FLUSH,
    TRANSCODE_EOF,
    TRANSCODE_STOP,
    TRANSCODE_DONE
}XlnxTranscodeStates;

/* Enum for transcoder command line arguments */
typedef enum
{
    HELP_ARG = 0,
    TRANSCODE_DEVICE_ID_ARG,
    TRANSCODE_STREAM_LOOP_ARG,
    TRANSCODE_NUM_FRAMES_ARG,
    TRANSCODE_GENERIC_MAX_ARG
}XlnxTranscodeArgIdentifiers;

/* Enum for decoder command line arguments */
typedef enum
{
    DEC_CODEC_ID_ARG = TRANSCODE_GENERIC_MAX_ARG,
    DEC_INPUT_FILE_ARG,
    DEC_LOW_LATENCY_ARG,
    DEC_LATENCY_LOGGING_ARG,
    DEC_MAX_ARG
}XlnxDecArgIdentifiers;

/* Enum for scaler command line arguments */
typedef enum
{
    SCAL_FILTER_ARG = DEC_MAX_ARG,
    SCAL_ENABLE_PIPELINE_ARG,
    SCAL_NUM_OUTPUTS_ARG,
    SCAL_OUTPUT_WIDTH_ARG,
    SCAL_OUTPUT_HEIGHT_ARG,
    SCAL_OUTPUT_RATE_ARG,
    SCAL_LATENCY_LOGGING_ARG,
    SCAL_MAX_ARG
}XlnxScalerArgIdentifiers;

/* Enum for encoder command line arguments */
typedef enum
{
    ENC_CODEC_ID_ARG = SCAL_MAX_ARG,
    ENC_BITRATE_ARG,
    ENC_FPS_ARG,
    ENC_INTRA_PERIOD_ARG,
    ENC_CONTROL_RATE_ARG,
    ENC_MAX_BITRATE_ARG,
    ENC_SLICE_QP_ARG,
    ENC_MIN_QP_ARG,
    ENC_MAX_QP_ARG,
    ENC_NUM_BFRAMES_ARG,
    ENC_IDR_PERIOD_ARG,
    ENC_PROFILE_ARG,
    ENC_LEVEL_ARG,
    ENC_NUM_SLICES_ARG,
    ENC_QP_MODE_ARG,
    ENC_ASPECT_RATIO_ARG,
    ENC_SCALING_LIST_ARG,
    ENC_LOOKAHEAD_DEPTH_ARG,
    ENC_TEMPORAL_AQ_ARG,
    ENC_SPATIAL_AQ_ARG,
    ENC_SPATIAL_AQ_GAIN_ARG,
    ENC_QP_ARG,
    ENC_NUM_CORES_ARG,
    ENC_TUNE_METRICS_ARG,
    ENC_LATENCY_LOGGING_ARG,
    ENC_OUTPUT_FILE_ARG
}XlnxEncArgIdentifiers;

/* Enum for lookahead arguments */
typedef enum
{
    EParamIntraPeriod = 0,
    EParamLADepth,
    EParamEnableHwInBuf,
    EParamSpatialAQMode,
    EParamTemporalAQMode,
    EParamRateControlMode,
    EParamSpatialAQGain,
    EParamNumBFrames,
    EParamCodecType,
    EParamLatencyLogging
}XlnxLAExtParams;

/* HEVC Encoder supported profiles */
typedef enum
{
    ENC_HEVC_MAIN = 0,
    ENC_HEVC_MAIN_INTRA
}XlnxHevcProfiles;

#endif //_XLNX_TRANSCODER_CONSTANTS_H_
