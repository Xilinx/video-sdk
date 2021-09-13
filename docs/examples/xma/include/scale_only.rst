****************************************************
XMA Scaler App
****************************************************

The ``examples/xma/scale_only`` folder contains a standalone scaler application using the C APIs (XMA) of the |SDK| to interact with the Xilinx video accelerators.


﻿Scaler Build Instructions
====================================================

Export the XRT and XRM environment variables needed to build the scaler application::

   source /opt/xilinx/xrt/setup.sh
   source /opt/xilinx/xrm/setup.sh

In the ``exampes/xma/scale_only`` directory, do the following::

   make clean
   make

The resulting executable is placed in the ``build`` directory::

	./build/u30_xma_scale


﻿Scaler Test Instructions
====================================================

The scaler XMA application supports most of the scaler options supported by FFmpeg. The XMA application supports only NV12 semi-planar input and outputs NV12 scaled elementary stream.

Scaler Usage
----------------------------------------------------

Before running scaler application, source the xcdr environment::

  source /opt/xilinx/xcdr/setup.sh

Below is the list of all the options of the scaler app:

.. literalinclude:: ./include/scale_only_help.rst


Sample Scaler Command
----------------------------------------------------

Scale 1080p nv12 to 720p, 480p, 360p, and 240p nv12::

  ./u30_xma_scale -w 1920 -h 1080 -i ~/Kimono1_1920x1080_24.yuvNV12 -w 1280 -h \
        720 -o Kimono_1280x720.NV12 -w 852 -h 480 -o Kimono_852x480.NV12 -w \
        640 -h 360 -o Kimono_640x360.NV12 -w 480 -h 240 -o Kimono_480x240.NV12


Limitations of the Scaler App
====================================================

* The scaler supports nv12 format only
* There may be a performance degradation relative to ffmpeg with a high number of outputs/processes.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
