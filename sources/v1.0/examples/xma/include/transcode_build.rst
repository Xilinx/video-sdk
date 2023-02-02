Transcoder Build Instructions
=============================

Export the XRT and XRM to build the transcoder application::

   source /opt/xilinx/xrt/setup.sh
   source /opt/xilinx/xrm/setup.sh

In the ``exampes/xma/transcode`` directory, do the following::

   make clean
   make

The resulting executable is placed in the ``build`` directory::

	./build/u30_xma_transcode

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
