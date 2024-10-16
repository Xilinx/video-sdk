
****************************************************
XMA Decoder App
****************************************************

Decoder Test Instructions
====================================================

The XMA decoder application supports most of the decode options supported by FFmpeg. The XMA decoder application supports only elementary H.264 or HEVC encoded bitstream and outputs YUV. 

Decoder Usage
----------------------------------------------------

Before running decoding application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the decoder app with the ``--help`` option will print the complete list of options::

  u30_xma_decode --help

.. literalinclude:: ./include/decoder_help.rst


Sample Decoder Command
----------------------------------------------------

Decode an h264 encoded file::

  u30_xma_decode  -i ~/CSGO_1920x1080_5000kbps.264 -c:v mpsoc_vcu_h264 -o ./CSGO_1920x1080.NV12

For more examples, refer to the :url_to_xma_apps:`decoder/test/scripts/decoder_app_test.sh` script.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
