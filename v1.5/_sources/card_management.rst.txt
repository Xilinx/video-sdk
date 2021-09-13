#####################################################
Card Management
#####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::


*****************************************
Overview
*****************************************

The |SDK| builds on the Xilinx Runtime (XRT) and the Xilinx Resource Manager (XRM) to interface with Xilinx video acceleration cards. The |SDK| includes the ``xbutil``, ``xbmgmt`` and ``xrmadm`` command line tools for card installation, upgrade, and management.


xbutil and xbmgmt
=========================================

The Xilinx® Board Utility (``xbutil``) and the Xilinx® Board Management Utility (``xbmgmt``) are standalone command line tools used to query and administer Xilinx acceleration cards such as the Alveo U30. 

- ``xbutil`` is used to examine, identify, program, and validate the installed accelerator card(s). 
- ``xbmgmt`` is used to flash the card firmware, examine devices, and administer the installed accelerator card(s). Running this command requires sudo privileges.

.. note::
   The ``xbmgmt`` tool is not supported on |VT1| instances.

The ``xbutil`` and ``xbmgmt`` commands typically target one device at a time using a PCIe BDF (Bus:Device.Function) identifier. Each device has two BDFs: a User BDF and a Management BDF. The BDF notation works as follows:

- PCI Domain number, often padded using leading zeros to four digits
- A colon (:)
- PCI Bus number in hexadecimal, often padded using a leading zeros to two or four digits
- A colon (:)
- PCI Device number in hexadecimal, often padded using a leading zero to two digits . Sometimes this is also referred to as the slot number.
- A decimal point (.)
- PCI Function number in hexadecimal. Only this last digit will differ between the User BDF and the Management BDF.

The ``xbutil`` command expects the User BDF of the targeted device while the ``xbmgmt`` command expects the Management BDF of the targeted device.

.. The ``xbutil`` and ``xbmgmt`` commands are delivered as part of the Xilinx Runtime (XRT) package. The complete documentation for these commands can be found in the main XRT documentation:

.. - https://xilinx.github.io/XRT/master/html/xbutil.html
.. - https://xilinx.github.io/XRT/master/html/xbmgmt.html


xrmadm and xrmd
=========================================

The Xilinx® FPGA resource manager (XRM) is the software which manages the hardware accelerators available in the system. The XRM daemon (``xrmd``) is a background process supporting reservation, allocation, and release of hardware acceleration resources. The XRM ``xrmadm`` command line tool is used to interact with the XRM daemon (``xrmd``) in order to check status and generate resource utilization reports. 

For more details about the XRM commands specific to the |SDK| refer to the :ref:`XRM Command Reference Guide <xrm-reference>`.

|

***********************************
Card and Device Identifiers
***********************************

.. _device-bdf:

Device User BDF
===================================

The list of all installed Xilinx devices, including their User BDF is obtained with the ``xbutil examine`` command.

For example, the command below detected 4 devices and lists the Management BDF for each of them::

    $ xbutil examine

    ...

    Devices present
      [0000:e3:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:e2:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:22:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:21:00.1] : xilinx_u30_gen3x4_base_1 

The last device listed has a User BDF of 0000:21:00.1, which describes Domain 0, Bus 21, Device 00, Function 1. 


.. _device-management-bdf:

Device Management BDF
===================================

The list of all installed Xilinx devices, including their Management BDF is obtained with the ``xbmgmt examine`` command.

For example, the command below detected 4 devices and lists the Management BDF for each of them::

    $ xbmgmt examine

    ...

    Devices present
      [0000:e3:00.0] : xilinx_u30_gen3x4_base_1 
      [0000:e2:00.0] : xilinx_u30_gen3x4_base_1 
      [0000:22:00.0] : xilinx_u30_gen3x4_base_1 
      [0000:21:00.0] : xilinx_u30_gen3x4_base_1 

The last device listed has a Management BDF of 0000:21:00.0, which describes Domain 0, Bus 21, Device 00, Function 0. 

Note that only the last digit (PCI Function) will differ between the User BDF and the Management BDF.


.. _device-render-id:

Device renderID
===================================

You can look-up the renderID of a device by using its User BDF and running the following command::

    ls /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/drm

NOTE: The colons of the BDF identifier must be separated by a backslash.

.. rubric:: Example

- Assuming a device with User BDF 0000:e3:00.1::

    $ ls /sys/bus/pci/devices/0000\:e3\:00.1/drm 
    card4  renderD131

- The renderID of this device is 131


.. _device-xclmgmt-id:


Device xclmgmtID
===================================

You can look-up the xclmgmtID of a device by using its Management BDF and running the following command::

    cat /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/instance

NOTE: The colons of the BDF identifier must be separated by a backslash.

.. rubric:: Example

