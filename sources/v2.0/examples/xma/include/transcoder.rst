****************************************************
XMA Transcoder App
****************************************************

Transcoder Test Instructions
====================================================

The transcoder XMA application supports most of the options supported by ffmpeg. The XMA application supports only elementary H.264 and HEVC encoded stream input and outputs H.264/HEVC encoded elementary stream.

Transcoder Usage
----------------------------------------------------

Before running transcoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the transcoder app with the ``--help`` option will print the complete list of options::

  ./u30_xma_transcode --help

.. literalinclude:: ./include/transcoder_help.rst

Sample Transcoder Commands
----------------------------------------------------

H.264 to HEVC ABR Transcoder::

  u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
  -c:v mpsoc_vcu_hevc -b:v 4000K  -o out1_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -o out1_test2.265 -c:v mpsoc_vcu_hevc -b:v 2500K -o out1_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -o out1_test4.265 -c:v mpsoc_vcu_hevc -b:v 625K -o out1_test5.265

HEVC ABR Transcoder with Look-Ahead::

  u30_xma_transcode -c:v mpsoc_vcu_hevc -i input_1080p.265 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
  -c:v mpsoc_vcu_hevc -b:v 4000K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test2.265 \
  -c:v mpsoc_vcu_hevc -b:v 2500K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test4.265 \
  -c:v mpsoc_vcu_hevc -b:v 625K -qp-mode 3 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_la_test5.265

H.264 to HEVC ABR Transcoder with Scaler Pipeline::

  u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
  -c:v mpsoc_vcu_hevc -b:v 4000K  -o out3_sc_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -o out3_sc_test2.265 -c:v mpsoc_vcu_hevc -b:v 2500K -o out3_sc_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -o out3_sc_test4.265 -c:v mpsoc_vcu_hevc -b:v 625K -o out3_sc_test5.265

H.264 ABR Transcoder with dynamic encoder parameters::

  u30_xma_transcode -c:v mpsoc_vcu_h264 -i sample.h264 -multiscale_xma -num-output 2 -out_1_width 1280 -out_1_height 720 -out_2_width 848 \
  -c:v mpsoc_vcu_h264 -b:v 4000K -expert-options dynamic-params=cmdfile.txt -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_dp_test1.h264 \
  -c:v mpsoc_vcu_h264 -b:v 3000K -expert-options dynamic-params=cmdfile.txt -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o out_dp_test2.h264

For more examples, refer to the :url_to_xma_apps:`transcoder/test/scripts/transcoder_app_test.sh` script.


Dynamic params config file format for the Transcoder app
--------------------------------------------------------
The config text file should have dynamic parameters for a frame on each line. It can have single parameter or multiple parameters for a frame on each line::

    <frameNumberN1>:<key1>=<value1>
    <frameNumberN2>:<key2>=<value2>,<key3>=<value3>


**Keys allowed with their range of values**

=============================  ===========================  ===========================
Dynamic Parameter              Key                          Valid Values
=============================  ===========================  ===========================
Number of B frames             ``NumB=<int>``               0 to 4
Bitrate (in bits per second)   ``BR=<int>``                 1K to INT_MAX
Temporal AQ mode               ``tAQ=<int>``                0 to 1
Spatial AQ mode                ``sAQ=<int>``                0 to 1
Spatial AQ gain                ``sAQGain=<int>``            0 to 100
=============================  ===========================  ===========================

**Sample dynamic params config file contents**

::

  300:NumB=1
  600:BR=6000000
  1200:sAQ=1,sAQGain=50
  1800:tAQ=1
  2400:NumB=0,BR=10000000,sAQ=0,sAQGain=50,tAQ=0

Limitations of the Transcoder App
====================================================

* The transcoder application supports only elementary H264/HEVC encoded streams as input file. It cannot parse container formats like MP4, AVI, etc.
* The transcoder outputs only H264/HEVC elementary streams.
* The lookahead depth should be same for all the channels.
* The dynamic params config file has a restricted format. Please check above sample config file format.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
