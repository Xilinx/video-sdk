######################
Video Quality Examples
######################

.. highlight:: none

This page is dedicated to explaining some of the details behind Video Quality (VQ), how it is measured, and how you can optimize your GStreamer commands with the Alveo U30 card to maximize its performance. 

Further documentation on this topic can be found in the :ref:`Tuning Video Quality <tuning-video-quality>` section of the U30 Video SDK user guide.


.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::


*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst

*****************************
Introduction to Video Quality
*****************************

.. include:: /include/examples/intro_to_vq.rst

********************************
Optimized Settings for the |SDK|
********************************
It is highly recommended to perform encoding on raw video clips; that is, clips that have not undergone a transform/compression/encoding in the past. This ensures that the clips are in a universally known state in order to fairly compare encoders. 

Alternatively, you can add the flags to decode before encoding, and the results will remain accurate as long as the same pre-encoded file is used as the source across all encoders under test. Information on this process can be found on the :doc:`GStreamer tutorial page </examples/gstreamer/tutorials>`.

Flags not illustrated in this page are covered in the :doc:`Using GStreamer </using_gstreamer>` chapter of the |SDK| user guide.


Subjective Quality
==================

- Example script for H264 streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/h264_subjective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/hevc_subjective.sh`

These are the command you should use to get maximum video quality to the human eye in most situations. It accepts a clip that is already decoded or a RAW YUV.

**Usage**::

    ./h264_subjective.sh  <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>
    ./hevc_subjective.sh  <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>


**Command Line Example**::

  gst-launch-1.0 filesrc location=~/videos/Test_1080p60.nv12 ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 ! vvas_xlookahead codec-type=0 lookahead-depth=20 spatial-aq=true temporal-aq=true dev-idx=0 b-frames=1 ! vvas_xvcuenc dev-idx=0 b-frames=1 gop-length=120 periodicity-idr=120 qp-mode=relative-load target-bitrate=5000 max-bitrate=5000 ! h264parse ! fpsdisplaysink video-sink="filesink location=/tmp/xil_enc_5000_subjective.h264 " text-overlay=false sync=false -v

    

Objective Quality
=================

- Example script for H264 streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/h264_objective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/hevc_objective.sh`

**Usage**::

    ./h264_objective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>
    ./hevc_objective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>

**Command Line Example**::

    gst-launch-1.0 filesrc location=Test_1080p60.NV12 ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 ! vvas_xlookahead codec-type=0 lookahead-depth=20 b-frames=1 dev-idx=0 ! vvas_xvcuenc dev-idx=0 b-frames=1 gop-length=120 periodicity-idr=120 tune-metrics=1 target-bitrate=5000 max-bitrate=5000 ! h264parse ! fpsdisplaysink video-sink="filesink location=/tmp/xil_enc__objective.h264 " text-overlay=false sync=false -v

This is the command you should run to get maximum objective scoring (PSNR, SSIM, VMAF). It accepts a clip that is already decoded or a RAW NV12 file.

Setting Differences Between Objective and Subjective Quality
============================================================

- ``qp-mode``

  + How an encoder quantizes its CU's (Macroblocks/Coding Tree Units/etc.) is what fundamentally defines a large amount of its quality.
   
- ``spatial-aq`` and ``temporal-aq``

  + Adaptive Quantization (AQ) exploits the fact that the human eye is more sensitive to certain regions of a frame. This method drops information from high-frequency locations and keeps more information in low-frequency locations in a frame. The result appears more visually appealing. To enable spatial or temporal AQ, qp-mode should be set to relative-load and lookahead should be enabled.
  
  + Imagine a scene of a windy forest: the moving leaves (high frequency/texture data), and tree trunks on the ground (low-frequency/texture data). Artifacts and issues in the low-frequency data will catch your eye much more than the high-frequency data. AQ will drop data in the leaves (they are much harder to see changes from frame-to-frame) and make sure the trunks and ground keep more of their data. It is a zero-sum game when compressing data.
  
  + Spatial AQ is redistribution of bits/data within a frame, while temporal AQ is data over time (i.e. over many frames). With Temporal AQ, the same concepts apply: high-motion regions are less noticeable than low-motion regions; Temporal AQ looks ahead in the Lookahead buffer to determine which is which and will redistribute bits/data accordingly for a more visually appealing scene.
  
  + There is another flag which is enabled (but is set to default in these strings, so it is omitted in the command line) with ``spatial-aq``. The flag is ``spatial-aq-gain`` and can be set 0-100; default is 50. This parameter is the strength of the redistribution of data within the frame. Setting too high a value may have a consequence of blurring edges. Experimentation across your clips is recommended if you wish to tune the parameter. We keep it to 50(%) to cover the widest set of use cases.

