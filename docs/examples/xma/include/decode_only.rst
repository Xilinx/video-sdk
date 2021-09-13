
****************************************************
XMA Decoder App
****************************************************

The ``examples/xma/decode_only`` folder contains a standalone decoder application using the C APIs (XMA) of the |SDK| to interact with the Xilinx video accelerators.

Decoder Build Instructions
====================================================

Export the XRT and XRM environment variables needed to build the decoder application::

   source /opt/xilinx/xrt/setup.sh
   source /opt/xilinx/xrm/setup.sh

In the ``exampes/xma/decode_only`` directory, do the following::

   make clean
   make

The resulting executable is placed in the ``build`` directory::

	./build/u30_xma_decode


Decoder Test Instructions
====================================================

The XMA decoder application supports most of the decode options supported by FFmpeg. The XMA application supports only NV12 semi-planar input and outputs NV12 encoded elementary stream. It ingests an H.264 or H.265 encoded file and utilizes hardware acceleration to get the decoded output. 

Decoder Usage
----------------------------------------------------

Before running decoding application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh


Running the decoder app with the ``--help`` option will print the complete list of options::

  ./build/u30_xma_decode --help

.. literalinclude:: ./include/decode_only_help.rst


Sample Decoder Command
----------------------------------------------------

Decode an h264 encoded file::

  ./build/u30_xma_decode  -i ~/CSGO_1920x1080_5000kbps.264 -c:v mpsoc_vcu_h264 -o ./CSGO_1920x1080.NV12


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
