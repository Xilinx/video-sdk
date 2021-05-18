
.. _xrm-api-reference:

*************************************
XRM API Reference
*************************************

The Xilinx® FPGA Resource Manager (XRM) library is used to manage the hardware accelerators available in the system. XRM keeps track of total system capacity for each of the compute units such as the decoder, scaler, and encoder. 

The XRM library includes a daemon, a command line tool and a C application programming interface (API). Using the library API, external applications can communicate with the XRM daemon and perform actions such as reserving, allocating and releasing resources; calculating resource load and max capacity. 

More details on the XRM command line tool (xrmadm) and the XRM daemon (xrmd) can be found in the :ref:`xrmadm and xrmd Commands <xrmadm-and-xrmd-commands>` section of the documentation.

The XRM C APIs are defined in :file:`xrm.h`. The detailed description of these APIs can be found in the `XRM documentation <https://xilinx.github.io/XRM/lib.html>`_.

Applications integrating the Xilinx Video SDK plugins (such as the 4 example XMA Apps included in this repository) must use the XRM APIs for two kinds of tasks: 

- Device ID selection (required for XMA initialization)
- Resource allocation (required for XMA session creation)
 

Device ID Selection with XRM
============================

In order to initialize the XMA library, the ID of the device on which the job will run must be known.

- If the user provides the device ID, the application can directly perform XMA initialization based on this information. 

- If the user does not specify a device ID, the application needs to determine a valid one. It can do so by using the XRM API to calculate the load of the specific job which needs to be run, reserve resources based on the load, and retrieve the ID of the device on which the resources have been reserved. More specifically: 

  #. Calculate the channel load based on the job properties (extracted from command line arguments and/or header of the video stream)
  #. Using the :c:func:`xrmCheckCuPoolAvailableNum` function, query XRM for the number of resources available based on the channel load. XRM checks all the devices available on the server and returns how many such channels can be accommodated.
  #. Using the :c:func:`xrmCuPoolReserve` function, reserve the resources required for this channel load. XRM returns a reservation index.
  #. Using the :c:func:`xrmReservationQuery` function, obtain the ID of the device on which the resource has been allocated and the name of the xclbin.
  #. Initialize XMA using the device ID and the xclbin information.

For more details, refer to the example below which provides a reference implementation of the steps described above.

- :c:func:`transcode_device_init` function in :url_to_repo:`examples/xma/transcode/src/xlnx_transcoder_xrm_interface.c`


Resource Allocation with XRM
============================

In order to create an XMA plugin session (encoder/decoder/scaler/lookahead), the necessary the compute unit (CU) resources must first be successfully reserved and allocated.

- If the user provides device ID, the application should perform CU allocation on that particular device. If there are not enough resources available to support the specific channel load, the application should error out and exit.

- If the user does not specify a device ID, the application should perform CU allocation using the reservation index it received during XMA initialization. In this case, the CU allocation will not fail as it has already been reserved during XMA initialization.

For more details, refer to the examples below which provides a reference implementation of the steps described above.

- :c:func:`enc_cu_alloc` function in :url_to_repo:`examples/xma/transcode/src/xlnx_encoder.c`
- :c:func:`dec_cu_alloc` function in :url_to_repo:`examples/xma/transcode/src/xlnx_decoder.c`
- :c:func:`scaler_cu_alloc` function in :url_to_repo:`examples/xma/transcode/src/xlnx_scaler.c`
- :c:func:`la_allocate_xrm_cu` function in :url_to_repo:`examples/xma/transcode/src/xlnx_lookahead.c`


Reserving Multiple Job Slots
============================

Another example of XMR API usage can be found in the `source code of the job slot reservation application <https://github.com/Xilinx/app-jobslot-reservation-xrm/tree/master/jobSlot_reservation.cpp>`_. This example shows how to reserve as many job slots as possible given an input job description. This example works as follows:

#. Calculate the channel load based on a JSON job description
#. Using the :c:func:`xrmCheckCuPoolAvailableNum` function, query XRM for the number of resources available based on the channel load. XRM checks all the devices available on the server and returns how many such channels can be accommodated.
#. Call the :c:func:`xrmCuPoolReserve` function as many times as there are available resources to reserve all of them. 
#. Wait for the user to press Enter to relinquish the reserved resources using :c:func:`xrmCuPoolRelinquish`.

This example can be used as a starting point for developing custom orchestration layers.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.