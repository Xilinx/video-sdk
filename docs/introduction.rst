#######################################################
Specs and Features of the Xilinx Video SDK
#######################################################

.. contents:: Table of Contents
    :local:
    :depth: 1

*****************************
The Xilinx Alveo U30 Card
*****************************

The Xilinx® Alveo™ U30 data center accelerator card is a low-profile, PCI™-based media accelerator card that delivers a high-density real-time transcoding solution for live streaming video service providers, OEMs, and Content Delivery Network (CDNs).

The Alveo U30 card is primarily targeted at real-time video workloads. It is expected that one or more sources of video input, either from files or from live video streams, are fed into the transcode pipeline. The encoder encodes one or more output streams from each scaled rendition of the input. 

.. image:: ./images/ffmpeg_transcoding_application.png
    :alt: FFmpeg Transcoding Application
    :align: center


******************************
The Xilinx Video SDK
******************************

The Xilinx Video SDK is a complete software stack allowing users to seamlessly leverage the hardware accelerated features of Alveo U30 cards. It includes the following elements:

* A pre-compiled version of FFmpeg which integrates key video transcoding plug-ins, enabling simple hardware offloading of compute-intensive workloads using the popular FFmpeg command line interface. This custom version of FFmpeg links to a host driver which communicates with the hardware on the PCIe card. No hardware experience is required to run FFmpeg commands on the Alveo U30 card.

* The Xilinx Resource Manager (XRM) which is the software used to manage and allocate all the hardware-accelerated features available in the system.  XRM allows running multiple video processing jobs across multiple devices and multiple Alveo U30 cards.

* A C-based application programming interface (API) which facilitates the integration of Alveo U30 transcoding capabilities in proprietary frameworks. This API is provided in the form plugins which can called from external application using the Xilinx Media Accelerator (XMA) interface.

* A suite of card management tools used to perform actions such as programming, resetting, or querying the status of Alveo U30 cards. 

* Many examples and tutorials illustrating how to use and make the most of Alveo U30 cards. 

*****************************
Hardware Features
*****************************

Video Codec Unit
================
The Alveo U30 data center accelerator card is powered by two Zynq® UltraScale+™ MPSoC devices with H.264/H.265 Video Codec Unit (VCU) cores. Features of the H.264/H.265 VCUs include:

* Multi-standard encoding/decoding support, including:

  * ISO MPEG-4 Part 10: Advanced Video Coding (AVC)/ITU H.264
  * ISO MPEG-H Part 2: High Efficiency Video Coding (HEVC)/ITU H.265
  * HEVC: Main, Main Intra, up to Level 5.1 High Tier
  * AVC: Baseline, Main, High, up to Level 5.2
* Supports resolutions from 128x128 to 3840x2160
* Simultaneous encoding and decoding of up to 46 streams with a maximum aggregated bandwidth of two 3840x2160\@60 fps per card
* Look-ahead driven video quality improvements through temporal adaptive and spatial adaptive quantization for resolutions up to 1080p
* Low latency rate control
* Flexible rate control: CBR, VBR, and Constant QP
* Simultaneous encoding and decoding up to 2 times 4K UHD resolution at 60 Hz per card
* Progressive support for H.264 and H.265
* Video input: YCbCr 4:2:0, 8-bit per color channel

.. _adaptive-bitrate-scaler:


Adaptive Bitrate Scaler
=======================
For streaming applications, video is distributed in different resolutions and bit rates to adapt to varying network bandwidth conditions. All adaptive bitrate (ABR) transcoding systems require an ABR scaler that downscales an input video stream to several different smaller resolutions that are then re-encoded. These smaller resolutions are referred to as an image pyramid or an ABR ladder.

The Xilinx ABR scaler is an accelerator capable of generating up to eight lower resolution output images from a single input image. The ABR scaler supports the following features:

* Supports up to 12 taps in both horizontal and vertical direction per stage
* High quality polyphase scaling with 64 phases and up to 12 taps in both horizontal and vertical direction per stage
* Dynamically configurable filter coefficients
* Supports 8-bit 4:2:0
* Luma and Chroma processed in parallel
* Supports 1080p60 real time or equivalent distributed between up to eight outputs
* Supports resolutions from 128x128 to 3840x2160, in multiples of 4
* Supports aggregate bandwidth of 2160p120 per device, to enable creating multiple ABR ladder renditions from a 2160p60 input
* Although the main use case is downscaling, the ABR scaler also allows for upscaling. The outputs must be configured in descending order


*****************************
Performance Tables
*****************************

The video processing power of the Alveo U30 cards can be harnessed in many different ways, from running a few high-definition jobs to running many low-resolution ones, with or without scaling. The tables below show how many jobs can be run at real-time speed based on the use case and the number of cards available. All these configurations have been tested and validated by Xilinx.

.. include:: ./include/introduction/performance_table.rst


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
