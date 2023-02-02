.. _installation-instructions:

####################################################
Getting Started with Alveo U30 on Premises
####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::


******************************************
Important Information and Prerequisites
******************************************

- The |SDK| must be installed on a server with one of more Xilinx Alveo U30 cards already installed.

- PCIe bifurcation must be enabled on each slot with an Alveo U30 card. Check the BIOS settings to make sure this is the case.

- The installation instructions require a bash shell. Make sure to be using one when installing the packages.

- The installation instructions provided should only be used with the OEM version of the Alveo U30 card. If you do not have an OEM card, or if you are unsure of which card version you have, please `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

|

******************************************
Install the |SDK|
******************************************

#. Open a bash shell

#. Disable automatic kernel updates on your system

#. Download the packages included in this repository::

    git clone https://github.com/Xilinx/video-sdk -b v2.0 --depth 1

#. Navigate to the directory containing the packages corresponding to your Operating System::

    cd ./video-sdk/release/<os>

#. Install the software packages::

    ./install

#. Program the binary images in the nonvolatile flash memories of the devices on your Alveo U30 cards::

    sudo /opt/xilinx/xrt/bin/xball --device-filter u30 xbmgmt program --base

   + Note: should any problem arise when running the command, revert the card to its golden image (as explained in the :ref:`card recovery instructions <standard-recovery-flow>`) and repeat this step.

#. Cold boot the machine to have the cards use the new binaries::

    sudo shutdown now

#. After the cold boot is complete, check if all the cards are up to date::

    sudo /opt/xilinx/xrt/bin/xball --device-filter u30 xbmgmt program --base

   + If all cards are up to date, the command will report so and terminate. You can proceed to the next step. 
   + Otherwise, you will see one or more messages indicating "Updating Satellite Controller (SC) firmware flash image" and the command will perform the required updates. After all updates are completed, perform a warm reboot of the machine.

#. Test that the installation was successful with the command below. This script will run a validation test on each of the devices in your system. For each of the devices, you should see a message indicating that the test was successful::

    source /opt/xilinx/xrt/setup.sh
    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh
    xball --device-filter u30 xbutil validate
    sudo /opt/xilinx/xrm/tools/restart_xrmd.sh

|

.. _runtime-setup:

******************************************
Set Up the Runtime Environment
******************************************

Note: These steps below should be performed each time you open a new terminal on your system.

#. Set up the runtime environment::

    source /opt/xilinx/xcdr/setup.sh

#. Optionally verify that the cards are correctly detected::

    xbutil examine

#. You are now ready to use the Xilinx video acceleration cards installed in your system. 

|

******************************************
Run Your First Examples
******************************************

See the :ref:`tutorials and examples <tutorials-and-examples>` page to learn how to run jobs on your system.
 

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
