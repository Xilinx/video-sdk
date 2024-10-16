.. _using-gstreamer:

###############
Using GStreamer
###############

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::


************
Introduction
************

GStreamer is a pipeline-based multimedia framework that links together a wide variety of media processing systems to complete complex workflows. The pipeline design serves as a base to create many types of multimedia applications such as video editors, transcoders, streaming media broadcasters and media players. The |SDK| includes an enhanced version of GStreamer which can communicate with the hardware accelerated transcode pipeline in Xilinx devices. 

It is not within the scope of this document to provide an exhaustive guide on the usage of GStreamer. Various resources can be found online, for example:

- https://gstreamer.freedesktop.org/


Overview of the GStreamer Plugins
=================================

The |SDK| provides the following GStreamer plugins for building hardware-accelerated video pipelines using Xilinx devices:

* ``vvas_xvcudec`` for H.264/H.265 decoding
* ``vvas_xvcuenc`` for H.264/H.265 encoding
* ``vvas_xabrscaler`` for hardware accelerated scaling and color conversion
* ``vvas_xlookahead`` for hardware accelerated lookahead

All the GStreamer plugins included in the |SDK| are released under the Vitis Video Analytics SDK (VVAS), a framework to build GStreamer-based solutions on Xilinx platforms. Source code and build scripts for the GStreamer plugins developed by Xilinx can be found the in the :file:`sources/video-sdk-gstreamer` folder of the |SDK| repository.

The following sections describe the options used with GStreamer to configure and use the various hardware accelerators available on Xilinx devices.

|

.. _gst-decoder-plugin:

**************
Decoder Plugin
**************

For the complete list of features and capabilities of the Xilinx hardware decoder, refer to the :ref:`Video Codec Unit <video-codec-unit>` section of the :doc:`Specs and Features </specs_and_features>` chapter of the documentation.

The ``vvas_xvcudec`` plugin provides support for hardware-accelerated H.264/H.265 decoding using Xilinx devices.

Decoder Inputs and Outputs
==========================
* Input: H.264/H.265 encoded stream in byte-stream formats
* Output: raw NV12 frames with the following requirements:

Decoder Parameters
==================

.. table:: 
   :widths: 30, 70

   ========================================== ===========================
   Property Name                              Description
   ========================================== ===========================
   .. option:: avoid-dynamic-alloc            | **Avoid dynamic allocation of output buffers**
                                              | Type: Boolean
                                              | Default Value: True
   .. option:: avoid-output-copy              | **Avoid output frames copy**
                                              | Avoid output frames copy even when downstream does not support GstVideoMeta metadata
                                              | Type: Boolean
                                              | Default Value: false
   .. option:: dev-idx                        | **Index of the device on which the decoder should be executed**
                                              | For more details, refer to the section about :ref:`Working with Multiple Devices <gst-multiple-devices>`
                                              | Valid Device index is 0 to 31. Default value is set to -1 intentionally so that user provides the correct device index.
                                              | Type: Integer
                                              | Range: -1 to 31
                                              | Default Value: -1
   .. option:: disable-hdr10-sei              | **Whether to passthrough HDR10/10+ SEI messages or not**
                                              | Type: Boolean
                                              | Default Value: false
   .. option:: interpolate-timestamps         | **Interpolate PTS of output buffers**
                                              | Type: Boolean
                                              | Default Value: false
   .. option:: low-latency                    | **Enable low-latency mode**
                                              | Enabling this mode reduces decoding latency when :option:`splitbuff-mode` is also enabled. **IMPORTANT:** This option should not be used with streams containing B frames. 
                                              | Type: Boolean
                                              | Range: true or false
                                              | Default Value: false
   .. option:: num-entropy-buf                | **Specifies the number of decoder internal entropy buffers**
                                              | Used to smooth out entropy decoding performance. Increasing buffering-count increases decoder memory footprint. Set this value to 10 for higher bit-rate use cases, for example uses cases where the bitrate is more than 100 Mb/s.
                                              | Type: Unsigned integer
                                              | Range: 2 to 10
                                              | Default Value: 2
   .. option:: reservation-id                 | **Resource Pool Reservation id**
                                              | Type: Unsigned Integer64
                                              | Range: 0 - 18446744073709551615
                                              | Default Value: 0
   .. option:: splitbuff-mode                 | **Configure decoder in split/unsplit input buffer mode**
                                              | The split buffer mode hands-off buffers to next pipeline stage earlier. Enabling both :option:`splitbuff-mode` and :option:`low-latency` reduces decoding latency.
                                              | Type: Boolean
                                              | Default Value: false                                           
   ========================================== ===========================