|

*****************************
Running PSNR/SSIM/VMAF scores
*****************************

Enabling VQ Scoring in GStreamer
================================

In order to enable VQ scoring in GStreamer, but VMAG plugin must be added. The VMAF GStreamer plugin can be compiled by following the steps described in https://werti.github.io/GSoC2019/. It is required to integrate https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/tree/master/ext/iqa with gst-plugins-bad-1.16.2 to match the GStreamer version of ths release package. 

The :file:`examples/gstreamer/quality_analysis/vmaf` directory included in this repository contains the following files which can be used to integrate the VMAF plugin in gst-plugins-bad-1.16.2::
    
    +-- install_vmaf.sh
    +-- model
    +-- patches
    ¦   +-- 0001-Add-Xilinx-s-format-support.patch
    ¦   +-- 0001-build-Adapt-to-backwards-incompatible-change-in-GNU-.patch
    ¦   +-- 0001-Building-the-vmaf-as-dynamic-library.patch
    ¦   +-- 0001-Derive-src-fps-from-vui_time_scale-vui_num_units_in_.patch
    ¦   +-- 0001-gst-plugins-base-Add-HDR10-support.patch
    ¦   +-- 0001-Update-Colorimetry-and-SEI-parsing-for-HDR10.patch
    ¦   +-- 0001-Videoaggregator-cleanup-functions.patch
    ¦   +-- 0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch
    +-- README.md

* The :file:`install_vmaf.sh` script installs VMAF as follows:

  - Applies the required patches from patches directory
  - Clones VMAF and builds the VMAF library
  - Builds and installs the iqa-vmaf plugin
    
* vmaf/model is a directory which contains different models used by the iqa-vmaf plugin. 

* 0001-Add-Xilinx-s-format-support.patch adds a patch to gst-plugins-base-1.16.2 which supports xilinxs format buffers and discussion about this patch is out of scope for this topic.

* 0001-build-Adapt-to-backwards-incompatible-change-in-GNU-.patch adds a patch to adapt to backwards incompatible change in GNU Make 4.3 and discussion about this patch is out of scope for this topic.

* 0001-Building-the-vmaf-as-dynamic-library.patch adds a patch on meson build of vmaf library to build it dynamically and to link dynamically with the iqa-vmaf plugin.y

* 0001-Derive-src-fps-from-vui_time_scale-vui_num_units_in.patch adds a patch to derive source fps from vui parameters and discussion about this patch is out of scope for this topic.

* 0001-gst-plugins-base-Add-HDR10-support.patch adds a patch to enhance base plugins with HDR10 support and corresponding colorimetry info . The discussion about this patch is out of scope for this topic.

* 0001-Update-Colorimetry-and-SEI-parsing-for-HDR10.patch adds a patch to update colorimetry and sei parsing and discussion about this patch is out of scope for this topic.

* 0001-Videoaggregator-patch.patch  adds a patch to gst-plugins-base-1.16.2/gst-libs/gst/video/gstvideoaggregator.h . This patch contains the macros of some cleanup functions which were later introduced in gst-plugins-base-1.17.1. This patch is required to build the iqa-vmaf plugin.

* 0001-VMAF-integration-in-gst-plugins-bad-1.16.2.patch adds a patch to gst-plugins-bad-1.16.2. i) This patch replaces gst-plugins-bad-1.16.2/ext/iqa directory with https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/tree/master/ext/iqa which contains the source code of iqa-vmaf plugin.


