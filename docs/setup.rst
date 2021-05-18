##############################
Environment Setup Instructions 
##############################

.. note::
   This page describes how to set up the runtime environment on a server with one or more Alveo U30 cards and the Xilinx Video SDK installed. If you have not installed the Xilinx Video SDK yet, please follow the :doc:`Installation Guide </installation>`. 

.. _environment-setup:

#. Setup the environment by sourcing the ``/opt/xilinx/xcdr/setup.sh`` script::

    $ source /opt/xilinx/xcdr/setup.sh
    ---------------------------------------
    -----Source Xilinx U30 setup files-----
    XILINX_XRT      : /opt/xilinx/xrt
    PATH            : /opt/xilinx/xrt/bin:/home/user/bin:/home/user/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
    LD_LIBRARY_PATH : /opt/xilinx/xrt/lib:
    PYTHONPATH     : /opt/xilinx/xrt/python:
    XILINX_XRM      : /opt/xilinx/xrm
    PATH            : /opt/xilinx/xrm/bin:/opt/xilinx/xrt/bin:/home/user/bin:/home/user/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
    LD_LIBRARY_PATH : /opt/xilinx/xrm/lib:/opt/xilinx/xrt/lib:
    -----Starting xrmd-----
    rm -f /dev/shm/xrm.data
    rm: cannot remove '/dev/shm/xrm.data': Operation not permitted
    systemctl start xrmd
    ==== AUTHENTICATING FOR org.freedesktop.systemd1.manage-units ===
    Authentication is required to start 'xrmd.service'.
    Authenticating as: user,,, (user)
    Password:   
   
#. You will be prompted to enter the root password to start the :ref:`Xilinx Resource Manager (XRM) daemon <xrmadm-and-xrmd-commands>`.

#. After entering the root password, you will see messages confirming that the devices and XRM plugins have been loaded::
    
    ==== AUTHENTICATION COMPLETE ===
    -----Load xclbin to devices-----
    {
        "response": {
            "name": "load",
            "requestId": "1",
            "status": "ok"
        }
    }
    
    -----Load xrm plugins-----
    {
        "response": {
            "name": "loadXrmPlugins",
            "requestId": "1",
            "status": "ok"
        }
    }
    
    ---------------------------------------


.. rubric:: Potential "failed" error code

If you source the setup script on a machine where the Xilinx Video SDK environment has already been initialized, you will see the "failed" status being reported. Note that this is **not an issue**. This status simply states that your environment is already set up and that the system is ready for use.
::
    
    -----Load xrm plugins-----
    {
        "response": {
            "name": "loadXrmPlugins",
            "requestId": "1",
            "status": "failed",
            "data": {
                "failed": "plugin xrmU30DecPlugin (version 2) is already loaded"
            }
        }
    }
    
    ---------------------------------------


.. rubric:: Ensuring Cards are Detected 
 
After setting up your environment, you can optionally use the included card management tools to :ref:`confirm that your cards and devices are properly detected <examining-cards>`.


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
