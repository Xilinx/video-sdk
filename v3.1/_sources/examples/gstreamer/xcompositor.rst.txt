#############################################
GStreamer Compositor Application
#############################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::


*********************************************
Introduction
*********************************************

The :program:`/opt/xilinx/vvas/bin/vvas_xcompositor` application is a command line utility which implements the GStreamer video transcoding pipeline described in the diagram below. This application expects four input video files 60(mp4 with H.264/H.265 or H.264/H.265 elementary stream). The resolution of each input cannot exceed 1080p. The application creates an output stream by first composing the four inputs in a 2x2 grid, then by adding an optional logo and lastly by inserting a time stamp.

This application is installed through the xilinx-alveo-u30-gstreamer package. If this package is not present on your system, install it by following the steps described in the :ref:`package feed <package-feed>` page.

.. image:: /images/gst_xcompositor.png
    :width: 1024
    :alt: GStreamer pipeline of the vvas_xcompositor application
    :align: center


*********************************************
Host System Requirement
*********************************************
* |SDK|
* GStreamer 1.16.2
* gst-plugins-good
* gst-plugins-base
* gst-plugins-bad
* gst-libav
* Opensource GStreamer plugins to be verified after installation of above packages:

  - compositor
  - gdkpixbufoverlay
  - timeoverlay
  - h264parse
  - qtdemux
  - qtmux

*********************************************
Usage
*********************************************

============== ===========================
Parameter      Description
============== ===========================
-f             | mp4/elementary-stream file path. Resolutions up to 1080p are supported. 
               | Type:    string  
               | Range:   NA  
               | Default: NA  
               | Option:  Mandatory 
-l             | png/jpg logo file path. No logo will be overlayed if this option is not specified.
               | Type:    string  
               | Range:   NA  
               | Default: Null  
               | Option:  Optional 
-b             | Max bitrate for encoder in Kbps
               | Type:    int 
               | Range:   0 to 4294967295 
               | Default: 5000  
               | Option:  Optional 
-i             | device index
               | Type:    int 
               | Range:   0 to 31 
               | Default: 0 
               | Option:  Optional 
-h             | Horizontal offset of overlay logo in fractions of video image width, from top-left corner of video image (in relative positioning)
               | Type:    double  
               | Range:   0 to 1  
               | Default: 0.9 
               | Option:  Optional 
-v             | Vertical offset of overlay logo in fractions of video image height, from top-left corner of video image (in relative positioning)
               | Type:    double  
               | Range:   0 to 1  
               | Default: 0 
               | Option:  Optional 
-n             | Number of input buffers
               | Type:    integer 
               | Range:   20 to 2147483647  
               | Default: 2000  
               | Option:  Optional 
-o             | Output file name and path
               | Type:    String  
               | Range:   NA  
               | Default: ./result.mp4  
               | Option:  Optional 
============== ===========================
 


 

 
*********************************************
Examples
*********************************************
1. Composition along with logo on top right of video::

    vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265 -l ./logo.png

2. Composition with logo on top left of video::

  	vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265 -l ./logo.png -h 0 -v 0

3. Composition with logo on center of video::

  	vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265 -l ./logo.png -h 0.5 -v 0.5

4. Composition without logo layer::

  	vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265

5. Composition on device with index 2::

    vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265 -i 2

6. Composition on decoder soft kernel start index at 8::

    vvas_xcompositor -f ./video1.mp4 -f ./video2.mp4 -f ./video3.h264 -f ./video4.h265

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
