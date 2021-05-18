#####################################################
Card Management and Recovery
#####################################################

.. contents:: Table of Contents
    :local:
    :depth: 1

.. _bdf-identifiers:

******************************
Device BDF Identifiers
******************************

There are two devices per Alveo U30 card. Each device is described by PCIe BDF (Bus:Device.Function) identifiers. BDF identifiers are used to target a specific device using the card management tools. 

The BDF notation works as follows:

- PCI Domain number, often padded using leading zeros to four digits
- A colon (:)
- PCI Bus number in hexadecimal, often padded using a leading zeros to two or four digits
- A colon (:)
- PCI Device number in hexadecimal, often padded using a leading zero to two digits . Sometimes this is also referred to as the slot number.
- A decimal point (.)
- PCI Function number in hexadecimal

Each device has two BDFs: a Management BDF and a User BDF. Only the last digit (PCI Function) will differ between the User BDF and the Management BDF.

- When using the ``xbutil`` command, the User BDF of the targeted device should be used. The User BDFs can be listed using the ``xbutil examine`` command. 

- When using the ``xbmgmt`` command, the Management BDF of the targeted device should be used. The Management BDFs can be listed using the ``xbmgmt examine`` command. Running the ``xbmgmt`` command requires sudo privileges.


For example, the command below detected 4 devices and lists the User BDF for each of them. The last device listed has a User BDF of 0000:21:00.1, which describes Domain 0, Bus 21, Device 00, Function 1. ::

    $ xbutil examine

    ...

    Devices present
      [0000:e3:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:e2:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:22:00.1] : xilinx_u30_gen3x4_base_1 
      [0000:21:00.1] : xilinx_u30_gen3x4_base_1 


|

.. _examining-cards:

**************************************
Ensuring Cards are Detected 
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

.. _xbutil-validate:

*******************************
Checking and Validating Devices
*******************************

The ``xbutil validate`` command can be used to check and validate the status of the devices on your Alveo U30 cards by executing a special built-in test. In order to run this command, the XRM daemon must first be stopped, otherwise the test will error with ``xclbin on card is in use`` and ``failed to load xclbin`` messages.

#. List the User BDFs of each of the devices on all the Alveo U30 cards installed in your system::

    xbutil examine

#. Stop the XRM daemon::

    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh

#. For each of the devices, run the self-checking test::
    
    xbutil validate --device <BDF>

#. After all tests have been run, reload the Xilinx Video SDK environment::

    sudo /opt/xilinx/xcdr/setup.sh

|

.. _mapping-devices-to-cards:

******************************
Mapping Devices to Cards
******************************

Knowing which devices are on which card is useful to selectively reset or recover a card. Follow these instructions to determine the mapping of devices to cards:

#. List the Management BDFs of each of the devices on all the Alveo U30 cards installed in your system::

    sudo /opt/xilinx/xrt/bin/xbmgmt examine

#. For each device in your system, gather status information::  

    sudo /opt/xilinx/xrt/bin/xbmgmt examine --device <BDF>

   The command will generate a report similar to the one below::

    ----------------------------------------------
    1/1 [0000:e3:00.0] : xilinx_u30_gen3x4_base_1
    ----------------------------------------------
    Flash properties
      Type                 : qspi_ps_x2_single
      Serial Number        : XFL1RT5PHT31

    Flashable partitions running on FPGA
      Platform             : xilinx_u30_gen3x4_base_1
      SC Version           : 6.3.7(FIXED)
      Platform UUID        : 323002F5-4D79-9C04-786D-B52BE50C3DAE
      Interface UUID       : 937ED708-67CF-3350-BC06-304053F4293C

    Flashable partitions installed in system
      Platform             : xilinx_u30_gen3x4_base_1
      SC Version           : 6.3.7
      Platform UUID        : 323002F5-4D79-9C04-786D-B52BE50C3DAE

#. For each device, note the BDF and the Serial Number. Two devices with the same serial number are on the same card. 

|

.. _card-reset:

******************************
Resetting a Card
******************************

Resetting an Alveo U30 card is done with the ``xbutil reset --device <BDF>`` command, where BDF identifies one of the two devices on the card to be reset. **IMPORTANT:** Even if the command takes the BDF of a single device, the command will always reset both devices on the Alveo U30 card. It is not possible to reset only one device. 

If you need to identify which two devices are on a given card in order reset only these two devices, refer to the :ref:`device-to-card mapping <mapping-devices-to-cards>` instructions.

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

|

*****************************************
Card Management Tools Reference Guide
*****************************************

The Xilinx Video SDK builds on the Xilinx Runtime (XRT) and the Xilinx Resource Manager (XRM) to interface with the Alveo U30 cards. The Xilinx Video SDK includes the ``xbutil``, ``xbmgmt`` and ``xrmadm`` command line tools for card installation, upgrade, and management.


xbutil and xbmgmt Commands
======================================

