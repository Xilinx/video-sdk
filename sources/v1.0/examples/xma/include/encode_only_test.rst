Encoder Test Instructions
=========================

The encoder XMA application supports most of the encoder options supported by FFmpeg. The XMA application supports only YUV 4:2:0 semi-planar input and outputs H.264/HEVC encoded elementary stream.

Encoder Usage
-------------

Before running encoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the encoder app with the ``-help`` option will print the complete list of options::

  ./u30_xma_enc -help

.. code-block:: none

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
          -pix_fmt <pixel-format>    Pixel format of the input file (yuv420p / nv12). 
                                     Default input file format will be chosen as nv12. 
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
                                     to 7, default is 2. 
          -periodicity-idr <value>   IDR picture frequency. Supported are 
                                     0 to UINT32_MAX, default is 
                                     UINT32_MAX. 
          -profile <value>           Encoder profile. 
                     For HEVC, supported are 0 or main and 1 or main-intra. 
                                     Default is 0/main. 
                                     ENC_HEVC_MAIN - 0 or main. 
                                     ENC_HEVC_MAIN_INTRA - 1 or main-intra. 
                     For H264, supported are 66 or baseline, 77 or main, 
                                     100 or high, default is 100/high. 
                                     ENC_H264_BASELINE - 66 or baseline. 
                                     ENC_H264_MAIN - 77 or main. 
                                     ENC_H264_HIGH - 100 or high. 
          -level <value>             Encoder level. 
                                     For HEVC, supported are 10 to 51, 
                                     default is 50. 
                                     For H264, supported are 10 to 52, 
                                     default is 50. 
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
          -o <file>                  File to which output is written.




Sample Encoder Commands
-----------------------

H.264 encoding, best objective score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 0 -o out1.264

H.264 encoding, best visual score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out2.264

HEVC encoding, best visual score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out1.265

HEVC encoding, best visual score, normal latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 3 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o out2.265

For more examples, refer to the :url_to_xma_apps:`encode_only/test/scripts/enc_app_test.sh` script.

Limitations of the Encoder App
==============================

* The encoder application assumes that the input YUV file is in NV12 format.
* The application outputs only H264/HEVC elementary streams.
* The application supports multi-process but not multi-channel.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
