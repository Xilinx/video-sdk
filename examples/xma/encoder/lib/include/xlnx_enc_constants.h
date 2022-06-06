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

#ifndef _XLNX_ENC_CONSTANTS_H_
#define _XLNX_ENC_CONSTANTS_H_

#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <xma.h>
#include <xrm.h>


#define XLNX_ENC_APP_MODULE "xlnx_encoder_app"

#define ENC_APP_DONE 1
#define ENC_APP_STOP 2

#define ENC_DEFAULT_NUM_B_FRAMES   2
#define ENC_DEFAULT_LEVEL          10
#define ENC_DEFAULT_FRAMERATE      25
#define ENC_DEFAULT_SPAT_AQ_GAIN   50
#define ENC_MAX_SPAT_AQ_GAIN       100
#define ENC_DEFAULT_GOP_SIZE       120
#define ENC_MAX_GOP_SIZE           1000
#define ENC_DEFAULT_IDR_PERIOD     (-1)
#define ENC_DEFAULT_BITS_PER_PIXEL 8

#define MAX_ARG_SIZE 64

#define ENC_MIN_LOOKAHEAD_DEPTH 0
#define ENC_MAX_LOOKAHEAD_DEPTH 20

#define ENC_SUPPORTED_MIN_WIDTH 64
#define ENC_DEFAULT_WIDTH       1920
#define ENC_SUPPORTED_MAX_WIDTH 3840
#define ENC_MAX_LA_INPUT_WIDTH  ENC_SUPPORTED_MAX_WIDTH

#define ENC_SUPPORTED_MIN_HEIGHT 64
#define ENC_DEFAULT_HEIGHT       1080
#define ENC_SUPPORTED_MAX_HEIGHT 2160
#define ENC_MAX_LA_INPUT_HEIGHT  ENC_SUPPORTED_MAX_HEIGHT

#define ENC_SUPPORTED_MAX_PIXELS                                               \
    ((ENC_SUPPORTED_MAX_WIDTH) * (ENC_SUPPORTED_MAX_HEIGHT))

#define ENC_MAX_LA_PIXELS ((ENC_MAX_LA_INPUT_WIDTH) * (ENC_MAX_LA_INPUT_HEIGHT))

#define ENC_DEFAULT_BITRATE       5000
#define ENC_DEFAULT_MAX_BITRATE   (ENC_DEFAULT_BITRATE)
#define ENC_SUPPORTED_MAX_BITRATE 35000000

#define ENC_SUPPORTED_MIN_QP 0
#define ENC_SUPPORTED_MAX_QP 51

#define ENC_OPTION_DISABLE 0
#define ENC_OPTION_ENABLE  1

#define ENC_RC_CONST_QP_MODE    0
#define ENC_RC_CBR_MODE         1
#define ENC_RC_VBR_MODE         2
#define ENC_RC_LOW_LATENCY_MODE 3

#define ENC_DEFAULT_GOP_MODE   0
#define ENC_PYRAMIDAL_GOP_MODE 1
#define ENC_LOW_DELAY_P_MODE   2
#define ENC_LOW_DELAY_B_MODE   3

#define ENC_GDR_DISABLE         0
#define ENC_GDR_VERTICAL_MODE   1
#define ENC_GDR_HORIZONTAL_MODE 2

#define ENC_LEVEL_10 10
#define ENC_LEVEL_11 11
#define ENC_LEVEL_12 12
#define ENC_LEVEL_13 13
#define ENC_LEVEL_20 20
#define ENC_LEVEL_21 21
#define ENC_LEVEL_22 22
#define ENC_LEVEL_30 30
#define ENC_LEVEL_31 31
#define ENC_LEVEL_32 32
#define ENC_LEVEL_40 40
#define ENC_LEVEL_41 41
#define ENC_LEVEL_42 42
#define ENC_LEVEL_50 50
#define ENC_LEVEL_51 51
#define ENC_LEVEL_52 52

#define ENC_UNIFORM_QP_MODE       0
#define ENC_AUTO_QP_MODE          1
#define ENC_RELATIVE_LOAD_QP_MODE 2

#define ENC_ASPECT_RATIO_AUTO 0
#define ENC_ASPECT_RATIO_4_3  1
#define ENC_ASPECT_RATIO_16_9 2
#define ENC_ASPECT_RATIO_NONE 3

#define ENC_CAVLC_MODE 0
#define ENC_CABAC_MODE 1

#define ENC_MAX_PARAMS 3

/* H264 Encoder supported profiles */
#define ENC_PROFILE_H264_INTRA (1 << 11) // 8+3; constraint_set3_flag
#define ENC_H264_BASELINE      66
#define ENC_H264_MAIN          77
#define ENC_H264_HIGH          100
#define ENC_H264_HIGH_10       110
#define ENC_H264_HIGH_10_INTRA (110 | ENC_PROFILE_H264_INTRA)

/* Lookahead constants */
#define XLNX_SCLEVEL1            2
#define XLNX_LOOKAHEAD_OUT_ALIGN 64
#define XLNX_LA_MAX_PARAMS       10
#define XLNX_MAX_LOOKAHEAD_DEPTH 20

/* HEVC Encoder supported profiles */
typedef enum {
    ENC_HEVC_MAIN = 0,
    ENC_HEVC_MAIN_INTRA,
    ENC_HEVC_MAIN_10,
    ENC_HEVC_MAIN_10_INTRA
} XlnxHevcProfiles;

typedef enum { ENCODER_ID_H264 = 0, ENCODER_ID_HEVC } XlnxEncoderCodecID;

typedef enum { LOOKAHEAD_ID_H264 = 0, LOOKAHEAD_ID_HEVC } XlnxLookaheadCodecID;

typedef enum {
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

typedef struct {
    char* key;
    int   value;
} XlnxEncProfileLookup;

#endif // _XLNX_ENC_CONSTANTS_H_