Decoder Example Pipelines
=========================
Refer to the :ref:`Decoder Pipeline <gstreamer-decode-only>` example for an illustration of how to use this plugin.

|

.. _gst-encoder-plugin:

**************
Encoder Plugin
**************

For the complete list of features and capabilities of the Xilinx hardware encoder, refer to the :ref:`Video Codec Unit <video-codec-unit>` section of the :doc:`Specs and Features </specs_and_features>` chapter of the documentation.

The ``vvas_xvcuenc`` plugin provides support for hardware-accelerated H.264/H.265 encoding using Xilinx devices.


Encoder Inputs and Outputs
==========================
* Input: raw 8-bit (NV12) or 10-bit (NV12_10LE32) video data

* Output: H264/H265 8-bit/10-bit encoded stream in byte-stream format

Encoder Parameters
==================

.. table:: 
   :widths: 30, 70

   ========================================== ===========================
   Property Name                              Description
   ========================================== ===========================
   .. option:: aspect-ratio                   | **Display aspect ratio of the video sequence to be written in SPS/VUI**
                                              | Type: Enum
                                              |  (0): auto             - 4:3 for SD video,16:9 for HD video,unspecified for unknown format
                                              |  (1): 4-3              - 4:3 aspect ratio
                                              |  (2): 16-9             - 16:9 aspect ratio
                                              |  (3): none             - Aspect ratio information is not present in the stream
                                              | Default: 0
   .. option:: avc-lowlat                     | **AVC Ultra Low Latency (ULL) density tuning**
                                              | Enable for H264 to run on multiple cores in ultra-low-latency mode.
                                              | Set this to 0 to allow for high density encoding, for resolutions up to 1080p60.
                                              | Set this to 1 to allow for low density encoding, for resolutions up to 4kp60.
                                              | Type: Boolean
                                              | Default: False
                                              | Consult the :ref:`Tuning Transcode Latency <latency-tuning>` section for more details
   .. option:: b-frames                       | **Number of B-frames between two consecutive P-frames**
                                              | Type: Integer
                                              | Range: 0 - 2147483647
                                              | Default: 2
                                              | Consult the :ref:`B Frames <tuning-b-frames>` section for more details on how to use this option.
   .. option:: control-rate                   | **Bitrate control method**
                                              | Type: Enum
                                              |  (0): disable          - Disable
                                              |  (1): constant         - Constant
                                              |  (2): variable         - Variable
                                              |  (3): low-latency      - Low Latency
                                              | Default: 1, "constant"
   .. option:: dependent-slice                | **Specify slice dependency**
                                              | Specifies whether additional slices are dependent on other slice segments or regular slices in multiple slices encoding sessions. Used in H.265 (HEVC) encoding only.
                                              | Type: Boolean
                                              | Default: False                                           
   .. option:: dev-idx                        | **Index of the device on which the encoder should be executed**
                                              | For more details, refer to the section about :ref:`Working with Multiple Devices <gst-multiple-devices>`
                                              | Type: Integer
                                              | Range: -1 - 31
                                              | Default: -1
                                              | Valid Device index is 0 to 31. Default value is set to -1 intentionally so that user provides the correct device index.
   .. option:: enable-pipeline                | **Enable buffer pipelining to improve performance in non zero-copy use cases**
                                              | Type: Boolean
                                              | Default: False
                                              | Set this value to True when the encoder on a device receives data from other device
   .. option:: gop-length                     | **Number of frames in a GOP**
                                              | Distance between two consecutive I frames. Must be a multiple of (b-frames+1)
                                              | Type: Unsigned Integer
                                              | Range: 0 - 1000
                                              | Default: 120
   .. option:: ip-delta                       | **IP delta**
                                              | Type: Integer
                                              | Range: -1 - 51
                                              | Default: -1
   .. option:: loop-filter-beta-offset        | **Loop filter beta offset**
                                              | Type: Integer
                                              | Range: -6 - 6
                                              | Default: -1
   .. option:: loop-filter-tc-offset          | **Loop filter tc offset**
                                              | Type: Integer
                                              | Range: -6 - 6
                                              | Default: -1
   .. option:: max-bitrate                    | **Max bitrate in Kbps.** Only used if control-rate=variable
                                              | Type: Unsigned Integer
                                              | Range: 0 - 35000000
                                              | Default: 5000
   .. option:: max-qp                         | **Maximum QP value allowed for the rate control**
                                              | Type: Unsigned Integer
                                              | Range: 1 - 51
                                              | Default: 51
   .. option:: min-qp                         | **Minimum QP value allowed for the rate control**
                                              | Type: Unsigned Integer
                                              | Range: 1 - 51
                                              | Default: 1
   .. option:: num-cores                      | **Number of encoder cores to be used for current stream**
                                              | If set to 0 (AUTO), the number of encoder cores is automatically determined. Otherwise sets the number of encoder cores to be used.
                                              | Type: Unsigned Integer
                                              | Range: 0 - 4
                                              | Default: 0
   .. option:: num-slices                     | **Number of slices produced for each frame.**
                                              | Each slice contains one or more complete macroblock/CTU row(s). Slices are distributed over the frame 
                                              | as regularly as possible.
                                              | In low-latency mode: 
                                              |  H.264(AVC): 32
                                              |  H.265 (HEVC): 22
                                              | In normal latency-mode:
                                              |  H.264(AVC): picture_height/16
                                              |  H.265(HEVC): minimum of picture_height/32
                                              | Type: Unsigned Integer
                                              | Range: 1 - 68
                                              | Default: 1
   .. option:: pb-delta                       | **PB delta**
                                              | Type: Integer
                                              | Range: -1 - 51
                                              | Default: -1
   .. option:: periodicity-idr                | **Periodicity of IDR frames**
                                              | Type: Unsigned Integer
                                              | Range: 0 - 4294967295
                                              | Default: 4294967295
   .. option:: qos                            | **Handle Quality-of-Service events from downstream**
                                              | Type: Integer
                                              | Type: Boolean
                                              | Default: False
   .. option:: qp-mode                        | **QP control mode used by the encoder**
                                              | Type: Enum
                                              |  (0): Uniform - Use the same QP for all coding units of the frame
                                              |  (1): Auto - Let the encoder handle the QP for each coding unit according to its content
                                              |  (2): ROI - Adjust QP according to the regions of interest defined on each frame.
                                              |  (3): Relative-load - Use the information gathered in the lookahead to calculate the best QP
                                              | Range: 0 - 3
                                              | Default: 1 (i.e. auto)
   .. option:: rc-mode                        | **Enable custom rate control mode**
                                              | To enable custom rate control, set this parameter to True and enable the Lookahead.
                                              | Type: Boolean
                                              | Default: True
   .. option:: reservation-id                 | **Resource Pool Reservation id**
                                              | Type: Unsigned Integer64
                                              | Range: 0 - 18446744073709551615
                                              | Default: 0
   .. option:: scaling-list                   | **Scaling list mode**
                                              | Type: Enum
                                              |  (0): Flat
                                              |  (1): Default
                                              | Range: 0 - 1
                                              | Default: 1
   .. option:: slice-qp                       | **Slice QP mode**
                                              | When RateCtrlMode = CONST_QP the specified QP is applied to all slices.
                                              | When RateCtrlMode = CBR the specified QP is used as initial QP
                                              | Type: Integer
                                              | Range: -1 - 51
                                              | Default: -1
   .. option:: target-bitrate                 | **Target bitrate in Kbps. (5000 Kbps = component default)**
                                              | This property can be changed in all the states of the element including PLAYING state.
                                              | Type: Unsigned Integer
                                              | Range: 0 - 4294967295
                                              | Default: 5000
   .. option:: tune-metrics                   | **Enable tuning video quality for objective metrics**
                                              | Type: Boolean
                                              | Default: False
                                              | Enabling :option:`tune-metrics` automatically forces :option:`qp-mode` = ``uniform`` and :option:`scaling-list` = ``flat``, overwritting any explicit user settings of two values. This option improves objective quality by providing equal importance to all the blocks in the frame: the same quantization parameters and transform coefficients are used for all of them.
   .. option:: ultra-low-latency              | **Ultra low latency mode**
                                              | Serializes encoding when b-frames=0.
                                              | Type: Boolean
                                              | Default: False
   ========================================== ===========================