**Building the VMAF GStreamer plugin**

Build and install the VMAF plugin as follows::

    cd vmaf
    source install_vmaf.sh

Verify that the plugin was successfully installed::
    
    gst-inspect-1.0 iqa-vmaf
    
The description and usage of the iqa-vmaf plugin can be found in https://gitlab.freedesktop.org/szve/gst-plugins-bad/-/blob/master/ext/iqa/iqa-vmaf.c

Performing VQ Scoring with GStreamer and the VMAF Plugin
========================================================

- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/measure_vq.sh`

**Usage**::
    
    ./measure_vq.sh <Encoded Clip generated using Master YUV clip> <Resolution ('W'x'H')> <Framerate> <Master YUV clip> <VMAF mMdel>

**Command Line Example**::

    ./measure_vq.sh u30_3mpbs_clip.mp4 1920x1080 60 original_clip.yuv vmaf/model/vmaf_v0.6.1.pkl


After installing vmaf using the ``install_vmaf.sh`` script, you can generate VQ metrics using the ``measure_vq.sh`` script

For each frame of the input clip, the ``measure_vq.sh`` script calculates and prints out several VQ metrics, such as ADM2, MOTION2, MS_SSIM, PSNR, SSIM, etc... 
    
The sample below shows the output of the ``measure_vq.sh`` script for frame number 499 of the input clip::
    
    Got message #678 from element "vmaf" (element): IQA-VMAF, padname=(string)sink_1, frame_num=(uint)499, metrics=(structure)"metrics\,\ adm2\=\(double\)0.96335089655397854\,\ motion2\=\(double\)3.5312457084655762\,\ ms_ssim\=\(double\)0.98481947307803175\,\ psnr\=\(double\)34.567887440129212\,\ ssim\=\(double\)0.99244385957717896\,\ vif_scale0\=\(double\)0.50567488401912375\,\ vif_scale1\=\(double\)0.88340058464896432\,\ vif_scale2\=\(double\)0.94600789149917519\,\ vif_scale3\=\(double\)0.9717222600144203\,\ vmaf\=\(double\)89.285598932085932\;";


**Complete Sequence:**

::

    # Install the VMAF plugin
    cd vmaf
    source install_vmaf.sh
    cd ..

    # Generate an encoded mp4 file from a 1080p60 8-bit YUV input file
    gst-launch-1.0 -v filesrc location=original_8bit_1080p60_clip.yuv \
        ! queue \
        ! rawvideoparse format=i420 width=1920 height=1080 framerate=60/1 \
        ! videoconvert ! video/x-raw, format=NV12 \
        ! vvas_xvcuenc dev-idx=0 target-bitrate=3000 max-bitrate=3000 \
        ! h264parse \
        ! qtmux \
        ! fpsdisplaysink video-sink="filesink location=/tmp/u30_3mpbs_clip.mp4" text-overlay=false sync=false"

    # Measure VQ
    ./measure_vq.sh u30_3mpbs_clip.mp4 1920x1080 60 original_8bit_1080p60_clip.yuv vmaf/model/vmaf_v0.6.1.pkl

|

*******************
Quality vs. Latency
*******************

A given encoder's "quality" is often a function of many different algorithms/functions/features. It is quite possible (and often seen) that an encoder can produce an h.264/HEVC compliant stream but have drastically different quality from one to another. 

Some of these features add latency, either by adding "pitstops" on the way to an outputted stream, or by increasing the complexity of the core-encoding functions. Most things in the video realm are content-dependent, or use-case-dependent, so the designer needs to determine what is best for them... a gradient of:

- absolute best quality with high latency

- lower quality with lowest latency. 

Xilinx-Specific Latency Flags
=============================

Decoder Options
---------------

- ``low_latency``

  + This flag when set to 0 disables the decoder's ability to process B-frames. Skipping this logic and providing an input with B-Frames will have jittery, undesired outputs.
  
Encoder Options
---------------
.. - ``control-rate=low-latency``
  
..   + While this flag is documented, it **should not be used**, please ignore it
  
- ``b-frames=<INT>``

  + This is the number of B-Frames inserted into the GOP. B-frames reference both past and future frames, so to build them, it will be required to have a buffer.
  
- ``scaling list``

  + Enabling this allows for an extra step of scaling low-frequency coefficients before they are quantized in the encoder. When enabled, better quality, higher latency; when disabled, lower quality, better/lower latency.
  
Lookahead Options
-----------------
- ``-lookahead_depth <INT>``

  + In order to best determine how best to encode the incoming video, you can create a buffer that the encoder can use to search for clues/hints. It drastically improves quality, but every frame you provide is another frame of latency.

- ``qp-mode=relative-load``

  + Using the FPGA, we are preprocessing the stream and making intelligent decisions which we can provide to the encoder as "hints". Adding this step helps improve quality at the cost of latency.
  + ``auto`` uses a more basic engine and will be slightly faster, lower quality.
  + ``uniform`` is fastest at the lowest quality for this option

- ``temporal-aq`` and ``spatial-aq``

  + These features are described above on this page; performing their functions increases both latency and quality.

Optimized Settings for Low Latency Streams
==========================================

With the above information in hand, below are the optimized commands for general types of video. Your content may require modifications to optimize fully.

Low Latency Subjective Quality
------------------------------

- Example script for H264 streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/h264_ll_subjective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/hevc_ll_subjective.sh`