The Xilinx® Board Utility (``xbutil``) and the Xilinx® Board Management Utility (``xbmgmt``) are standalone command line tools used to query and administer Xilinx accelerator cards such as the Alveo U30. 

- ``xbutil`` is used to examine, identify, program, and validate the installed accelerator card(s). 
- ``xbmgmt`` is used to flash the card firmware, examine devices, and administer the installed accelerator card(s). It requires sudo privileges when running it. 

The ``xbutil`` and ``xbmgmt`` commands typically target one device at a time. The targeted device is specified using a PCIe BDF (Bus:Device.Function) identifier. For more details about BDF identifiers, refer to the :ref:`Device BDF Identifiers <bdf-identifiers>` section above.

The ``xbutil`` and ``xbmgmt`` commands are delivered as part of the Xilinx Runtime (XRT) package. The complete documentation for these commands can be found in the main XRT documentation:

- `xbutil <https://xilinx.github.io/XRT/master/html/xbutil2.html>`_
- `xbmgmt <https://xilinx.github.io/XRT/master/html/xbmgmt2.html>`_

**Note:** Do not set the XRT_TOOLS_NEXTGEN environment variable mentioned on those pages.


.. rubric:: Migrating from legacy versions of the XRT tools

This release of the Xilinx Video SDK includes both the legacy version and the new generation version of the ``xbutil`` and ``xbmgmt`` card management tools. The legacy version is included for backwards compatibility purposes only. Xilinx recommends using the new XRT commands when creating scripts or executing card management operations.

To help migrating from the legacy commands to the new commands `the following page <https://xilinx.github.io/XRT/master/html/xbtools_map.html>`_ provides tables mapping the legacy options to the new options of the ``xbutil`` and ``xbmgmt`` tools.


.. _xrmadm-and-xrmd-commands:

xrmadm and xrmd Commands
==================================

The Xilinx® FPGA resource manager (XRM) is the software which manages the hardware accelerators available in the system. The XRM daemon (``xrmd``) is a background process supporting reservation, allocation, and release of hardware acceleration resources. The XRM ``xrmadm`` command line tool is used to interact with the XRM daemon (``xrmd``). 

The ``xrmadm`` command provides the following capabilities and uses a JSON file as input for each action:

- Generate status reports for each device
- Load and unload the hardware accelerators
- Load and unload the software plugins


The XRM related files are installed under ``/opt/xilinx/xrm/`` and U30-specific XRM commands are available at ``/opt/xilinx/xcdr/scripts/xrm_commands/``.


Default Setup
---------------------------------------

Sourcing the ``/opt/xilinx/xcdr/setup.sh`` script to set up the environment takes care of setting up XRM:

- The XRM daemon (``xrmd``) is started 
- The hardware accelerators (xclbin) and software plugins are loaded on the Alveo U30 cards


Generating Status Reports
---------------------------------------

``xrmadm`` can generate reports with the status of each device in the system. This capability is particularly useful to check the loading of each hardware accelerator.

To generate a report for all the devices in the system::

  xrmadm /opt/xilinx/xrm/test/list_cmd.json


To generate a report for a single device specified in the json file::

  xrmadm /opt/xilinx/xrm/test/list_onedevice.json


A sample JSON file for generating a report for device 0 is shown below::

    {
        "request": {
            "name": "list",
            "requestId": 1,
            "device": 0
        }
    }


Loading/Unloading Hardware Accelerators
---------------------------------------

``xrmadm`` can be used to load or unload the hardware accelerators on the programmable devices of the Alveo U30 card. The hardware accelerators must be reloaded after rebooting a card.

To load the hardware accelerators on a given device::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/load_multiple_devices/load_device0_cmd.json

To unload the hardware accelerators from a given device::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/unload_multiple_devices/unload_device_0_cmd.json

A sample JSON file for loading two devices (0 and 1) is shown below::

    {
        "request": {
            "name": "load",
            "requestId": 1,
            "parameters": [
                {
                "device": 0,
                "xclbin": "/opt/xilinx/xcdr/xclbins/transcode.xclbin"
                },
                {
                "device": 1,
                "xclbin": "/opt/xilinx/xcdr/xclbins/transcode.xclbin"
                }
            ]
        }
    }


Loading/Unloading Software Plugins
---------------------------------------

``xrmadm`` can be used to load or unload the software plugins required to manage the compute resources. The software plugins perform resource management functions such as calculating CU load and CU max capacity. Once a plugin is loaded, it becomes usable by a host application through the XRM APIs. The XRM plugins need to be loaded before executing an application (such as FFmpeg) which relies on the plugins.

To load the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/load_multi_u30_xrm_plugins_cmd.json


To unload the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/unload_multi_u30_xrm_plugins_cmd.json


Controlling the xrmd Daemon
---------------------------------------
The following commands can be used to start, stop, restart, or get the status of the daemon::

    sudo /opt/xilinx/xrm/tools/start_xrmd.sh
    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh
    sudo /opt/xilinx/xrm/tools/restart_xrmd.sh
    sudo systemctl status xrmd


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