Encoder Example Pipelines
=========================
Refer to the :ref:`Encoder Pipeline <gstreamer-encode-only>` example for an illustration of how to use this plugin.

|

.. _gst-scaler-plugin:

*****************
ABR Scaler Plugin
*****************
The ``vvas_xabrscaler`` plugin provides support for hardware-accelerated resizing and color space conversion using Xilinx devices. This plugin supports:

#. Single input multiple output pads
#. Color space conversion
#. Resizing
#. Each output pad has independent configurations, like resolution and color space.

For optimal visual quality in scaling operations, refer to :ref:`ABR Scaling <tuning-abr-scaling>` section of the |SDK| user guide.


ABR Scaler Inputs and Outputs
=============================
* Input: raw data with 8-bit (NV12) or 10-bit format (NV12_10LE32)
* Output: raw data with 8-bit (NV12) or 10-bit format (NV12_10LE32)


ABR Scaler Parameters
=====================

.. table:: 
   :widths: 30, 70

   ========================================== ===========================
   Property Name                              Description
   ========================================== ===========================
   .. option:: avoid-output-copy              | **Avoid output frames copy on all source pads**
                                              | Avoid output frames copy on all source pads even when downstream does not support GstVideoMeta metadata
                                              | Type: Boolean
                                              | Default Value: false
   .. option:: coef-load-type                 | **coefficients loading type for scaling**
                                              | Type: Enum
                                              |  (0): fixed   - Use fixed filter coefficients
                                              |  (1): auto    - Auto generate filter coefficients
                                              | Default: 1
   .. option:: dev-idx                        | **Index of the device on which the scaler should be executed**
                                              | For more details, refer to the section about :ref:`Working with Multiple Devices <gst-multiple-devices>`
                                              | Type: Integer
                                              | Range: -1 to 31
                                              | Default: -1
                                              | Valid Device index is 0 to 31. Default value is set to -1 intentionally so that user provides the correct device index
   .. option:: enable-pipeline                | **Enable buffer pipelining to improve performance in non zero-copy use cases**
                                              | Type: Boolean
                                              | Default: False
                                              | Set this value to 1 when the scaler on one device is receiving data from other device
   .. option:: reservation-id                 | **Resource Pool Reservation id**
                                              | Type: Unsigned Integer64
                                              | Range: 0 - 18446744073709551615
                                              | Default Value: 0
   ========================================== ===========================


