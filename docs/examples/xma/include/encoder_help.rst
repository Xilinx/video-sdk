 XMA Encoder App Usage:
    ./program [input options] -i input-file -c:v <codec-option>  [encoder options] -o <output-file>

Arguments:

    --help                     Print this message and exit.
    -d <device-id>             Specify a device on which the encoder
                                to run. Default: 0
    -frames <frame-count>      Number of frames to be processed.

Input options:

    -stream_loop <loop-count>  Number of times to loop the input YUV
                                file.
    -w <width>                 Width of YUV input.
    -h <height>                Height of YUV input.
    -pix_fmt <pixel-format>    Pix format of the input file (yuv420p,
                                nv12, yuv420p10le, xv15). Default: nv12
    -i <input-file>            Name and path of input YUV file

Codec option:
    -c:v <codec>               Encoder codec to be used. Supported
                                are mpsoc_vcu_hevc, mpsoc_vcu_h264
Encoder params:
    -b:v <bitrate>             Bitrate can be given in Kbps or Mbps
                                or bits i.e., 5000000, 5000K, 5M.
                                Default is 200kbps
    -fps <fps>                 Input frame rate. Default is 25.
    -g <intraperiod>           Intra period. Default is 12.
    -control-rate <mode>       Rate control mode. Supported are 0
                                to 3, default is 1.
    -max-bitrate <bitrate>     Maximum bit rate. Supported are 0 to
                                350000000, default is 5000
    -slice-qp <qp>             Slice QP. Supported are -1 to 51,
                                default is -1
    -min-qp <qp>               Minimum QP. Supported are 0 to 51,
                                default is 0.
    -max-qp <qp>               Maximum QP. Supported values are 0
                                to 51, default is 51.
    -bf <frames>               Number of B frames. Supported are 0
                                to INT_MAX, default is 2.
                                For GOP mode 0, supported are 0 to 4
                                For GOP mode 1, supported 3, 5, 7, 15
                                For GOP mode 2, no B frames.
                                For GOP mode 3, supported are 0 to INT_MAX
    -periodicity-idr <value>   IDR picture frequency. Supported are
                                0 to UINT32_MAX, default is
                                UINT32_MAX.
    -force_key_frame <value>   Insert idr frame dynamically (Frame
                                numbers starting from 0)
                                Example: -force_key_frame "(122,222,333)"
    -profile <value>           Encoder profile.
                For HEVC, supported are 0 or main, 1 or main-intra,
                                2 or main-10 and 3 or main-10-intra.
                                Default is 0/main.
                                ENC_HEVC_MAIN - 0 or main.
                                ENC_HEVC_MAIN_INTRA - 1 or main-intra.
                                ENC_HEVC_MAIN_10 - 2 or main-10.
                                ENC_HEVC_MAIN_10_INTRA - 3 or main-10-intra.
                For H264, supported are 66 or baseline, 77 or main,
                                100 or high, 110 or high-10 and
                                2158 or high-10-intra. Default is 100
                                ENC_H264_BASELINE - 66 or baseline.
                                ENC_H264_MAIN - 77 or main.
                                ENC_H264_HIGH - 100 or high.
                                ENC_H264_HIGH_10 - 110 or high-10.
                                ENC_H264_HIGH_10_INTRA - 2158 or high-10-intra.
    -level <value>             Encoder level.
                                For HEVC, supported are 10, 11, 20,
                                21, 30, 31, 40, 41, 50, 51.
                                default is 10.
                                For H264, supported are 10, 11, 12,
                                13, 20, 21, 22, 30, 31, 32, 40, 41, 42, 
                                50, 51, 52. Default is 10.
    -slices <value>            Number of slices per frame. Supported
                                are 1 to 68, default is 1.
    -qp-mode <mode>            QP mode. Supported are 0, 1, and 2,
                                default is 1.
    -aspect-ratio <value>      Aspect ratio. Supported values are 0
                                to 3, default is 0.
    -scaling-list <0/1>        Scaling list. Enable/Disable,
                                default enable.
    -lookahead-depth <value>   Lookahead depth. Supported are 0 to
                                20, default is 0.
    -temporal-aq <0/1>         Temporal AQ. Enable/Disable,
                                default disable.
    -spatial-aq <0/1>          Spatial AQ. Enable/Disable,
                                default disable.
    -spatial-aq-gain <value>   Spatial AQ gain. Supported are 0 to
                                100, default is 50.
    -cores <value>             Number of cores to use, supported are
                                0 to 4, default is 0.
    -tune-metrics <0/1>        Tunes MPSoC H.264/HEVC encoder's video
                                quality for objective metrics, default
                                disable.
    -latency_logging <0/1>     Enable latency logging in syslog.
    -expert-options <string>   Expert options for the encoder, stored
                                as string of key=value pairs.
                                dynamic-params=<params_file>
    -o <file>                  File to which output is written.
..
  ------------

  © Copyright 2020-2022 Xilinx, Inc.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
