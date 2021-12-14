########################################
|SDK| Release Notes
########################################

.. note::
   Version: |release|

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

|

******************************
What's New
******************************

This version provides various performance and stability improvements, including:

- Support for |VT1| instances as well as Amazon EKS and ECS
- Support for Amazon Linux 2
- Improved density, especially when processing many low-resolution streams
- Improved runtime for both single and multi-process execution models
- Improved resiliency of XMA and XVBM libraries

|

*****************************************
Supported Platforms and Operating Systems
*****************************************

The |SDK| currently supports the following platforms:

- Xilinx Alveo U30
- |VT1|

The |SDK| currently supports the following software tools:

- FFmpeg 4.1

The |SDK| has been tested and validated by Xilinx on the following operating environments:

- Ubuntu 20.04 (Kernel 5.4)
- Ubuntu 18.04 (Kernel 5.4)
- RHEL 7.8 (Kernel 4.9.184)
- Amazon Linux 2

The above list of validated operating environments is not intended to be exclusionary. If you choose to run the |SDK| using a different operating environment and encounter issues, please `file a GitHub issue <https://github.com/Xilinx/video-sdk/issues>`_.


|

******************************
Limitations
******************************

- The scaler is tuned for downscaling and expects non-increasing resolutions in an ABR ladder. Increasing resolutions between outputs will reduce video quality. For more details see :ref:`Using FFmpeg for Video Scaling <using-ffmpeg-for-video-scaling>`.

- The |SDK| is optimized for live video real-time transcoding. For example, even though a single Alveo U30 card has the capability of running two Ultra HD (4K) transcodes in real-time at 60 frames per second (2x4kp60), running a single 4k stream at 120 fps is not supported. Similarly, a single Alveo U30 card supports up to 8x1080p60 transcodes in real-time, but it does not support fewer streams running at 120fps or beyond.

- When processing 4x 1080p60 streams, the scaler can only sustain a rate of 60fps for up to 5 outputs. Therefore, given 4 1080p60 input streams, real-time performance is only possible for up to 20 scaled outputs.

- The maximum bit-rate tested is 35 Mb/s; if the bit-rate is higher, the encoder might not work properly.

- ABR video transcode use cases have been tested with up to 5 output renditions per input for a maximum supported number of channels of 8 running real-time per card. 

- The lookahead feature is not supported when encoding streams to a 4k resolution.

|

******************************
Known Issues
******************************

Potential "failed" error code when sourcing the setup.sh script
=================================================================

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


Potential "Too many packets buffered for output stream 0:1" error when running FFmpeg
=====================================================================================
.. https://jira.xilinx.com/browse/CR-1110063

- Description: Some FFmpeg jobs may fail with a error message indicating "Too many packets buffered for output stream 0:1.". This is a known FFmpeg issue (https://trac.ffmpeg.org/ticket/6375) which is not specific to the |SDK|.

- Solution: Run FFmpeg with the ``-max_muxing_queue_size 9999`` option


Multiple "Starting new cluster" messages when encoding to Matroska
==================================================================
.. https://jira.xilinx.com/browse/CR-1092967

- Description: When encoding to .mkv, FFmpeg may have issues synchronizing video and audio data leading to messages of the following form: ``[matroska @ 0x563b3f675100] Starting new cluster``

- Solution: Pass ``-max_interleave_delta 0`` on the FFmpeg command line


Streams with rotation metadata may cause a segfault while transcoding
=====================================================================
.. https://jira.xilinx.com/browse/CR-1093015
.. https://jira.xilinx.com/browse/CR-1092997

- Description: The hardware transcode pipeline can be unstable on a stream with rotation metadata

- Solution: Use the FFmpeg ``-noautorotate`` option in cases where rotation metadata is present. 
  

Generating a device status report with the xrmadm tool does not work on |VT1| instances
=======================================================================================
.. https://jira.xilinx.com/browse/CR-1107503

- Description: The ``xrmadm /opt/xilinx/xrm/test/list_cmd.json`` command fails with a Traceback error message.

- Solution: None.


Reverting to the golden image may not work by default on some on-prem systems
=============================================================================
- Description: The ``xbmgmt program --revert-to-golden`` and ``xbmgmt flash --factory_reset`` command expects the golden image of the card to be flashed at a particular memory location. If this is not the case, the command will not work. 

- Solution: Relocate the golden image by following the instructions below.  

  .. _flashing-the-golden-image:

  #. To reflash the golden image, your system must be already configured with release 0.96.0 or newer. If this is not the case, `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

  #. Set up your environment:: 

      source /opt/xilinx/xrt/setup.sh

  #. List the Management BDF of your devices::

      sudo /opt/xilinx/xrt/bin/xbmgmt examine

  #. For each device, run the following command to flash the golden image to the expected memory location::

      sudo /opt/xilinx/xrt/bin/xbflash.qspi --qspips-flash --input /opt/xilinx/firmware/u30/gen3x4/base/data/BOOT_golden.BIN --offset 0x6000000 --bar-offset 0x10000 --card <BDF>

  #. Once the golden image has been properly relocated, you can revert the card to the golden image by following the :ref:`card recovery instructions <card-recovery>`.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
