
H.264 Codec Reference
====================================

H.264 Decoder Options
------------------------------------
The entire list options for the Alveo U30 H.264 decoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h decoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 decoder has the following options::

  Decoder mpsoc_vcu_h264 [MPSOC H.264 Decoder]:
      General capabilities: delay avoidprobe 
      Threading capabilities: none
      Supported pixel formats: xlnx_xvbm
  MPSOC H.264 decoder AVOptions:
    -low_latency       <int>        .D.V..... Should low latency decoding be used (from 0 to 1) (default 0)
    -entropy_buffers_count <int>        .D.V..... Specify number of internal entropy buffers (from 2 to 10) (default 2)
    -latency_logging   <int>        .D.V..... Log latency information to syslog (from 0 to 1) (default 0)
    -splitbuff_mode    <int>        .D.V..... configure decoder in split/unsplit input buffer mode (from 0 to 1) (default 0)


H.264 Encoder Options
------------------------------------
The entire list options for the Alveo U30 H.264 encoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h encoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 encoder has the following options::

  Encoder mpsoc_vcu_h264 [MPSOC H.264 Encoder]:
      General capabilities: delay threads 
      Threading capabilities: auto
      Supported pixel formats: xlnx_xvbm nv12
  MPSOC VCU H264 encoder AVOptions:
    -control-rate      <int>        E..V..... Rate Control Mode (from 0 to 3) (default cbr)
       const-qp                     E..V..... Constant QP
       cbr                          E..V..... Constant Bitrate
       vbr                          E..V..... Variable Bitrate
       low-latency                  E..V..... Low Latency
    -max-bitrate       <int64>      E..V..... Maximum Bit Rate (from 0 to 3.5e+10) (default 5e+06)
    -slice-qp          <int>        E..V..... Slice QP (from -1 to 51) (default auto)
       auto                         E..V..... Auto
    -min-qp            <int>        E..V..... Minimum QP value allowed for the rate control (from 0 to 51) (default 0)
    -max-qp            <int>        E..V..... Maximum QP value allowed for the rate control (from 0 to 51) (default 51)
    -bf                <int>        E..V..... Number of B-frames (from 0 to UINT32_MAX) (default 2)
    -periodicity-idr   <int>        E..V..... IDR Picture Frequency (from -1 to UINT32_MAX) (default -1)
    -profile           <int>        E..V..... Set the encoding profile (from 66 to 100) (default high)
       baseline                     E..V..... Baseline profile
       main                         E..V..... Main profile
       high                         E..V..... High profile
    -level             <int>        E..V..... Set the encoding level restriction (from 10 to 52) (default 1)
       1                            E..V..... 1 level
       1.1                          E..V..... 1.1 level
       1.2                          E..V..... 1.2 level
       1.3                          E..V..... 1.3 level
       2                            E..V..... 2 level
       2.1                          E..V..... 2.1 level
       2.2                          E..V..... 2.2 level
       3                            E..V..... 3 level
       3.1                          E..V..... 3.1 level
       3.2                          E..V..... 3.2 level
       4                            E..V..... 4 level
       4.1                          E..V..... 4.1 level
       4.2                          E..V..... 4.2 level
       5                            E..V..... 5 level
       5.1                          E..V..... 5.1 level
       5.2                          E..V..... 5.2 level
    -slices            <int>        E..V..... Number of Slices (from 1 to 68) (default 1)
    -qp-mode           <int>        E..V..... QP Control Mode (from 0 to 2) (default auto)
       uniform                      E..V..... Use the same QP for all coding units of the frame
       auto                         E..V..... Let the VCU encoder change the QP for each coding unit according to its content
       relative-load                E..V..... Use the information gathered in the lookahead to calculate the best QP
    -aspect-ratio      <int>        E..V..... Aspect-Ratio (from 0 to 3) (default auto)
       auto                         E..V..... 4:3 for SD video, 16:9 for HD video, unspecified for unknown format
       4:3                          E..V..... 4:3 aspect ratio
       16:9                         E..V..... 16:9 aspect ratio
       none                         E..V..... Aspect ratio information is not present in the stream
    -scaling-list      <int>        E..V..... Scaling List Mode (from 0 to 1) (default default)
       flat                         E..V..... Flat scaling list mode
       default                      E..V..... Default scaling list mode
    -cores             <int>        E..V..... Number of cores to use (from 0 to 4) (default auto)
       auto                         E..V..... Automatic
    -lookahead_depth   <int>        E..V..... Number of frames to lookahead for qp maps generation or custom rate control. Up to 20 (from 0 to 20) (default 0)
    -temporal-aq       <int>        E..V..... Enable Temporal AQ. (from 0 to 1) (default enable)
       disable                      E..V..... Disable Temporal AQ
       enable                       E..V..... Enable Temporal AQ
    -spatial-aq        <int>        E..V..... Enable Spatial AQ. (from 0 to 1) (default enable)
       disable                      E..V..... Disable Spatial AQ
       enable                       E..V..... Enable Spatial AQ
    -spatial-aq-gain   <int>        E..V..... Percentage of spatial AQ gain (from 0 to 100) (default 50)
    -latency_logging   <int>        E..V..... Log latency information to syslog (from 0 to 1) (default 0)
    -expert-options    <string>     E..V..... Expert options for MPSoC H.264 Encoder
    -tune-metrics      <int>        E..V..... Tunes MPSoC H.264 Encoder's video quality for objective metrics (from 0 to 1) (default disable)
       disable                      E..V..... Disable tune metrics
       enable                       E..V..... Enable tune metrics


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
