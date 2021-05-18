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

#ifndef _XLNX_ENC_CONSTANTS_H_
#define _XLNX_ENC_CONSTANTS_H_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <xma.h>
#include <xrm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <termios.h>

#define ENC_APP_SUCCESS            0
#define ENC_APP_FAILURE            (-1)
#define ENC_APP_DONE               1
#define ENC_APP_STOP               2

#define ENC_DEFAULT_NUM_B_FRAMES   2
#define ENC_DEFAULT_LEVEL          10
#define ENC_DEFAULT_FRAMERATE      25
#define ENC_DEFAULT_SPAT_AQ_GAIN   50
#define ENC_MAX_SPAT_AQ_GAIN       100
#define ENC_DEFAULT_GOP_SIZE       120

#define MAX_ARG_SIZE               64
#define VCU_HEIGHT_ALIGN           64
#define VCU_STRIDE_ALIGN           256

#define ENC_MIN_LOOKAHEAD_DEPTH    0
#define ENC_MAX_LOOKAHEAD_DEPTH    20

#define ENC_SUPPORTED_MIN_WIDTH    64
#define ENC_DEFAULT_WIDTH          1920
#define ENC_SUPPORTED_MAX_WIDTH    3840
#define ENC_MAX_LA_INPUT_WIDTH     1920

#define ENC_SUPPORTED_MIN_HEIGHT   64
#define ENC_DEFAULT_HEIGHT         1080
#define ENC_SUPPORTED_MAX_HEIGHT   2160
#define ENC_MAX_LA_INPUT_HEIGHT    1080

#define XLNX_ENC_LINE_ALIGN(x,LINE_SIZE) (((((size_t)x) + \
                    ((size_t)LINE_SIZE - 1)) & (~((size_t)LINE_SIZE - 1))))

#define ENC_SUPPORTED_MAX_PIXELS   ((ENC_SUPPORTED_MAX_WIDTH) * (ENC_SUPPORTED_MAX_HEIGHT))

#define ENC_MAX_LA_PIXELS          ((ENC_MAX_LA_INPUT_WIDTH) * (ENC_MAX_LA_INPUT_HEIGHT))

#define ENC_DEFAULT_BITRATE        5000
#define ENC_DEFAULT_MAX_BITRATE    (ENC_DEFAULT_BITRATE)
#define ENC_SUPPORTED_MAX_BITRATE  35000000

#define ENC_SUPPORTED_MIN_QP       0
#define ENC_SUPPORTED_MAX_QP       51

#define ENC_OPTION_DISABLE         0
#define ENC_OPTION_ENABLE          1

#define ENC_RC_CONST_QP_MODE       0
#define ENC_RC_CBR_MODE            1
#define ENC_RC_VBR_MODE            2
#define ENC_RC_LOW_LATENCY_MODE    3

#define ENC_DEFAULT_GOP_MODE       0
#define ENC_PYRAMIDAL_GOP_MODE     1
#define ENC_LOW_DELAY_P_MODE       2
#define ENC_LOW_DELAY_B_MODE       3

#define ENC_GDR_DISABLE            0
#define ENC_GDR_VERTICAL_MODE      1
#define ENC_GDR_HORIZONTAL_MODE    2

#define ENC_LEVEL_10               10
#define ENC_LEVEL_11               11
#define ENC_LEVEL_12               12
#define ENC_LEVEL_13               13
#define ENC_LEVEL_20               20
#define ENC_LEVEL_21               21
#define ENC_LEVEL_22               22
#define ENC_LEVEL_30               30
#define ENC_LEVEL_31               31
#define ENC_LEVEL_32               32
#define ENC_LEVEL_40               40
#define ENC_LEVEL_41               41
#define ENC_LEVEL_42               42
#define ENC_LEVEL_50               50
#define ENC_LEVEL_51               51
#define ENC_LEVEL_52               52

#define ENC_UNIFORM_QP_MODE        0
#define ENC_AUTO_QP_MODE           1
#define ENC_RELATIVE_LOAD_QP_MODE  2

#define ENC_ASPECT_RATIO_AUTO      0
#define ENC_ASPECT_RATIO_4_3       1
#define ENC_ASPECT_RATIO_16_9      2
#define ENC_ASPECT_RATIO_NONE      3

#define ENC_CAVLC_MODE             0
#define ENC_CABAC_MODE             1

#define ENC_MAX_OPTIONS_SIZE       2048
#define ENC_MAX_EXT_PARAMS         3

/* H264 Encoder supported profiles */
#define ENC_H264_BASELINE          66
#define ENC_H264_MAIN              77
#define ENC_H264_HIGH              100

#define XLNX_ENC_APP_MODULE        "xlnx_encoder_app"

/* Lookahead constants */
#define XLNX_SCLEVEL1              2
#define XLNX_LA_MAX_NUM_EXT_PARAMS 10
#define XLNX_MAX_LOOKAHEAD_DEPTH   20


/* HEVC Encoder supported profiles */
typedef enum
{
    ENC_HEVC_MAIN = 0,
    ENC_HEVC_MAIN_INTRA
} XlnxHevcProfiles;

typedef enum
{
    ENCODER_ID_H264 = 0,
    ENCODER_ID_HEVC
} XlnxEncoderCodecID;

typedef enum
{
    LOOKAHEAD_ID_H264 = 0,
    LOOKAHEAD_ID_HEVC
} XlnxLookaheadCodecID;

typedef enum
{
    YUV_NV12_ID = 0,
    YUV_420P_ID

} XlnxInputPixelFormat;

typedef enum
{
    HELP_ARG = 0,
    DEVICE_ID_ARG,
    LOOP_COUNT_ARG,
    INPUT_FILE_ARG,
    ENCODER_ARG,
    INPUT_WIDTH_ARG,
    INPUT_HEIGHT_ARG,
    INPUT_PIX_FMT_ARG,
    BITRATE_ARG,
    FPS_ARG,
    INTRA_PERIOD_ARG,
    CONTROL_RATE_ARG,
    MAX_BITRATE_ARG,
    SLICE_QP_ARG,
    MIN_QP_ARG,
    MAX_QP_ARG,
    NUM_BFRAMES_ARG,
    IDR_PERIOD_ARG,
    PROFILE_ARG,
    LEVEL_ARG,
    NUM_SLICES_ARG,
    QP_MODE_ARG,
    ASPECT_RATIO_ARG,
    SCALING_LIST_ARG,
    LOOKAHEAD_DEPTH_ARG,
    TEMPORAL_AQ_ARG,
    SPATIAL_AQ_ARG,
    SPATIAL_AQ_GAIN_ARG,
    QP_ARG,
    NUM_FRAMES_ARG,
    LATENCY_LOGGING_ARG,
    NUM_CORES_ARG,
    TUNE_METRICS_ARG,
    OUTPUT_FILE_ARG
} XlnxEncArgIdentifiers;

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
} XlnxLAExtParams;

typedef enum {

    ENC_READ_INPUT = 0,
    ENC_LA_PROCESS,
    ENC_LA_FLUSH,
    ENC_SEND_INPUT,
    ENC_GET_OUTPUT,
    ENC_FLUSH,
    ENC_EOF,
    ENC_STOP,
    ENC_DONE
} XlnxEncoderState;

typedef struct
{
    char *key;
    int value;
} XlnxEncProfileLookup;


#endif // _XLNX_ENC_CONSTANTS_H_
