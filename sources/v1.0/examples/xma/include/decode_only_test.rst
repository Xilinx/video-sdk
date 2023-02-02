
Decoder Test Instructions
=========================

The XMA decoder application supports most of the decode options supported by FFmpeg. The XMA application supports only NV12 semi-planar input and outputs NV12 encoded elementary stream. It ingests an H.264 or H.265 encoded file and utilizes hardware acceleration to get the decoded output. 

Decoder Usage
-------------

Before running decoding application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh


Running the decoder app with the ``--help`` option will print the complete list of options::

  ./build/u30_xma_decode --help

.. code-block:: none

  This is a standalone xma decoder app. It ingests an h264 or h265 
  encoded file and utilizes hardware acceleration to get the decoded 
  output.

  Usage: 
          ./u30_xma_decode [options] -i <input-file> -c:v <codec-type> 
          [codec_options] -o <output-file>

  Arguments:
          --help                     Print this message and exit
          -log <level>               Specify the log level
          -d <device-id>             Specify a device on which to run.
                                     Default: 0

  Input Arguments:

          -stream_loop <loop-count>  Number of times to loop the input
                                     file
          -i <input-file>            Input file to be used

  Codec Arguments:

          -c:v <codec>               Specify H264 or H265 decoding. 
                                     (mpsoc_vcu_h264, mpsoc_vcu_hevc)
          -low_latency               Should low latency decoding be used
          -entropy_buf_cnt <count>   Specify number of internal entropy
                                     buffers. [2-10], default: 2
          -latency_logging           Log latency information to syslog
          -splitbuff_mode            Configure decoder in split/unsplit
                                     input buffer mode
          -frames <frame-count>      Number of frames to be processed.
          -o <file>                  File to which output is written.


Sample Decoder Command
----------------------

Decode an h264 encoded file::

  ./build/u30_xma_decode  -i ~/CSGO_1920x1080_5000kbps.264 -c:v mpsoc_vcu_h264 -o ./CSGO_1920x1080.NV12


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
