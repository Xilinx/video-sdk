###################
|SDK| Release Notes
###################

.. note::
   Version: |release|

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

|

**********
What's New
**********

This version provides various performance and stability improvements and new features including:

- Support for deployment and installation through package feed
- Dynamic GOP
- Ultra low latency (ULL) encoding
- Min/max frame QP bounding
- Enhanced operating system support


|

****************
Changed Behavior
****************

New k8s-device-plugin
=====================
A new Kubernetes plugin is available for U30 cards. This new plugin no longer requires an exact version number when specifying U30 resource limits. This allows for compatibility of pod description files across all releases of the |SDK|. Kubernetes clusters using this plugin will continue to work seamlessly even after an upgrade of the |SDK|. **If using Kubernetes, to prevent service degradation, it is necessary to deploy this new plugin (version 1.1.0)**  before upgrading to the latest version of the |SDK|. For more details, refer to the instructions for :ref:`deploying with Kubernetes <deploying-with-kubernetes>`. 


|

*****************************************
Supported Platforms and Operating Systems
*****************************************

The |SDK| currently supports the following platforms:

- Xilinx Alveo U30
- |VT1|

The |SDK| currently supports the following software tools:

- FFmpeg 4.4
- GStreamer 1.16.2

The |SDK| has been tested and validated by Xilinx on the following operating environments:

- Amazon Linux 2 (Kernel 4.14 and 5.10)
- Ubuntu 22.04 (Kernel 5.15)
- Ubuntu 20.04.4 (Kernel 5.13)
- Ubuntu 20.04.3 (Kernel 5.11)
- Ubuntu 20.04.1 (Kernel 5.4)
- Ubuntu 20.04.0 (Kernel 5.4)
- Ubuntu 18.04.5 (Kernel 5.4)
- RHEL 7.8 (Kernel 4.9.184)

The above list of validated operating environments is not intended to be exclusionary. If you choose to run the |SDK| using a different operating environment and encounter issues, please `file a GitHub issue <https://github.com/Xilinx/video-sdk/issues>`_.


|

***********
Limitations
***********

- The |SDK| supports both 8 and 10-bit streams, but they cannot be mixed together. The entire pipeline must be either 8 or 10-bits.

- The |SDK| is optimized for live video real-time transcoding. Even though a single Alveo U30 card has the capability of running two Ultra HD (4K) transcodes in real-time at 60 frames per second (2x4kp60), running a single 4k stream at 120 fps is not supported. Similarly, a single Alveo U30 card supports up to 8x1080p60 transcodes in real-time, but it does not support fewer streams running at 120fps or beyond.

- The maximum aggregate bandwidth bitrate tested per device is 35 Mb/s for 4kp60 for 8-bit and 10-bit streams. Performance of video encoding is not qualified for higher bitrates.

- When using 10-bit streams, real-time performance for an aggregate bandwidth of 4kp60 per device is not be guaranteed. 

  + Techniques to improve performance include: enabling pipelining when using the scaler, using low latency input streams, adjusting the output bitrate, increasing the number of entropy buffers in the decoder and reducing the number of B-frames in the encoder and the input stream.

- When decoding streams with a high bitrate or with a large number of B-frames, jobs may not meet real-time performance when 2 entropy buffers are used (default setting). In this case, it is recommended to use more entropy buffers. The maximum number of entropy buffers in the decoder is 10.

- 4K60 encode-only and decode-only use cases involve significant transfers of raw data between the host and the device. This may impact the overall performance.

- When encoding 4k streams to H.264, the :option:`-slices` option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.

- Transcode pipelines split across two devices involving a H264 codec will not meet realtime performance for 10-bit streams.

- When processing a 1080p60 stream, the scaler can only sustain a rate of 60fps for up to 5 outputs. Therefore, given 4 1080p60 input streams, real-time performance is only possible for up to 20 scaled outputs.

- The scaler is tuned for downscaling and expects non-increasing resolutions in an ABR ladder. Increasing resolutions between outputs is supported but will reduce video quality.

- The lookahead depth must be less than the periodicity of Intra and IDR frames.

- If the decoded input stream has fps information in both the container and the VUI header, the decoder takes the fps information from the container instead of the VUI header.

- HDR10/10+ is supported only for transcode uses cases (involving both the hardware decoder and encoder) and with the following restrictions:

  + Ambient Viewing Environment SEI not supported
  + Tone mapping Info SEI not supported
  + Color Remapping Info SEI not supported
  + HDR10+ Data caching for frame rate conversion is not supported
  + Metadata adaptation for resolution scaling is not supported
  + Transfer characteristics other than AL_TRANSFER_CHARAC_BT_2100_PQ (16) and AL_TRANSFER_CHARAC_BT_2100_HLG (18) are not supported. If any other value is specified, the encoder defaults to AL_TRANSFER_CHARAC_UNSPECIFIED (2). 
  + Color matrix coefficients other than AL_COLOUR_MAT_COEFF_BT_2100_YCBCR (9) are not supported. If any other value is specified, the encoder defaults to AL_COLOUR_MAT_COEFF_UNSPECIFIED (2).

|

************
Known Issues
************

Potential "failed" error code when sourcing the setup.sh script
===============================================================
.. https://jira.xilinx.com/browse/CR-1139665

