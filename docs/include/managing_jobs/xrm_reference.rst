.. _xrm-reference:
.. _xrmadm-and-xrmd-commands:

*****************************************
XRM Reference Guide
*****************************************

The Xilinx® FPGA resource manager (XRM) is the software which manages the hardware accelerators available in the system. XRM includes the following components:

- ``xrmd``: the XRM daemon, a background process supporting reservation, allocation, and release of hardware acceleration resources. 
- ``xrmadm`` the command line tool is used to interact with the XRM daemon (``xrmd``). 
- a C Application Programming Interface (API)

Command Line Interface
=========================================

The XRM ``xrmadm`` command line tool is used to interact with the XRM daemon (``xrmd``). It provides the following capabilities and uses a JSON file as input for each action:

- Generate status reports for each device
- Load and unload the hardware accelerators
- Load and unload the software plugins

The XRM related files are installed under ``/opt/xilinx/xrm/`` and device-specific XRM commands are available at ``/opt/xilinx/xcdr/scripts/xrm_commands/``.



Setup
-----------------------------------------

When sourced, the ``/opt/xilinx/xcdr/setup.sh`` script takes care of setting up the enviroment for the |SDK|, including its XRM components:

- The XRM daemon (``xrmd``) is started 
- The hardware accelerators (xclbin) and software plugins are loaded on the Xilinx devices


Generating Status Reports
-----------------------------------------

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
-----------------------------------------

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
-----------------------------------------

``xrmadm`` can be used to load or unload the software plugins required to manage the compute resources. The software plugins perform resource management functions such as calculating CU load and CU max capacity. Once a plugin is loaded, it becomes usable by a host application through the XRM APIs. The XRM plugins need to be loaded before executing an application (such as FFmpeg/GStreamer) which relies on the plugins.

To load the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/load_multi_u30_xrm_plugins_cmd.json


To unload the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/unload_multi_u30_xrm_plugins_cmd.json


Controlling the xrmd Daemon
-----------------------------------------
The following commands can be used to start, stop, restart, or get the status of the daemon::

    sudo /opt/xilinx/xrm/tools/start_xrmd.sh
    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh
    sudo /opt/xilinx/xrm/tools/restart_xrmd.sh
    sudo systemctl status xrmd


C Application Programming Interface
=========================================

XRM provides a C Application Programming Interface (API) to reserve, allocate and release CUs from within a custom application. For complete details about this programming interface, refer to the :ref:`XRM API Reference Guide <xrm-api-reference>` section of the documentation. 




..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.