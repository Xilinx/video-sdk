Transcoder Test Instructions
============================

The transcoder XMA application supports most of the options supported by ffmpeg. The XMA application supports only elementary H.264 and HEVC encoded stream input and outputs H.264/HEVC encoded elementary stream.

Transcoder Usage
----------------

Before running transcoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the transcoder app with the ``--help`` option will print the complete list of options::

  ./u30_xma_transcode --help

.. code-block:: none

  XMA Transcoder App Usage: 
        ./program [generic options] -c:v <decoder codec> [decoder options]  -i input-file -multiscale_xma -outputs [num] [Scaler options]  -c:v <encoder codec> [encoder options] -o <output-file>  -c:v <encoder codec> [encoder options] -o <output-file>  -c:v <encoder codec> [encoder options] -o <output-file>..... 

  Arguments:

          --help                     Print this message and exit.
          -d <device-id>             Specify a device on which the 
                                     transcoder to run. Default: 0
          -stream_loop <loop-count>  Number of times to loop the input file 
          -frames <frame-count>      Number of input frames to be processed 
                                     
  Decoder options:

          -c:v <codec>               Decoder codec to be used. Supported 
                                     are mpsoc_vcu_hevc, mpsoc_vcu_h264 
          -low-latency <0/1>         Low latency for decoder. Default 
                                     disabled 
          -latency_logging <0/1>     Latency logging for decoder. Default 
                                     disabled 
          -i <input-file>            Name and path of input H.264/HEVC file 
                                     
  Scaler options:

          -multiscale_xma            Name of the ABR scaler filter 
          -num-output <value>        Number of output files from scaler 
          -out_1_width <width>       Width of the scaler output channel 1 
          -out_1_height <height>     Height of the scaler output channel 1 
          -out_1_rate <full/half>    Full of Half rate for output channel 1 
          -out_2_width <width>       Width of the scaler output channel 2 
          -out_2_height <height>     Height of the scaler output channel 2 
          -out_2_rate <full/half>    Full of Half rate for output channel 2 
          -out_3_width <width>       Width of the scaler output channel 3 
          -out_3_height <height>     Height of the scaler output channel 3 
          -out_3_rate <full/half>    Full of Half rate for output channel 3 
          -out_4_width <width>       Width of the scaler output channel 4 
          -out_4_height <height>     Height of the scaler output channel 4 
          -out_4_rate <full/half>    Full of Half rate for output channel 4 
          -out_5_width <width>       Width of the scaler output channel 5 
          -out_5_height <height>     Height of the scaler output channel 5 
          -out_5_rate <full/half>    Full of Half rate for output channel 5 
          -out_6_width <width>       Width of the scaler output channel 6 
          -out_6_height <height>     Height of the scaler output channel 6 
          -out_6_rate <full/half>    Full of Half rate for output channel 6 
          -out_7_width <width>       Width of the scaler output channel 7 
          -out_7_height <height>     Height of the scaler output channel 7 
          -out_7_rate <full/half>    Full of Half rate for output channel 7 
          -out_8_width <width>       Width of the scaler output channel 8 
          -out_8_height <height>     Height of the scaler output channel 8 
          -out_8_rate <full/half>    Full of Half rate for output channel 8 
          -latency_logging <0/1>     Latency logging for scaler. Default 
                                     disabled 
  Encoder options:

          -c:v <codec>               Encoder codec to be used. Supported 
                                     are mpsoc_vcu_hevc, mpsoc_vcu_h264 
          -b:v <bitrate>             Bitrate can be given in Kbps or Mbps 
                                     or bits i.e., 5000000, 5000K, 5M. 
                                     Default is 5000kbps 
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
                                     and 100 or high. 
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


Sample Transcoder Commands
--------------------------

H.264 to HEVC ABR Transcoder::

  ./u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
  -c:v mpsoc_vcu_hevc -b:v 4000K  -o out1_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -o out1_test2.265 -c:v mpsoc_vcu_hevc -b:v 2500K -o out1_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -o out1_test4.265 -c:v mpsoc_vcu_hevc -b:v 625K -o out1_test5.265

HEVC ABR Transcoder with Look-Ahead::

   ./u30_xma_transcode -c:v mpsoc_vcu_hevc -i input_1080p.265 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
   -c:v mpsoc_vcu_hevc -b:v 4000K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test2.265 \
   -c:v mpsoc_vcu_hevc -b:v 2500K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test4.265 \
   -c:v mpsoc_vcu_hevc -b:v 625K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test5.265

H.264 to HEVC ABR Transcoder with Scaler Pipeline::

   ./u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
   -c:v mpsoc_vcu_hevc -b:v 4000K  -o out3_sc_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -o out3_sc_test2.265 -c:v mpsoc_vcu_hevc -b:v 2500K -o out3_sc_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -o out3_sc_test4.265 -c:v mpsoc_vcu_hevc -b:v 625K -o out3_sc_test5.265

For more examples, refer to the :download:`transcode/test/scripts/transcoder_app_test.sh </../examples/xma/transcode/test/scripts/transcoder_app_test.sh>` script.

Limitations of the Transcoder App
=================================

* The transcoder application supports only elementary H264/HEVC encoded streams as input file. It cannot parse container formats like MP4, AVI, etc.
* The transcoder outputs only H264/HEVC elementary streams.
* The lookahead depth should be same for all the channels.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
