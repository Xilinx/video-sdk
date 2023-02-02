****************************************************
XMA Encoder App
****************************************************

The ``examples/xma/encode_only`` folder contains a standalone encoder application using the C APIs (XMA) of the |SDK| to interact with the Xilinx video accelerators.

﻿Encoder Build Instructions
====================================================

Export the XRT and XRM environment variables needed to build the encoder application::

   source /opt/xilinx/xrt/setup.sh
   source /opt/xilinx/xrm/setup.sh

In the ``exampes/xma/encode_only`` directory, do the following::

   make clean
   make

The resulting executable is placed in the ``build`` directory::

	./build/u30_xma_enc


﻿Encoder Test Instructions
====================================================

The encoder XMA application supports most of the encoder options supported by FFmpeg. The XMA application supports only YUV 4:2:0 semi-planar input and outputs H.264/HEVC encoded elementary stream.

Encoder Usage
----------------------------------------------------

Before running encoder application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Running the encoder app with the ``-help`` option will print the complete list of options::

  ./u30_xma_enc -help

.. literalinclude:: ./include/encode_only_help.rst


Sample Encoder Commands
----------------------------------------------------

H.264 encoding, best objective score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 0 -o out1.264

H.264 encoding, best visual score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 2 -level 42 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out2.264

HEVC encoding, best visual score, low latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out1.265

HEVC encoding, best visual score, normal latency::

  ./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 3 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o out2.265

For more examples, refer to the :url_to_xma_apps:`encode_only/test/scripts/enc_app_test.sh` script.


Limitations of the Encoder App
====================================================

* The encoder application assumes that the input YUV file is in NV12 format.
* The application outputs only H264/HEVC elementary streams.
* The application supports multi-process but not multi-channel.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