- Description: If you source the setup script on a machine where the |SDK| environment has already been initialized, you will see the "failed" status being reported. 

- Solution: This is not an indication of a problem. This status simply states that your environment is already set up and that the system is ready for use.::
    
    -----Load xrm plugins-----
    {
        "response": {
            "name": "loadXrmPlugins",
            "requestId": "1",
            "status": "failed",
            "data": {
                "failed": "plugin xrmU30DecPlugin (version 2) is already loaded"
            }
        }
    }
    
    ---------------------------------------


Running 'xbutil validate' from a VM with version 1.5 installed gives an error
=============================================================================

- Description: After the devices have been flashed with the version 2.0 or newer, running 'xbutil validate' from a virtual machine with version 1.5 installed results in an error. The validation program included in version 1.5 of the |SDK| is not forward compatible with version 2.0 or newer.

- Solution: None 


Custom rate control needs a minimum of 8 lookahead frames for reasonable picture quality
========================================================================================
.. https://jira.xilinx.com/browse/CR-1127435

- Description: Custom rate control is automatically enabled when the lookahead depth is set to 1 or more. However, to obtain reasonable picture quality, custom rate control needs a minimum of 8 lookahead frames.

- Solution: Increase the lookahead depth to 8 frames or more when using custom rate control, or set the control rate to constant when using less a lookahead depth smaller than 8.


The encoder adds duplicate frames for streams with 'fps' and 'timescale' parameters which do not match
======================================================================================================
.. https://jira.xilinx.com/browse/CR-1113049

- Description: When the ‘fps’ and ‘timescale’ parameters of the input stream do not match, FFmpeg calculates a framerate value which leads to duplicate streams.

- Solution: Ensure  'fps' and 'timescale' parameters match


FFmpeg jobs split across two devices need the xvbm_convert filter to move data between devices
==============================================================================================
.. https://jira.xilinx.com/browse/CR-1107581

- Description: FFmpeg will generate garbage results if a job is split across two devices without using the :option:`xvbm_convert` filter to copy the frame buffers between devices. 

- Solution: Use :option:`xvbm_convert` filters to ensure the second device operates on valid data, as explained in the :ref:`data movement <ffmpeg-data-movement>` section of the documentation.


Initialization error when encoding to 4K
========================================
.. https://jira.xilinx.com/browse/CR-1116058
.. https://jira.xilinx.com/browse/CR-1143721

- Description: When running a 4K pipeline, the following error is seen when the encoder is initialized::

  [XMA] ERROR:  ** xma-vcu-encoder VCU_INIT failed : device error: Channel creation failed, processing power of the available cores insufficient.

- Solution: 4k30 encoding requires 2 cores and 4k60 encoding requires 4 cores. Set the ``-slices`` and ``-cores`` encoder options accordingly.


Potential "Too many packets buffered for output stream 0:1" error when running FFmpeg
=====================================================================================
.. https://jira.xilinx.com/browse/CR-1110063

- Description: Some FFmpeg jobs may fail with a error message indicating "Too many packets buffered for output stream 0:1.". This is a known FFmpeg issue (https://trac.ffmpeg.org/ticket/6375) which is not specific to the |SDK|.

- Solution: Run FFmpeg with the ``-max_muxing_queue_size 9999`` option


FFmpeg RTMP Origin Lip Sync Issue
=================================
.. https://jira.xilinx.com/browse/CR-1137599

- Description: On some rtmp streaming sources, lip sync issue may be observed.

- Solution: To resolve this issue add ``-vsync cfr`` option to the decoder.


Multiple "Starting new cluster" messages when encoding to Matroska
==================================================================
.. https://jira.xilinx.com/browse/CR-1092967

- Description: When encoding to .mkv, FFmpeg may have issues synchronizing video and audio data leading to messages of the following form: ``[matroska @ 0x563b3f675100] Starting new cluster``

- Solution: Pass ``-max_interleave_delta 0`` on the FFmpeg command line


Streams with rotation metadata may cause a segfault while transcoding
=====================================================================
.. https://jira.xilinx.com/browse/CR-1093015
.. https://jira.xilinx.com/browse/CR-1092997

- Description: The hardware transcode pipeline can be unstable on a stream with rotation metadata.

- Solution: Use the FFmpeg ``-noautorotate`` option in cases where rotation metadata is present. 
  

Abrupt termination of FFmpeg processes may cause video resources to not be released correctly
=============================================================================================
.. https://jira.xilinx.com/browse/CR-1092946

- Description: Rerunning FFmpeg after abrutly terminating previous runs gives an "xrm_allocation: resource allocation failed" error message indicating that there are not enough video resources available to run this job on the Xilinx device.

- Solution: Users should terminate all running FFmepg processes before exiting their shell. Otherwise, a SIGHUP will be sent to the running FFmpeg processes and this may result in an unhandled signal leading to non-graceful termination and video resources will not be released correctly.


.. _rhel-subscription:


GStreamer package installation failure on RHEL due to dependencies
==================================================================
- Description: Package installation may fail as it is required to have an active RHEL subscription to download packages from RHEL's repositories.

- Solution: Get a subscription to RHEL and re-run the installation. Refer to https://access.redhat.com/solutions/253273


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
