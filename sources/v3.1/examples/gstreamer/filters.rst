#########################################
GStreamer Examples using Software Filters
#########################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::


The ``examples/gstreamer/filters/`` folder contains different examples showing how to use GStreamer combining both Xilinx accelerated functions and software filters.

*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst


*****************
Video Rotation
*****************
- Example script: :url_to_examples:`examples/u30/gstreamer/filters/01_rotate_video.sh`

This example accepts an 8-bit, YUV420, pre-encoded h264 file and will send the rotated, encoded h.264 output to /tmp/xil_rotate_<N>.mp4 at a rate of 8Mbps.


*****************
Logo Overlay
*****************
- Example script: :url_to_examples:`examples/u30/gstreamer/filters/02_logo_overlay.sh`

This example accepts an 8-bit, YUV420, pre-encoded h264 file and an image file ("logo"). It will scale the logo to 500x100, place it 16 pixels right and 16 pixels down from the top-left corner of the output video file, which will be an encoded h.264 output saved to /tmp/xil_logo_overlay.mp4 at a rate of 8Mbps.


*****************
Crop and Zoom
*****************
- Example script: :url_to_examples:`examples/u30/gstreamer/filters/03_crop_zoom.sh`

This example accepts an 8-bit, YUV420, pre-encoded h264 file and will crop a 300x200 sized section of the original video. The section's top left corner begins at 20 pixels to the right, and 10 pixels down from the top-left corner of the original video. The output video is encoded in 8Mbps, and is saved to /tmp/xil_crop_zoom.mp4


*****************
Video Composition
*****************
- Example script: :url_to_examples:`examples/u30/gstreamer/filters/04_multi_comp.sh`

This example accepts 4x 8-bit, YUV420, pre-encoded h264 files of equal dimensions, and will create an output 2x2 composite video which will be an encoded h.264 output saved to /tmp/xil_2x2_composite.mp4 at a rate of 8Mbps. The output resolution will be equal to the original input.


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
