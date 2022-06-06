
.. _manual-resource-management:


**********************************
Manual Resource Management
**********************************

The card management tools included in the |SDK| provide ways to query the status and utilization of the compute resources on the Xilinx devices. Using these tools the user can determine which resources are available and thereby determine on which device to submit a job (using :ref:`explicit device identifies <using-explicit-device-ids>`, as explained in the previous section).

Given that each device has a 2160p60 (4K) input and output bandwidth limit, the user is responsible for only submitting jobs which will not exceed the capacity of the specified device. If a job is submitted on a device where there are not enough compute unit resources available to support the job, the job will error out with a message about resource allocation failure.

The XRM and card management tools provide methods to estimate CU requirements and check current device load.

Checking System Load
====================

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

Insufficient Resources
======================

If there are not enough compute unit resources available on the device to support a FFmpeg job, the job will error out with a message about resource allocation failure::

    xrm_allocation: failed to allocate decoder resources from device 0
    [MPSOC HEVC decoder @ 0x562c7695b200] xrm_allocation: resource allocation failed

In this case, you can check the system load (as described in the section below) and look for a device with enough free resources, or wait until another job finishes and releases enough resources to run the desired job.


Job Resource Requirements
=========================

The load of a given job can be estimated by taking the resolution of the job as a percentage of the 2160p60 (4K) maximum. For instance, a 1080p60 stream will require 25% of the resources available on a device.

In addition, it is possible to run FFmpeg with the :option:`-loglevel` option set to ``debug`` to get information about the resource requirements for a given job. The messages generated in the transcript will look as follow::

  ---decoder xrm out: dec_load=250000, plugin=/opt/xilinx/xma_plugins/libvcu-xma-dec-plg.so, device=0, cu=6, ch=0
  ---encoder xrm out: enc_load=250000, plugin=/opt/xilinx/xma_plugins/libvcu-xma-enc-plg.so, device=0, cu=38, ch=0

Resource loads are reported with a precision of 1/1000000. In the above example, the job requires 25% of the decoder resources and 25% of the encoder resources on the device.





..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.