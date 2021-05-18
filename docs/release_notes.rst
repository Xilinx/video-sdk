########################################
Release Notes
########################################

.. note::
    Version: |release|

.. contents:: Table of Contents
    :local:
    :depth: 1

Summary
===========

This is first production release of the Xilinx Video SDK, a complete solution for high-density real-time transcoding supporting:

- Seamless integration with FFmpeg

- Simultaneous decoding, scaling and encoding of up to 46 streams with a maximum aggregated bandwidth of two 3840x2160\@60 fps per card

- Video resolutions from 128x128 to 3840x2160

For a complete list of features and performance tables, refer to the :doc:`Specs and Features </introduction>` page of the documentation

|

Limitations
===========

- The scaler is tuned for downscaling and expects non-increasing resolutions in an ABR ladder. Increasing resolutions between outputs will reduce video quality. For more details see :ref:`Setting up an ABR Ladder <setting-up-an-abr-ladder>`.

- The current Alveo U30 solution is targeted for live video real-time transcoding. Even though a single Alveo U30 card has the capability of running two Ultra HD (4K) transcodes in real-time at 60 frames per second (2x4kp60), running a single 4k stream at 120 fps is not supported. Similarly, a single Alveo U30 card supports up to 8x1080p60 transcodes in real-time, but it does not support fewer streams running at 120fps or beyond.

- When processing 4x 1080p60 streams, the scaler can only sustain a rate of 60fps for up to 5 outputs. Therefore, given 4 1080p60 input streams, real-time performance is only possible for up to 20 scaled outputs.

- The maximum bit-rate tested is 35 Mb/s; if the bit-rate is higher, the encoder might not work properly.

- ABR video transcode use cases have been tested with up to 5 output renditions per input for a maximum supported number of channels of 8 running real-time per card. 

- The lookahead feature is not supported when encoding streams to a 4k resolution.

|

Known Issues
============


Multiple ``Staring new cluster`` messages when encoding to Matroska
---------------------------------------------------------------------
When encoding to .mkv, FFmpeg may have issues synchronizing video and audio data leading to messages of the form::

    [matroska @ 0x563b3f675100] Starting new cluster

- Solution: Pass ``-max_interleave_delta 0`` on the FFmpeg command line


Streams with rotation metadata may cause a segfault while transcoding
-----------------------------------------------------------------------------------------

The hardware transcode pipeline can be unstable on a stream with rotation metadata

- Solution: Use the ``-noautorotate`` option in cases where rotation metadata is present.

  - Change ``ffmpeg -c:v mpsoc_vcu_h264 -i $i -c:v mpsoc_vcu_h264 -f mp4 -y out.mp4``
  - to ``ffmpeg -noautorotate -c:v mpsoc_vcu_h264 -i $i -c:v mpsoc_vcu_h264 -f mp4 -y out.mp4``
  

The ``--revert-to-golden`` and ``--factory_reset`` commands may not work by default
-----------------------------------------------------------------------------------------

The ``xbmgmt program --revert-to-golden`` and ``xbmgmt flash --factory_reset`` command expects the golden image of the card to be flashed at a particular memory location. If this is not the case, the command will not work. 

- Solution: Relocate the golden image by following the instructions below.  

.. _flashing-the-golden-image:

#. To reflash the golden image, your system must be already configured with release 0.96.0 or newer. If this is not the case, `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

#. Set up your environment:: 

    source /opt/xilinx/xrt/setup.sh

#. List the Management BDF of your devices::

    sudo /opt/xilinx/xrt/bin/xbmgmt examine

#. For each device, run the following command to flash the golden image to the expected memory location::

    sudo /opt/xilinx/xrt/bin/xbflash.qspi --qspips-flash --input /opt/xilinx/firmware/u30/gen3x4/base/data/BOOT_golden.BIN  --offset 0x6000000 --bar-offset 0x10000 --card <BDF>

#. Once the golden image has been properly relocated, you can revert the card to the golden image by following the :ref:`card recovery instructions <card-recovery>`.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
