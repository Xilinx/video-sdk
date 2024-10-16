
To check the current loading of all the devices in your system, use the following command::

    xrmadm /opt/xilinx/xrm/test/list_cmd.json

This will generate a report in JSON format containing the load information for all the compute unit (CU) resources. The report contains a section for each device in the system. The device sections contain sub-sections for each of the CUs (decoder, scaler, lookahead, encoder) in that device. For example, the load information for the encoder on device 0 may look as follows:: 

    "device_0": {
        ...
        "cu_4": {
            "cuId         ": "4",
            "cuType       ": "IP Kernel",
            "kernelName   ": "encoder",
            "kernelAlias  ": "ENCODER_MPSOC",
            "instanceName ": "encoder_1",
            "cuName       ": "encoder:encoder_1",
            "kernelPlugin ": "/opt/xilinx/xma_plugins/libvcu-xma-enc-plg.so",
            "maxCapacity  ": "497664000",
            "numChanInuse ": "20",
            "usedLoad     ": "831472 of 1000000",
            "reservedLoad ": "0 of 1000000",
            "resrvUsedLoad": "0 of 1000000"
        },


The ``usedLoad`` value indicates how much of that resource is currently being used. The value will range from 0 (nothing running) to 1000000 (fully loaded). The ``reservedLoad`` value indicates how much of that resource is being reserved using XRM. The ``resrvUsedLoad`` value indicates how much of the reserved load is actually being used.

In the above example, the encoder is 83.14% utilized. An additional job may only be run on this device if it requires less than 17% of the encoder resources.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.