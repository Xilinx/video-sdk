****************************************************
XMA Transcoder App
****************************************************

The ``examples/xma/transcode`` folder contains a standalone transcoder application using the C APIs (XMA) of the |SDK| to interact with the Xilinx video accelerators.

Transcoder Build Instructions
====================================================

Export the XRT and XRM environment variables needed to build the transcoder application::

   source /opt/xilinx/xrt/setup.sh
   source /opt/xilinx/xrm/setup.sh

In the ``exampes/xma/transcode`` directory, do the following::

   make clean
   make

The resulting executable is placed in the ``build`` directory::

  ./build/u30_xma_transcode


﻿Transcoder Test Instructions
====================================================

The transcoder XMA application supports most of the options supported by ffmpeg. The XMA application supports only elementary H.264 and HEVC encoded stream input and outputs H.264/HEVC encoded elementary stream.

Transcoder Usage
----------------------------------------------------

Before running transcoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the transcoder app with the ``--help`` option will print the complete list of options::

  ./u30_xma_transcode --help

.. literalinclude:: ./include/transcode_help.rst

Sample Transcoder Commands
----------------------------------------------------

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

For more examples, refer to the :url_to_xma_apps:`transcode/test/scripts/transcoder_app_test.sh` script.

Limitations of the Transcoder App
====================================================

* The transcoder application supports only elementary H264/HEVC encoded streams as input file. It cannot parse container formats like MP4, AVI, etc.
* The transcoder outputs only H264/HEVC elementary streams.
* The lookahead depth should be same for all the channels.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
