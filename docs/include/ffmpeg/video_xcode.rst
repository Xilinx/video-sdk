*********************************************************
Using FFmpeg for Video Encoding and Decoding on Alveo U30
*********************************************************

General FFmpeg Options
======================

===========================  ===========================
Options                      Descriptions
===========================  ===========================
.. option:: -i               | The input file.
.. option:: -b:v             | Specify the video bitrate.
                             | You can specify this in Mb or Kb. For example -b:v 1M or -b:v 1000K.
                             | Can be specified in Mb or Kb. For example -b:v 1M or -b:v 1000K
.. option:: -c:v             | Specify the video codec. 
                             | This option must be set for any video stream sent to a Alveo U30 device.
                             | Valid values are ``mpsoc_vcu_hevc`` (for HEVC) or ``mpsoc_vcu_h264`` (for H.264)
.. option:: -f               | The container format.
.. option:: -r               | The frame rate in fps (Hz).
.. option:: -filter_complex  | Used to specify ABR scaling options. 
                             | Consult the :ref:`Setting up an ABR ladder <setting-up-an-abr-ladder>` section for more details on how to use this option.
.. option:: -xlnx_hwdev      | Specify on which Alveo U30 device the FFmpeg job should run
                             | Valid values are positive integers. Default is device 0.
                             | Consult the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` section for more details on how to use this option.
===========================  ===========================


Alveo U30 Encoder Options
=========================

=============================  ===========================
Options                        Descriptions
=============================  ===========================
.. option:: -cores             | **Number of encoder cores in the Alveo U30 to utilize**
                               | Valid values: 0 to 4
                               | The Alveo U30 devices are built of encoder and decoder cores that each run at 1080p60, and 1080p120 speed, respectively. 
                               | This means to operate on a 4kp60 (maximum throughput of the chip), all cores are fully utilized automatically.
                               | If you operate on a smaller file (e.g. a single 1080p60 clip), you can leverage all of the cores to increase the FPS at which 
                               | it processes. This is particularly useful in "faster than realtime" (FTRT) use cases, where you want to finish encoding of a clip 
                               | as fast as possible. The :option:`-cores` option will provide diminishing returns when multiple streams are processed on the same device.
                               | Likewise, this option has limited value on livestreaming use-cases as you cannot operate on a video stream faster than you receive it. 
                               | You can find :ref:`faster than realtime examples <faster-than-realtime-example>` in the tutorials section.
.. option:: -slices            | **Number of slices to operate on at once within a core**
                               | Valid values: 0 to 68
                               | Slices are a fundamental part of the stream format. You can operate on these in parallel to increase speed 
                               | at which a stream is processed. 
                               | However, operating on multiple "slices" of video at once will have negative video quality.
                               | When used in conjunction with :option:`-cores`, you can maximize the processing FPS on video streams.  
                               | This option must be used when encoding 4k streams to H.264 in order to sustain real-time performance.
.. option:: -g                 | **GOP size** 
                               | Set this to 2x frame rate for a 2 second GOP
.. option:: -level             | **Encoding level restriction** 
                               | 1 (default). If the user does not set this value, the encoder will automatically assign appropriate level
                               | based on resolution, frame rate and bitrate
                               | Valid values for H.264: 1, 1.1, 1.2, 1.3, 2, 2.1, 2.2, 3, 3.1, 3.2, 4, 4.1, 4.2, 5, 5.1, 5.2 
                               | Valid values for HEVC: 1, 2, 2.1, 3, 3.1, 4, 4.1, 5, 5.1 
.. option:: -profile           | **Set the encoding profile** 
                               | Valid values for H.264: ``high`` (default), ``baseline``, ``main``
                               | Valid values for HEVC: ``main`` (default), ``main-intra``
.. option:: -max-bitrate       | **Maximum bitrate**
                               | Valid values: 0 to 3.5e+10 (default 5e+06)
                               | You may want to use this to limit encoding bitrate if you have not specified a :option:`-b:v` bitrate
.. option:: -periodicity-idr   | **IDR picture frequency**
                               | Valid values: 0 to UINT32_MAX (default)
.. option:: -bf                | **Number of B frames**
                               | Valid values: 0 to 4 (default is 2) 
                               | For tuning use 1 or 2 to improve video quality at the cost of latency. 
                               | Consult the :ref:`B Frames <tuning-b-frames>` section for more details on how to use this option.
.. option:: -lookahead_depth   | **Number of frames to lookahead for qp maps**
                               | Valid values: 0 (default) to 20 
                               | For tuning set this to 20 to improve subjective video quality at the cost of latency.
                               | Lookahead is not supported when encoding 4k streams.
                               | Consult the :ref:`Lookahead <tuning-lookahead>` section for more details on how to use this option.
.. option:: -qp-mode           | **QP control mode**
                               | Valid values: ``auto`` (default), ``relative_load``, ``uniform``
                               | For tuning use uniform for best objective scores
                               | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
.. option:: -spatial-aq        | **Enable spatial AQ**
                               | Valid values: disable or enable (default)
                               | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
.. option:: -spatial-aq-gain   | Percentage of spatial AQ gain.
                               | Valid values: 0 to 100 (default 50)
                               | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
.. option:: -temporal-aq       | **Enable temporal AQ**
                               | Valid values: disable or enable (default)
                               | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
.. option:: -scaling-list      | **Determine if the quantization values are auto scaled**
                               | Valid values:  0, 1 (default)
                               | Consult the :ref:`Scaling List <tuning-scaling-list>` section for more details on how to use this option.
.. option:: -vsync             | **Add in a vsync frame**
                               | Valid values: 0, 1
                               | Set this to 0 to prevent extra frames being added.
=============================  ===========================




Alveo U30 Decoder Options
=========================

====================================  ===========================
Options                               Descriptions
====================================  ===========================
.. option:: -low_latency              | Configure decoder to handle out-of-order frames in order to decrease the latency of the system.
                                      | IMPORTANT: This option should not be used when processing streams containing B frames. 
                                      | Valid values: 0 (default) and 1
.. option:: -entropy_buffers_count    | Specify number of internal entropy buffers.
                                      | Valid values: 2 (default) to 10
                                      | Can be used to improve the performance of ABR ladders for input streams with a high bitrate
                                      | or a high number of reference frames. 2 is enough for most cases. 5 is the practical limit.
.. option:: -splitbuff_mode           | Configure decoder in split/unsplit input buffer mode.
                                      | Valid values: 0 (default) and 1
====================================  ===========================


Alveo U30 Miscellaneous Options
===============================

====================================  ===========================
Options                               Descriptions
====================================  ===========================
.. option:: -latency_logging          | Log latency information to syslog. 
                                      | Valid values: 0 (disabled, default) and 1 (enabled)
.. option:: -loglevel                 | Configures the FFmpeg log level.
                                      | Setting this option to ``debug`` displays comprehensive debug information about the job 
====================================  ===========================


.. _tuning-encoder-options:

Tuning Visual Quality of Encoded Video
======================================
The quality of encoded video depends on various factors. It is primarily a function of target bit rate and type of video content. However, there are some encoder parameters which can be used to adjust the video quality in the Alveo U30 card.

The sections below describe the major FFmpeg options impacting visual quality. Various examples illustrating the effect of these settings can be found here: :doc:`Quality analysis examples </examples/ffmpeg/quality_analysis>`.

.. _tuning-b-frames:

Number of B Frames
-------------------------
The number of B frames can be adjusted according to the amount of motion in the video content. Xilinx suggests setting B frames to 2 for static scenes, slow to medium motion, talking head, or video conferencing type content and 1 for gaming and fast motion content. 

To change B frames, use the :option:`-bf` option on the FFmpeg command line. Valid values are 0 to 4, default is 2.

.. _tuning-lookahead:

Lookahead
-------------------------
Lookahead is used to improve the accuracy of rate control by enabling the encoder to buffer a specified number of frames (using the parameter). Spatial and temporal complexity measures are computed for these frames. The rate control uses these measures to distribute more bits to frames which are hard to encode, and less bits to frames which are easy to encode. This redistribution results in better video quality. When latency is tolerable in applications, Xilinx recommends a lookahead depth of 20 frames to get optimum video quality.

To enable lookahead, use the :option:`-lookahead_depth` option on the FFmpeg command line.


.. _tuning-aq:

Adaptive Quantization
-------------------------
This tool improves the visual quality by changing the quantization parameter (QP) within a frame. The QP for each frame is determined by the rate control, and adaptive quantization (AQ) adjusts QP on top of that for different regions within a frame. It exploits the fact that the human eye is more sensitive to certain regions of a frame and redistributes more bits to those regions. 

The Alveo U30 card supports two types of AQ: Spatial Adaptive Quantization and Temporal Adaptive Quantization. Both of these AQ modes are enabled by default, and :option:`-qp-mode` is set to ``relative-load`` when :option:`-lookahead_depth` >= 1.

Spatial Adaptive Quantization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Spatial AQ adjusts the QP within a frame based on the spatial characteristics. The human eye is more sensitive to regions which are flat and have low texture than regions which have lots of detail and texture. Spatial AQ exploits this and provides more bits to the low texture and flat regions at the expense of high texture regions. This redistribution of bits to visually perceptible regions of the frame brings about visual improvement. Although spatial AQ improves visual quality, it hurts objective metrics and causes a drop in PSNR and VMAF. It is recommended to turn this feature off when performing PSNR/VMAF based evaluation.

The spatial AQ algorithm can be controlled using the :option:`-spatial-aq-gain` option. The range of this option is from 0 to 100 and indicates the strength of this algorithm as a percentage.

To enable spatial AQ, set the :option:`-spatial-aq-gain` to 1 and the :option:`-spatial-aq-gain` to 50 on the FFmpeg command line. If no value is specified for the :option:`-spatial-aq-gain` option, the default value is 50.

Temporal Adaptive Quantization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Temporal AQ adjusts the QP based on the temporal characteristics of the sequence. It utilizes the lookahead frames to capture the temporal characteristics where static/low motion or background is differentiated with high motion regions. The high motion regions are not very sensitive to the human eye as compared with low motion regions. Temporal AQ exploits this fact and redistributes more bits to static or low motion regions.

To enable temporal AQ, set the :option:`-temporal-aq` option to 1 on the FFmpeg command line.


.. _tuning-scaling-list:


Scaling List
-------------------------
Scaling list offers a mechanism to scale the transform coefficients by specifying scaling matrices. This influences the quality of encoded video. There are two options to specify the scaling lists mode: 0 = default and 1 = flat.

For visual quality improvements, the scaling list mode must be set to default. The default scaling mode gives more importance to low-frequency coefficients and less importance to high-frequency coefficients. To improve the objective numbers (such as PSNR and VMAF), the scaling mode must be set to flat, where all the coefficients are scaled equally.

To change the scaling list mode, use the :option:`-scaling-list` option (0 = flat, 1 = default) on the FFmpeg command line.


Considerations for Decoding and Encoding 4K Streams
===================================================

The Xilinx Video SDK solution supports real-time decoding and encoding of 4k streams with the following notes:

- The Alveo U30 video pipeline is optimized for live-streaming use cases. For 4k streams with bitrates significantly higher than the ones typically used for live streaming, it may not be possible to sustain real-time performance.
- When decoding 4k streams with a high bitrate, increasing the number of entropy buffers using the :option:`-entropy_buffers_count` option can help improve performance
- When encoding raw video to 4k, set the :option:`-s` option to ``3840x2160`` to specify the desired resolution.
- When encoding 4k streams to H.264, the :option:`-slices` option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.
- The lookahead feature is not supported for 4k. FFmpeg will give an error if :option:`-lookahead_depth` is enabled when encoding to 4k.



..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.