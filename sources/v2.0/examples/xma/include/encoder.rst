****************************************************
XMA Encoder App
****************************************************

Encoder Test Instructions
====================================================

The encoder XMA application supports most of the encoder options supported by FFmpeg. The XMA application supports only YUV 4:2:0 input and outputs H.264/HEVC encoded elementary stream.

Encoder Usage
----------------------------------------------------

Before running encoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the encoder app with the ``-help`` option will print the complete list of options::

  u30_xma_encode -help

.. literalinclude:: ./include/encoder_help.rst


Sample Encoder Commands
----------------------------------------------------

H.264 encoding, best objective score, low latency::

  u30_xma_encode -w 1920 -h 1080  -i input_1080.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 0 -o out1.264

H.264 encoding, best visual score, low latency::

  u30_xma_encode -w 1920 -h 1080  -i input_1080.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out2.264

H.264 encoding, using dynamic encoder paramters::

  u30_xma_encode -w 1920 -h 1080 -pix_fmt yuv420p -i input_1080.yuv -fps 60 -g 120 -periodicity-idr 120 -frames 600 -c:v mpsoc_vcu_h264 -expert-options dynamic-params=cmdfile.txt -lookahead-depth 20 -spatial-aq 1 -temporal-aq 1 -spatial-aq-gain 80 -o out3.264

HEVC encoding, best visual score, low latency::

  u30_xma_encode -w 1920 -h 1080  -i input_1080.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out1.265

HEVC encoding, best visual score, normal latency::

  u30_xma_encode -w 1920 -h 1080  -i input_1080.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 3 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o out2.265

HEVC encoding, using dynamic encoder paramters::

  u30_xma_encode -w 1920 -h 1080 -pix_fmt yuv420p -i input_1080.yuv -fps 60 -g 120 -periodicity-idr 120 -frames 600 -c:v mpsoc_vcu_hevc -expert-options dynamic-params=cmdfile.txt -lookahead-depth 20 -spatial-aq 1 -temporal-aq 1 -spatial-aq-gain 80 -o out3.265

For more examples, refer to the :url_to_xma_apps:`encoder/test/scripts/encoder_app_test.sh` script.

Dynamic params config file format for the Encoder app
-----------------------------------------------------
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


Limitations of the Encoder App
====================================================

* The encoder application supports only yuv 420 8-bit or 10-bit.
* The application outputs only H264/HEVC elementary streams.
* The application supports multi-process but not multi-channel.
* The dynamic params config file has a restricted format. Please check above sample config file format.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