ABR Scaler Example Pipelines
============================
Refer to the :ref:`Transcode with Multiple-Resolution Outputs <gstreamer-transcode-and-scale>` example for an illustration of how to use this plugin.

|

.. _gst-lookahead-plugin:

****************
Lookahead Plugin
****************
The ``vvas_xlookahead`` plugin provides support for hardware-accelerated lookahead functionality using Xilinx devices. This plugin is typically used in conjunction with the encoder plugin to improve the subjective quality of the encoded stream at the cost of latency. The plugin can generate quantization parameters (QP buffer) as well as frame sum of absolute differences and frame activity information (FSFA buffer). The QP and FSFA buffers are attached to the input frame as metadata and sent to next element in the GStreamer media pipeline

Lookahead Inputs and Outputs
============================
* Accepts raw NV12 frames up to resolution 3840x2160 as input
* Produces QP and/or FSFA buffers based on the following user controls:

  - QP buffer is generated when the :option:`lookahead-depth` option is set to a value greater than 0.
  - FSFA buffer is generated when the :option:`rc-mode` option is enabled in the Encoder.

Lookahead Parameters
====================

.. table:: 
   :widths: 30, 70
   
   ========================================== ===========================
   Property Name                              Description
   ========================================== ===========================
   .. option:: b-frames                       | **Number of B-frames between two consecutive P-frames**
                                              | The same setting should be used for the encoder and lookahead plugins. This property can be changed in PAUSED/PLAYING state of the element when the encoder ``gop-mode`` is basic.
                                              | Type: Integer
                                              | Range: 0 - 2147483647
                                              | Default: 2
   .. option:: codec-type                     | **Codec type H264/H265**
                                              | The same setting should be used for the encoder and lookahead plugins. Based on the codec type, corresponding metadata will be produced and attached to the input frame
                                              | Type: Enum
                                              |  (0): H264
                                              |  (1): H265                                           
                                              | Default: None
   .. option:: dev-idx                        | **Index of the device on which the lookahead should be executed**
                                              | For more details, refer to the section about :ref:`Working with Multiple Devices <gst-multiple-devices>`
                                              | Type: Integer
                                              | Range: -1 to 31
                                              | Default: -1
                                              | Valid Device index is 0 to 31. Default value is set to -1 intentionally so that user provides the correct device index
   .. option:: dynamic-gop                    | **Whether to enable dynamic GOP mode**
                                              | Type: Boolean
                                              | Default Value: false
                                              | Set this to 1 to enable dynamic insertion of B frames, within 0-2 range
                                              | Consult the :ref:`Dynamic GOP <dynamic-gop>` section for more details.
   .. option:: enable-pipeline                | **Enable buffer pipelining to improve performance in non zero-copy use cases**
                                              | Type: Boolean
                                              | Default: False
                                              | Set this value to True when the lookahead on a device receives data from other device
   .. option:: lookahead-depth                | **Lookahead depth**
                                              | The maximum lookahead depth allowed is 1/3 of the stream frame rate. When latency is tolerable, use the maximum lookahead depth to get optimum video quality to get optimum video quality. Consult the :ref:`Lookahead <tuning-lookahead>` section for more details on how to use this option.
                                              | Type: Unsinged Integer
                                              | Range: 1 to 20
                                              | Default: 8
   .. option:: max-qp                         | **Maximum QP value allowed for the rate control**
                                              | Type: Unsigned Integer
                                              | Range: 0 - 51
                                              | Default: 51
   .. option:: min-qp                         | **Minimum QP value allowed for the rate control**
                                              | Type: Unsigned Integer
                                              | Range: 0 - 51
                                              | Default: 0
   .. option:: reservation-id                 | **Resource Pool Reservation id**
                                              | Type: Unsigned Integer64
                                              | Range: 0 - 18446744073709551615
                                              | Default Value: 0
   .. option:: spatial-aq                     | **Enable/Disable Spatial AQ activity**
                                              | This property can be changed in all the states of the element including PLAYING state.
                                              | Type: Boolean
                                              | Default: True
   .. option:: spatial-aq-gain                | **Percentage of Spatial AQ gain**. Applied when enable-spatial-aq is true
                                              | This property can be changed in all the states of the element including PLAYING state.
                                              | Type: Unsigned Integer
                                              | Range: 0 to 100
                                              | Default: 50
   .. option:: temporal-aq                    | **Enable/Disable Temporal AQ linear**
                                              | This property can be changed in all the states of the element including PLAYING state.
                                              | Type: Boolean
                                              | Default: True
   ========================================== ===========================


