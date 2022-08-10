
.. _xrm-api-reference:

*************************************
XRM API Reference
*************************************

.. note::
   Version 2.0 of the |SDK| uses XRM 2021.2 which provides new resource management APIs. These new APIs are identified with the "V2" suffix. They provide more flexible resource allocation capabilities and Xilinx recommends using these "V2" APIs for any new development and integration work. The original XRM APIs are still supported. Applications developped using the original APIs do not require any modification.

The Xilinx® FPGA Resource Manager (XRM) library is used to manage the hardware accelerators available in the system. XRM keeps track of total system capacity for each of the compute units such as the decoder, scaler, and encoder. 

The XRM library includes a daemon, a command line tool and a C application programming interface (API). Using the library API, external applications can communicate with the XRM daemon and perform actions such as reserving, allocating and releasing resources; calculating resource load and max capacity. 

More details on the XRM command line tool (xrmadm) and the XRM daemon (xrmd) can be found in the :ref:`XRM Reference Guide <xrm-reference>` section of the documentation.

The XRM C APIs are defined in :file:`xrm.h`. The detailed description of these APIs can be found in the `XRM documentation <https://xilinx.github.io/XRM/lib.html>`_. The XRM APIs listed below are the most commonly used to manage video acceleration resources:

- `xrmCheckCuPoolAvailableNumV2 <https://xilinx.github.io/XRM/lib.html#xrmcheckcupoolavailablenumv2>`_
- `xrmCuPoolReserveV2 <https://https://xilinx.github.io/XRM/lib.html#xrmcupoolreservev2>`_
- `xrmReservationQueryV2 <https://xilinx.github.io/XRM/lib.html#xrmreservationqueryv2>`_
- `xrmCuPoolRelinquishV2 <https://xilinx.github.io/XRM/lib.html#xrmcupoolrelinquishv2>`_
- `xrmCuAllocV2 <https://xilinx.github.io/XRM/lib.html#xrmcuallocv2>`_
- `xrmCuReleaseV2 <https://xilinx.github.io/XRM/lib.html#xrmcureleasev2>`_


Applications integrating the |SDK| plugins (such as the 4 example XMA Apps included in this repository) use the XRM APIs for two kinds of tasks: 

- Resource reservation (optional)
- Resource allocation (required)
 

Resource Reservation with XRM
=============================

The XRM library can be optionally be used to identify a device with enough available resources to run the desired job and reserve the corresponding resources. Doing involves three steps: 

#. Calculate the channel load based on the job properties
#. Using the :c:func:`xrmCheckCuPoolAvailableNumV2` function, query XRM for the number of resources available based on the channel load. XRM checks all the devices available on the server and returns how many such channels can be accommodated.
#. Using the :c:func:`xrmCuPoolReserveV2` function, reserve the resources required for this channel load. XRM returns a reservation index.

For an example of how to perform resource reservation using XRM APIs, refer the to the `source code of the job slot reservation application <https://github.com/Xilinx/app-jobslot-reservation-xrm/blob/U30_GA_2/jobSlot_reservation.cpp>`_ file. The job slot reservation tool reserves the maximum number of slots for a given job. This code can be adapted to reserve a single slot on one the device with enough resources for the job of interest.

Once resources have been reserved, it is also possible to use the :c:func:`xrmReservationQueryV2` API to obtain the ID of the device on which the resource has been allocated and the name of the xclbin. The device ID and xclbin information can then be used to initialize the XMA session.

Resources reserved with :c:func:`xrmCuPoolReserveV2` must be relinquished with the :c:func:`xrmCuPoolRelinquishV2` function once the application no longer needs them. 


Resource Allocation with XRM
============================

In order to create an XMA plugin session (encoder/decoder/scaler/lookahead), the necessary compute unit (CU) resources must first be successfully allocated with XRM using the :c:func:`xrmCuAllocV2` function (or :c:func:`xrmCuListAllocV2` to reserve multiple CUs at once).

Resources allocated with :c:func:`xrmCuAllocV2` (or :c:func:`xrmCuListAllocV2`) must be released with the :c:func:`xrmCuReleaseV2` function (or :c:func:`xrmCuListReleaseV2`) once the application no longer needs them. 

The resource allocation procedure is different depending on whether resources were previously reserved or not.


Allocation of Pre-Reserved Resources
------------------------------------

If resources were previously reserved using the :c:func:`xrmCuPoolReserveV2` function, the application should perform CU allocation using the device ID and the reservation ID obtained during the resource reservation process. In this case, CU allocation will not fail as it the necessary resources have already been reserved.

#. Create a :c:var:`xrmCuPoolReserveV2` data structure
#. Assign the reservation ID to the :c:var:`poolID` field of the :c:var:`xrmCuPoolReserveV2` data structure
#. If resources were reserved across multiple devices, assign the device ID of these specific resources to the :c:var:`deviceInfo` field of the :c:var:`xrmCuPoolReserveV2` data structure
#. Allocate the resources using the :c:func:`xrmCuAllocV2` function


Allocation of Non-Reserved Resources
------------------------------------

If resources were not previously reserved using the :c:func:`xrmCuPoolReserveV2` function, the application should first calculate the load of the current job and then attempt CU allocation for that particular load in a user-specified device. CU allocation will fail if there are not enough resources to support the specific channel load on that device.

#. Calculate the channel load based on the job properties
#. Create a :c:var:`xrmCuPoolReserveV2` data structure
#. Assign the resource load to the :c:var:`requestLoad` field of the :c:var:`xrmCuPoolReserveV2` data structure
#. Assign the user-specified device ID to the :c:var:`deviceInfo` field of the :c:var:`xrmCuPoolReserveV2` data structure
#. Allocate the resources using the :c:func:`xrmCuAllocV2` function

For a detailled example of how allocate non-reserved resources, refer to two following functions from the XMA sample applications:

- :c:func:`xlnx_xrm_load_calc` function in :url_to_repo:`examples/xma/common/src/xlnx_xrm_utils.c#L108`
- :c:func:`xlnx_xrm_cu_alloc` function in :url_to_repo:`examples/xma/common/src/xlnx_xrm_utils.c#L240`

The :c:func:`xlnx_xrm_load_calc` function calculates the resource load for the given job, and the :c:func:`xlnx_xrm_cu_alloc` function allocates the necessary resources in a specific device to support the given load.


..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.