- Assuming a device with Management BDF 0000:e3:00.0::

    $ cat /sys/bus/pci/devices/0000\:e3\:00.0/instance
    58112

- The xclmgmtID of this device is 58112

|

.. _card-serial_number:

Card Serial Number
===================================

You can look-up the serial number of a card by using the User BDF of a device located on the card and running the following command::

    cat /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/xmc.u.<xxx>/serial_num

NOTE: The colons of the BDF identifier must be separated by a backslash and <xxx> is an integer number unique to each device. 


.. rubric:: Example

- Assuming a device with BDF 0000:e3:00.1::

    $ cat /sys/bus/pci/devices/0000\:e3\:00.1/xmc.u.19922947/serial_num 
    XFL1RT5PHT31

- The serial number of the card on which this device is located XFL1RT5PHT31

|

.. _examining-cards:

**************************************
Checking System Status 
**************************************

The ``xbutil examine`` commands provides useful details about your environment and can be used to ensure that your cards and devices are properly detected. In the example below, four devices are found and usable::

    $ xbutil examine

    System Configuration
      OS Name              : Linux
      Release              : 5.4.0-050400-generic
      Version              : #201911242031 SMP Mon Nov 25 01:35:10 UTC 2019
      Machine              : x86_64
      CPU Cores            : 32
      Memory               : 64078 MB
      Distribution         : Ubuntu 18.04.5 LTS
      GLIBC                : 2.27
      Model                : PowerEdge R7525

    XRT
      Version              : 2.10.43
      Branch               : u30
      Hash                 : 41f6054b2c19619ceceb07bf7b662e5003388d89
      Hash Date            : 2021-05-11 09:17:17
      XOCL                 : 2.10.43, 41f6054b2c19619ceceb07bf7b662e5003388d89
      XCLMGMT              : 2.10.43, 41f6054b2c19619ceceb07bf7b662e5003388d89

    Devices present
      [0000:e3:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:e2:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:22:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:21:00.1] : xilinx_u30_gen3x4_base_1 

|

.. _device-details:

**************************************
Inspecting Device Details
**************************************

The ``xbmgmt examine -d <Management BDF>`` commands provides additional details about the status and configuration of each Xilinx device installed. 

For example, for the device with Management BDF 0000:e3:00.0::

    $ sudo /opt/xilinx/xrt/bin/xbmgmt examine -d e3:00.0

    ----------------------------------------------
    1/1 [0000:e3:00.0] : xilinx_u30_gen3x4_base_1
    ----------------------------------------------
    Flash properties
      Type                 : qspi_ps_x2_single
      Serial Number        : XFL1RT5PHT31

    Flashable partitions running on FPGA
      Platform             : xilinx_u30_gen3x4_base_1
      SC Version           : 6.3.8(FIXED)
      Platform UUID        : 1B5FEB2A-91B6-818D-A3E8-D9867DE17DA0
      Interface UUID       : 937ED708-67CF-3350-BC06-304053F4293C

    Flashable partitions installed in system
      Platform             : xilinx_u30_gen3x4_base_1
      SC Version           : 6.3.8
      Platform UUID        : 1B5FEB2A-91B6-818D-A3E8-D9867DE17DA0

|

.. _xbutil-validate:

*******************************
Checking and Validating Devices
*******************************

The ``xbutil validate`` command can be used to check and validate the health of the devices on your Alveo U30 cards by executing a special built-in test. In order to run this command, the XRM daemon must first be stopped, otherwise the test will error with ``xclbin on card is in use`` and ``failed to load xclbin`` messages.

#. List the User BDFs of each of the devices on all the Alveo U30 cards installed in your system::

    xbutil examine

#. Stop the XRM daemon::

    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh

#. For each of the devices, run the self-checking test::
    
    xbutil validate --device <User BDF>

#. After all tests have been run, restart the XRM daemon and reload the environment::

    sudo /opt/xilinx/xrm/tools/start_xrmd.sh
    source /opt/xilinx/xcdr/setup.sh

|

.. _mapping-devices-to-cards:

******************************
Mapping Devices to Cards
******************************

Knowing which devices are on which card is useful to selectively reset or recover a card. This is done by mapping the BDFs of the devices to the serial number of the cards as explained in the instructions below:

#. List the User BDFs of each of the devices on all the Alveo U30 cards installed in your system::

    xbutil examine

#. For each User BDF, look-up the corresponding serial number::

    cat /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/xmc.u.<xxx>/serial_num

   The colons of the BDF identifier must be separated by a backslash and <xxx> is an integer number unique to each device. For example, for the device with BDF 0000:e3:00.1::

    $ cat /sys/bus/pci/devices/0000\:e3\:00.1/xmc.u.19922947/serial_num 
    XFL1RT5PHT31

