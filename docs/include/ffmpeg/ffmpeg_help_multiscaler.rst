
Multiscaler Filter Reference
====================================
The entire list options for the Alveo U30 Multiscaler (multiscale_xma) can be displayed using the following command::

  ffmpeg -h filter=multiscale_xma

The multiple output hardware scaling filter has the following options::

  Filter multiscale_xma
    Xilinx Multi Scaler (in ABR mode) using XMA APIs
      Inputs:
         #0: default (video)
      Outputs:
          dynamic (depending on the options)
  multiscale_xma AVOptions:
    outputs           <int>        ..FV..... set number of outputs (from 1 to 8) (default 8)
    out_1_width       <int>        ..FV..... set width of output 1 (should be multiple of 4) (from 128 to 3840) (default 1600)
    out_1_height      <int>        ..FV..... set height of output 1 (should be multiple of 4) (from 128 to 3840) (default 900)
    out_1_pix_fmt     <string>     ..FV..... set format of output 1 (default "xlnx_xvbm")
    out_1_rate        <string>     ..FV..... set rate of output 1 (default "full")
    out_2_width       <int>        ..FV..... set width of output 2 (should be multiple of 4) (from 128 to 3840) (default 1280)
    out_2_height      <int>        ..FV..... set height of output 2 (should be multiple of 4) (from 128 to 3840) (default 720)
    out_2_pix_fmt     <string>     ..FV..... set format of output 2 (default "xlnx_xvbm")
    out_2_rate        <string>     ..FV..... set rate of output 2 (default "full")
    out_3_width       <int>        ..FV..... set width of output 3 (should be multiple of 4) (from 128 to 3840) (default 800)
    out_3_height      <int>        ..FV..... set height of output 3 (should be multiple of 4) (from 128 to 3840) (default 600)
    out_3_pix_fmt     <string>     ..FV..... set format of output 3 (default "xlnx_xvbm")
    out_3_rate        <string>     ..FV..... set rate of output 3 (default "full")
    out_4_width       <int>        ..FV..... set width of output 4 (should be multiple of 4) (from 128 to 3840) (default 832)
    out_4_height      <int>        ..FV..... set height of output 4 (should be multiple of 4) (from 128 to 3840) (default 480)
    out_4_pix_fmt     <string>     ..FV..... set format of output 4 (default "xlnx_xvbm")
    out_4_rate        <string>     ..FV..... set rate of output 4 (default "full")
    out_5_width       <int>        ..FV..... set width of output 5 (should be multiple of 4) (from 128 to 3840) (default 640)
    out_5_height      <int>        ..FV..... set height of output 5 (should be multiple of 4) (from 128 to 3840) (default 480)
    out_5_pix_fmt     <string>     ..FV..... set format of output 5 (default "xlnx_xvbm")
    out_5_rate        <string>     ..FV..... set rate of output 5 (default "full")
    out_6_width       <int>        ..FV..... set width of output 6 (should be multiple of 4) (from 128 to 3840) (default 480)
    out_6_height      <int>        ..FV..... set height of output 6 (should be multiple of 4) (from 128 to 3840) (default 320)
    out_6_pix_fmt     <string>     ..FV..... set format of output 6 (default "xlnx_xvbm")
    out_6_rate        <string>     ..FV..... set rate of output 6 (default "full")
    out_7_width       <int>        ..FV..... set width of output 7 (should be multiple of 4) (from 128 to 3840) (default 320)
    out_7_height      <int>        ..FV..... set height of output 7 (should be multiple of 4) (from 128 to 3840) (default 240)
    out_7_pix_fmt     <string>     ..FV..... set format of output 7 (default "xlnx_xvbm")
    out_7_rate        <string>     ..FV..... set rate of output 7 (default "full")
    out_8_width       <int>        ..FV..... set width of output 8 (should be multiple of 4) (from 128 to 3840) (default 224)
    out_8_height      <int>        ..FV..... set height of output 8 (should be multiple of 4) (from 128 to 3840) (default 224)
    out_8_pix_fmt     <string>     ..FV..... set format of output 8 (default "xlnx_xvbm")
    out_8_rate        <string>     ..FV..... set rate of output 8 (default "full")
    latency_logging   <int>        ..FV..... Log latency information to syslog (from 0 to 1) (default 0)

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
