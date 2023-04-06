###############################
Specs and Features of the |SDK|
###############################

.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::


*********
The |SDK|
*********

The |SDK| is a complete software stack allowing users to seamlessly leverage the features of Xilinx video acceleration units such as the ones available on Alveo U30 cards and |VT1| instances. It includes the following elements:

* Pre-compiled versions of FFmpeg and GStreamer which integrate key video transcoding plug-ins, enabling simple hardware offloading of compute-intensive workloads using these two popular frameworks. These custom versions of FFmpeg and GStreamer link to a host driver which communicates with the hardware on the PCIe card. No hardware experience is required to run FFmpeg or GStreamer commands with the |SDK|.

* The Xilinx Resource Manager (XRM) which is the software used to manage and allocate all the hardware-accelerated features available in the system.  XRM allows running multiple video processing jobs across multiple devices and multiple Xilinx video acceleration cards.

* A C-based application programming interface (API) which facilitates the integration of Xilinx video transcoding capabilities in proprietary frameworks. This API is provided in the form plugins which can be called from external application using the Xilinx Media Accelerator (XMA) interface.

* A suite of card management tools used to perform actions such as programming, resetting, or querying the status of Xilinx video acceleration cards. 

* Many examples and tutorials illustrating how to use and make the most of the |SDK|. 

.. image:: ./images/ffmpeg_transcoding_application.png
   :alt: FFmpeg Transcoding Application
   :align: center


*************************
The Xilinx Alveo U30 Card
*************************

The `Xilinx® Alveo™ U30 data center accelerator card <https://www.xilinx.com/products/boards-and-kits/alveo/u30.html>`_ is a low-profile, PCI™-based media accelerator card that delivers a high-density real-time transcoding solution for live streaming video service providers, OEMs, and Content Delivery Network (CDNs).

The Alveo U30 card is primarily targeted at real-time video workloads. It is expected that one or more sources of video input, either from files or from live video streams, are fed into the transcode pipeline. The encoder encodes one or more output streams from each scaled rendition of the input.

.. _video-codec-unit:

Video Codec Unit
================
The Alveo U30 data center accelerator card is powered by two Zynq® UltraScale+™ MPSoC devices with H.264/H.265 Video Codec Unit (VCU) cores. Features of the H.264/H.265 VCUs include:

* Video format: YCbCr 4:2:0, 8 or 10-bit per color channel
* Multi-standard encoding/decoding support, including:

  * ISO MPEG-4 Part 10: Advanced Video Coding (AVC)/ITU H.264 - Baseline, Constrained Baseline, High, High-10, High-10-Intra up to Level 5.2
  * ISO MPEG-H Part 2: High Efficiency Video Coding (HEVC)/ITU H.265 - Main, Main-Intra, Main10, Main-10-Intra, up to Level 5.2 High Tier

* Supports resolutions from 128x128 to 3840x2160
* Simultaneous encoding and decoding with a maximum aggregated bandwidth of 2x 4Kp60 per card
* Look-ahead driven video quality improvements through temporal adaptive and spatial adaptive quantization
* Low latency rate control
* Flexible rate control: CBR, VBR, and Constant QP
* Simultaneous encoding and decoding up to 2 times 4K UHD resolution at 60 Hz per card
* Progressive support for H.264 and H.265
* HDR10/10+: HDR data is automatically populated by the decoder and passed to other accelerators in the transcode pipeline. 

  * The following HDR10 SEI are supported:

    * Mastering Display Color Volume (SEI ITU)
    * Content Light Level (SEI ITU)
    * Alternative Transfer Charateristics (SEI ITU)

  * The following HDR10+ SEI are supported:

    * ST2094_10 (DolbyVision, User defined SEI)
    * ST2094_40 (Samsung, User defined SEI)

  * Behavior for HDR10/10+ SEI is as follows:

    * Static HDR SEI (MDCV, CLL & ATC) will not change in-between IDRs (and even in the video sequence according the HDR standards).
    * MDCV, CLL & ATC will be written only on IDRs, according to the persistency of MDCV, CLL & ATC SEIs.
    * ST2094_10 will be written on each access unit as per constraint of section A.2.1 ts_103572v010101p.pdf.
    * ST2094_40 will be written on IDRs, and whenever the user changes its content, according to the persistency specification in A341S34-1-582r4-A341-Amendment-2094-40.pdf

.. _adaptive-bitrate-scaler:

Adaptive Bitrate Scaler
=======================
For streaming applications, video is distributed in different resolutions and bit rates to adapt to varying network bandwidth conditions. All adaptive bitrate (ABR) transcoding systems require an ABR scaler that downscales an input video stream to several different smaller resolutions that are then re-encoded. These smaller resolutions are referred to as an image pyramid or an ABR ladder.

The Xilinx ABR scaler is an accelerator capable of generating up to eight lower resolution output images from a single input image. The ABR scaler supports the following features:

* Supports up to 12 taps in both horizontal and vertical direction per stage
* High quality polyphase scaling with 64 phases and up to 12 taps in both horizontal and vertical direction per stage
* Supports 8 and 10-bit 4:2:0
* Luma and Chroma processed in parallel
* Supports 1080p60 real time or equivalent distributed between up to eight outputs
* Supports resolutions from 128x128 to 3840x2160, in multiples of 4
* Supports aggregate bandwidth of 2160p120 per device, to enable creating multiple ABR ladder renditions from a 2160p60 input
* The scaler is tuned for downscaling and expects non-increasing resolutions in an ABR ladder. Increasing resolutions between outputs is supported but will reduce video quality.

The |SDK| allows using the Xilinx ABR scaler as follows:

* Up to 32 input streams of raw or encoded video can be scaled down per device
* Each input stream can be scaled down to a maximum of 8 outputs streams of lower resolution and/or lower frame rate
* Up to 32 scaled outputs streams are supported per device, up to a maximum total equivalent bandwidth of 4kp60
* Supports resolutions from 128x128 to 3840x2160, in multiples of 4
* The scaler passes scaled frames and meta data to the next scaling level (if one is defined) and to the encoder (if one is being used)
* Each level of scaling adds a little more latency to the pipeline


.. _perf-vq:

Video Quality
=============

The Alveo U30 card nominally produces video quality (VQ) that is closely correlated to x264 and x265 faster presets. This video quality is highly dependent on video content so actual results may vary.

.. _perf-tables:

Performance Tables
==================

The video processing power of the Alveo U30 cards can be harnessed in many different ways, from running a few high-definition jobs to running many low-resolution ones, with or without scaling. The tables below show how many jobs can be run at real-time speed based on the use case and the number of cards available. All these configurations have been tested and validated by Xilinx and assume normal operating ranges.

Performance Tables for 8-bit Color
----------------------------------

.. include:: /include/specs_and_features/performance_tables_8bit.rst


Performance Tables for 10-bit Color
-----------------------------------

.. include:: /include/specs_and_features/performance_tables_10bit.rst


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
