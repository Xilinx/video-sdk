###############################
Using FFmpeg  
###############################

.. highlight:: none

This page documents how to use FFmpeg with the |SDK|.

.. contents:: Table of Contents
    :local:
    :depth: 1

|

************
Introduction
************

FFmpeg is an industry standard, open source, widely used utility for handling video. FFmpeg has many capabilities, including encoding and decoding all video compression formats, encoding and decoding audio, encapsulating, and extracting audio, and video from transport streams, and many more. The |SDK| includes an enhanced version of FFmpeg that can communicate with the hardware accelerated transcode pipeline in Xilinx devices.

It is not within the scope of this document to provide an exhaustive guide on the usage of FFmpeg. Various resources can be found online, for example:

-  https://www.ffmpeg.org/documentation.html
-  http://howto-pages.org/ffmpeg/

The following sections describe the options used with FFmpeg to configure the various hardware accelerators available on Xilinx devices. 

|

*****************************************************
Example Commands
*****************************************************

A simple FFmpeg command for accelerated encoding with the |SDK| will look similar to this one::

   ffmpeg -c:v mpsoc_vcu_h264 -i infile.mp4 -c:v mpsoc_vcu_hevc -s:v 1920x1080 -b:v 1000K -r 60 -f mp4 -y transcoded.mp4

There are many other ways in which FFmpeg can be used to leverage the video transcoding features of Xilinx devices. Examples illustrating how to run FFmpeg for encoding, decoding, and transcoding with and without ABR scaling. To see examples of all these possibilities, refer to the :doc:`FFmpeg tutorials </examples/ffmpeg/tutorials>` included in this repository.

|

.. include:: ./include/ffmpeg/video_xcode.rst

|
  
.. include:: ./include/ffmpeg/video_scaling.rst

|

.. include:: ./include/ffmpeg/data_movement.rst

|

.. include:: ./include/ffmpeg/mapping_audio.rst

|

.. include:: ./include/ffmpeg/ffmpeg_build_instructions.rst

|

*********************************************
Xilinx FFmpeg Reference Guide
*********************************************

H.264 Codec Reference
====================================

H.264 Decoder Options
------------------------------------
The entire list options for the Xilinx H.264 decoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h decoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 decoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_h264_decode.rst
   :language: none


H.264 Encoder Options
------------------------------------
The entire list options for the Xilinx H.264 encoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h encoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 encoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_h264_encode.rst
   :language: none


HEVC Codec Reference
====================================

HEVC Decoder Options
------------------------------------
The entire list options for the Xilinx HEVC decoder (mpsoc_vcu_hevc) can be displayed using the following command::

  ffmpeg -h decoder=mpsoc_vcu_hevc

The mpsoc_vcu_hevc decoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_hevc_decode.rst
   :language: none


HEVC Encoder Options
------------------------------------
The entire list options for the Xilinx HEVC encoder (mpsoc_vcu_hevc) can be displayed using the following command::

  ffmpeg -h encoder=mpsoc_vcu_hevc

The mpoc_vcu_hevc encoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_hevc_encode.rst
   :language: none


Multiscaler Filter Reference
====================================
The entire list options for the Xilinx Multiscaler (multiscale_xma) can be displayed using the following command::

  ffmpeg -h filter=multiscale_xma

The multiple output hardware scaling filter has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_multiscaler.rst
   :language: none
..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