Lookahead Example pipeline
==========================
Refer to the :ref:`Transcode with Lookahead for Multiple-Resolution outputs <gstreamer-lookahead-and-scale>` example for an illustration of how to use this plugin.

::

  gst-launch-1.0 filesrc location=<input mp4 absolute path> ! qtdemux ! queue ! h264parse ! \
    vvas_xvcudec    dev-idx=0 ! queue ! \
    vvas_xlookahead dev-idx=0 kernel-name=lookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 ! queue ! \
    vvas_xvcuenc    dev-idx=0 target-bitrate=4000 rc-mode=1 ! \
    h264parse ! "video/x-h264" ! fpsdisplaysink video-sink="fakesink" text-overlay=false sync=false -v


|

********************************
Moving Data through the Pipeline
********************************

The GStreamer plugins included in the |SDK| take care of efficiently moving data through the video processing pipeline. For optimal performance, video buffers will be moved between the host and the appropriate device only if needed by plugins in the pipeline. This is known as "zero-copy".

- The decoder plugin will only copy the output buffer from the device to the host if a downstream element (e.g. filesink) running on the host needs to access the decoded frame. This allows downstream hardware-accelerated plugins (e.g. scaler, encoder) running on the same Xilinx device to process the decoded frames without unnecessary data transfers.

- The encoder plugin will only copy the input buffer from host to the device if it is allocated in user-space memory on the host or in a different FPGA device. This allows the encoder to process frames produced by upstream hardware-accelerated plugins (e.g. decoder, scaler) running on the same Xilinx device without unnecessary data transfers.

|

.. _gst-multiple-devices:

*****************************
Working with Multiple Devices
*****************************

By default (if no device identifier is specified) a job is submitted to device 0. When running large jobs or multiple jobs in parallel, device 0 is bound to run out of resources rapidly and additional jobs will error out due to insufficient resources. 

By using the :option:`dev-idx` option of the GStreamer plugins included in the |SDK|, the different functions (decoder, scaler, encoder) of a pipeline can be individually submitted to a specific device. This makes it easy and straightforward to leverage the entire video acceleration capacity of your system, regardless of the number of cards and devices. 

Consult the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` section for more details on how to work with multiple devices.

|

.. include:: ./include/gstreamer/mapping_audio.rst

|

.. include:: ./include/gstreamer/gstreamer_build_instructions.rst

|

..
  ------------

  © Copyright 2020-2023, Advanced Micro Devices, Inc.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
