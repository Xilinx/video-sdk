.. _ffmpeg-multiple-devices:

**********************************
Working with Multiple Devices
**********************************

By default (if no device identifier is specified) a job is submitted to device 0. When running large jobs or multiple jobs in parallel, device 0 is bound to run out of resources rapidly and additional jobs will error out due to insufficient resources. 

By using the :option:`-lxlnx_hwdev` and :option:`-xlnx_hwdev` options, the different components (decoder, scaler, encoder) of a job can be individually submitted to a specific device. This makes it easy and straightforward to leverage the entire video acceleration capacity of your system, regardless of the number of cards and devices. 

Consult the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` section for more details on how to work with multiple devices.


..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.