#. Note the serial number associated with each BDF. Two devices with the same serial number are on the same card. 


|

.. _managing-resources:

**********************************
Managing Resource Utilization
**********************************

Given that each device has a 2160p60 (4K) input and output bandwidth limit, the user is responsible for only submitting jobs which will not exceed the capacity of the specified device. This section provides information on how to estimate CU requirements and check current device load.

.. rubric:: Checking System Load

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

.. rubric:: Insufficient Resources

If there are not enough compute unit resources available on the device to support a FFmpeg job, the job will error out with a message about resource allocation failure::

    xrm_allocation: failed to allocate decoder resources from device 0
    [MPSOC HEVC decoder @ 0x562c7695b200] xrm_allocation: resource allocation failed

In this case, you can check the system load (as described in the section below) and look for a device with enough free resources, or wait until another job finishes and releases enough resources to run the desired job.


.. rubric:: Job Resource Requirements

The load of a given job can be estimated by taking the resolution of the job as a percentage of the 2160p60 (4K) maximum. For instance, a 1080p60 stream will require 25% of the resources available on a device.

In addition, it is possible to run FFmpeg with the :option:`-loglevel` option set to ``debug`` to get information about the resource requirements for a given job. The messages generated in the transcript will look as follow::

  ---decoder xrm out: dec_load=250000, plugin=/opt/xilinx/xma_plugins/libvcu-xma-dec-plg.so, device=0, cu=6, ch=0
  ---encoder xrm out: enc_load=250000, plugin=/opt/xilinx/xma_plugins/libvcu-xma-enc-plg.so, device=0, cu=38, ch=0

Resource loads are reported with a precision of 1/1000000. In the above example, the job requires 25% of the decoder resources and 25% of the encoder resources on the device.

|

.. _card-reset:

******************************
Resetting a Card
******************************

.. note::
   This action is only available on-premises and is not supported on |VT1| instances.

Resetting an Alveo U30 card is done with the ``xbutil reset --device <BDF>`` command, where BDF identifies one of the two devices on the card to be reset. If you need to identify which two devices are on a given card in order reset only these two devices, refer to the :ref:`device-to-card mapping <mapping-devices-to-cards>` instructions.


.. rubric:: IMPORTANT 

- The command will reset the device specified with the User BDF **as well as all other devices present on the same Xilinx card**. It is not possible to reset only one device. 

- Since ``xbutil reset`` resets all devices of a given card, Xilinx recommends against assigning a single device to a containerized application.

- ``xbutil reset`` will not work within containers or virtual machines when the management functions are not opened to the user.



#. Verify that all jobs running on both devices of the card can be safely interrupted.

#. List the User BDFs of each of the devices on all the Alveo U30 cards installed in your system::

    xbutil examine

#. Reset one of the two devices on the card which needs to be reset. This command will reset **both** devices on that card, not just the specified device::

    xbutil reset --device <BDF>  

#. The resetted cards and devices will not be readily usable. The hardware binaries must be reloaded on the devices before running new jobs::

    source /opt/xilinx/xcdr/setup.sh

|

.. _card-recovery:

*******************************
Recovering a Card
*******************************

.. note::
   This action is only available on-premises and is not supported on |VT1| instances.

In the event that your card has become corrupted and that a reset is not sufficient, you will need to recover it. This is normally be done in-band via a set of simple commands. Should the card's flash devices become corrupted to the point where it is no longer detected by PCIe, out-of-band recovery with a JTAG cable may be required.

.. _standard-recovery-flow:

Standard Recovery Flow
==================================

.. note::
   The instructions below assume that you are trying to recover a system which was flashed with release 0.96.0 or newer. If this is not the case, `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

#. Set up your environment:: 

    source /opt/xilinx/xrt/setup.sh

#. List the Management BDF of your devices::

    sudo /opt/xilinx/xrt/bin/xbmgmt examine

   NOTE: There are two devices per Alveo U30 card. If you want to identify which two devices are on a given card in order recover only these two devices, refer to the :ref:`device-to-card mapping <mapping-devices-to-cards>` instructions.

#. For each device which needs to be recovered, run the following command to flash it with the golden image::

    sudo /opt/xilinx/xrt/bin/xbmgmt program --revert-to-golden --device <BDF>

   NOTE: You may receive an error indicating ``Factory reset not supported. No Golden image found on flash.``. This is a known issue which is permanently resolved by following these instructions: :ref:`reflashing the golden image<flashing-the-golden-image>`.
   
#. Cold boot the machine when all desired devices have been reverted to factory settings.


.. _advanced-recovery-flow:

Advanced Recovery Flow
==================================

Should there be an issue with the standard in-band recovery process, it is still possible to recover the card using out-of-band methods. Please `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_ for more details.



..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