**Usage**::

    ./h264_ll_subjective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>
    ./hevc_ll_subjective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>


**Command Line**::

    gst-launch-1.0 filesrc location=Test_1080p60.NV12 ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 ! vvas_xvcuenc dev-idx=0 b-frames=0 gop-length=120 periodicity-idr=120 qp-mode=auto target-bitrate=5000 max-bitrate=5000 ! h264parse ! fpsdisplaysink video-sink="filesink location=/tmp/xil_enc__ll_subjective.h264 " text-overlay=false sync=false -v

Low Latency Objective Quality
-----------------------------

- Example script for H264 streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/h264_ll_objective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/hevc_ll_objective.sh`

**Usage** ::

    ./h264_ll_objective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>
    ./hevc_ll_objective.sh <device index> <Input 1080p60 NV12 file> <target-bitrate in kbps>


**Command Line** ::

    gst-launch-1.0 filesrc location=~/videos/Test_1080p60.NV12 ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 ! vvas_xvcuenc dev-idx=0 b-frames=0 gop-length=120 periodicity-idr=120 tune-metrics=1 target-bitrate=5000 max-bitrate=5000 ! h264parse ! fpsdisplaysink video-sink="filesink location=/tmp/xil_enc__ll_objective.h264 " text-overlay=false sync=false -v

Measuring Latency
=================

Latency numbers are measured using GStreamer framework's latency tracer module. After enabling GST_DEBUG as follows, run the given example script which will dump the log to log.txt and retrieve the latency values using ``gst-stats-1.0`` command::

    export GST_DEBUG="GST_TRACER:7" GST_TRACERS="latency(flags=pipeline+element)" GST_DEBUG_FILE=log.txt

Ladder pipeline with 1080p60 MP4 file with H264 elementarty stream with I and P frames as input, and generates 720p60, 720p30, 480p30, 360p30 and 160p30 output ladders is used to measure the latency. This script has encoder and decoder parameters set to ensure minumum latency requirements of the acceleration components.

Latency Measurement Script
--------------------------

- Example script for HEVC streams: :url_to_examples:`examples/u30/gstreamer/quality_analysis/latency_test.sh`

**Usage** ::

    ./latency_test.sh <Input 1080p60 MP4 file with H.264>

**Command Line** ::

    e.g. ./latency_test.sh ~/videos/bbb_sunflower_1080p_60fps_normal.mp4
    
    Average end to end latency (720p60 leg) in milli seconds :  56.0469
    Average decoder latency in milli seconds :  11.4191
    Average ecnoder latency (720p60 leg) in milli seconds :  38.5176
    Average scaler latency (720p60 leg) in milli seconds :  5.7965

Change the script accordingly to measure latencies of the other ABR ladders

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
