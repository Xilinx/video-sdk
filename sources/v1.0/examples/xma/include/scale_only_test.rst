Scaler Test Instructions
========================

The scaler XMA application supports most of the scaler options supported by FFmpeg. The XMA application supports only NV12 semi-planar input and outputs NV12 scaled elementary stream.

Scaler Usage
------------

Before running scaler application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Below is the list of all the options of the scaler app:

.. code-block:: none

  This program ingests an NV12 input file and utilizes hardware acceleration to scale to various resolutions. 

  Usage:
          ./u30_xma_scale [options] -w <input-width> -h <input-height> -i 
          <input-file> [scaler_options] -w <output-1-width> -h 
          <output-1-height> -o <output-1-file> -w ...

  Arguments:
          --help                     Print this message and exit
          -log <level>               Specify the log level
          -d <device-id>             Specify a device on which to run.
                                     Default: 0

  Input Arguments:

          -stream_loop <loop-count>  Number of times to loop the input
                                     file
          -w <width>                 Specify the input's width
          -h <height>                Specify the input's height
          -pix_fmt <fmt>             Specify the input's pixel format.
                                     nv12 is the only format supported.
          -fps <frame-rate>          Frame rate. Used for scaler load
                                     calculation.
          -i <input-file>            Input file to be used

  Output Arguments:
          -coeff_load <load>         Specify the coefficient load. 0 Auto 
                                     (default), 1 static, 2 FilterCoef.txt.
          -enable-pipeline           Enable scaler pipeline
          -rate <half/full>          Set the rate to half. Half rate drops 
                                     frames to reduce resource usage. 
                                     Default: full.
          -enable-latency-logging    Enable latency logging
          -w <width>                 Specify the output's width
          -h <height>                Specify the output's height
          -frames <frame-count>      Number of frames to be processed.
          -o <file>                  File to which output is written.


Sample Scaler Command
---------------------

Scale 1080p nv12 to 720p, 480p, 360p, and 240p nv12::

  ./u30_xma_scale -w 1920 -h 1080 -i ~/Kimono1_1920x1080_24.yuvNV12 -w 1280 -h \
        720 -o Kimono_1280x720.NV12 -w 852 -h 480 -o Kimono_852x480.NV12 -w \
        640 -h 360 -o Kimono_640x360.NV12 -w 480 -h 240 -o Kimono_480x240.NV12


Limitations of the Scaler App
=============================

* The scaler supports nv12 format only
* There may be a performance degradation relative to ffmpeg with a high number of outputs/processes.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
