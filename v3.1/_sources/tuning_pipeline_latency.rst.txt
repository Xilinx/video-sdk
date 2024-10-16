
.. _latency-tuning:

########################
Tuning Transcode Latency
########################

.. highlight:: none
.. contents:: Table of Contents
    :local:
    :depth: 1

Latency tuning allows for trade off among end to end delay, video bit rate, GOP composition, etc. Interactive applications that require low latency, can do so by setting relevant parameters in both decoder and encoder. In the latter case, it is understood that lowering the latency comes at the cost of increased bit rate, for a given visual quality.

.. _tuning-decoder-latency:

Decoder Latency
===============
For streams without B frames, decoding latency can be reduced by enabling both the :option:`-low_latency` and :option:`-splitbuff_mode` decoder options in FFmpeg or the :option:`low-latency` and :option:`splitbuff-mode` options in GStreamer. 

The low latency decoding mode is not compatible with B frames. If the input streams contains B-frames, the :option:`-low_latency` option should not be used.

When using the XMA APIs, the :option:`-splitbuff_mode` should only be used if the application can always send a complete Access Unit in one shot to the decoder.

.. _tuning-encoder-latency:

Encoder Latency
===============

Xilinx encoder performs multi-objective optimization with set constraints on bit rate, GOP topology, visual quality measures, etc. As such, it may be tuned to achieve a compromise between latency and quality or be specialized to ultra low latency.

Guidelines on Encoder Latency Configuration
-------------------------------------------

Encoding latency can also be reduced by trading off compression rate or visual quality. The following table lists the encoder options which can used to reduce to that effect.

.. table::
   :widths: 35, 65

   =============================  ===========================
   Encode Options                 Notes
   =============================  ===========================
   Look Ahead Depth               | For best visual quality, it is recommend to set the lookahead depth to 1/3 of the frame rate; however, this also increases latency proportionally to the requested depth. It is possible to reduce latency at the expense of visual quality by reducing the lookahead depth.
   QP Mode                        | Any option other than *uniform* requires extra processing and hence will increase the pipeline delay.
   Scaling List                   | Disabling this option skips over a visual quality refinement process and as such will reduce the pipeline delay.
   Adaptive Quantizations (AQ)    | Temporal and Spatial AQ require extra processing stage and will increase the pipeline delay. Disabling AQ will improve encoding latency.
   Rate Control                   | Setting this to any option other than *low latency* will result in extra delay.
   Number of B frames             | It is understood that for every inserted B frame there will be a frame period delay.
   =============================  ===========================

Ultra Low Latency (ULL) Mode
----------------------------

Ultra Low Latency (ULL) encoding is enabled by setting :option:`-disable-pipeline` flag in FFmpeg or :option:`ultra-low-latency` in GStreamer. 

.. rubric:: Notes

- The ultra low latency encoding mode is not compatible with B frames. Requesting the insertion of B frames by the encoder will result in an immediate termination of the pipeline when ULL is enabled. The user must choose between encoding in ULL mode or adding B frames to the encoded stream.

- When ULL is enabled, the maximum achievable real-time rate is 4kp30.

.. rubric:: Considerations for AVC encoding

Encoding to AVC using the ULL mode affects the maximum density and resolution per device. The FFmpeg :option:`-avc-lowlat` and GStreamer :option:`avc-lowlat` options must be set as a based on the desired density or resolution, as explained below.

- If ``avc-lowlat`` is set to 0 (default), only a single accelerator core can be used per stream. This implies that only resolutions up to 1080p60 are supported. However, multiple streams may be active on a given core, which means that density is maximized.
- If ``avc-lowlat`` is set to 1, multiple accelerator cores can be used per stream. This implies that all resolutions are supported. However, only a single stream may be active on a given core which limits overall density on the device.

Given that each accelerator core is capable of encoding at an aggregate rate of 1080p60, it is usually best to use the following ``avc-lowlat`` settings when encoding to AVC in ULL mode:

- If the resolution is 4kp30 or higher, set ``avc-lowlat`` to 1 (as more than 1 core is needed for these resolutions)
- If the resolution is 1080p60 or lower, set ``avc-lowlat`` to 0 for of 1080p60 or lower (to maximize density)

Furthermore, when encoding to AVC in the ULL mode, realtime performance is only guaranteed for 1080p60 and not beyond.

These considerations do not apply when encoding to HEVC in ULL mode.



Example Use Cases
=================
The following scripts exemplify typical use cases for each possible low latency pipeline configuration:

#. To achieve low latency with control over visual quality refer to:

   * :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_ll_subjective.sh` and :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_ll_subjective.sh` to obtain low latency with good subjective quality
   * :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_ll_objective.sh` and :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_ll_objective.sh` to obtain low latency with good objective quality
   
#. :url_to_examples:`examples/u30/ffmpeg/tutorials/07_ffmpeg_transcode_lowlatency.sh` to obtain ultra low latency transcode

#. :url_to_examples:`examples/u30/ffmpeg/quality_analysis/latency_test.sh` to measure decode and encode latencies.


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
