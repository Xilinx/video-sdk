.. _installation-instructions:

####################################################
Getting Started with Alveo U30 on Premises
####################################################

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

- If you need to upgrade a system with version 0.95.0 (Alpha) or older installed, please `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

|

******************************************
Install the |SDK|
******************************************

#. Download the packages included in this repository::

    git clone https://github.com/Xilinx/video-sdk

#. Navigate to the directory containing the packages corresponding to your Operating System::

    cd ./video-sdk/release/<os>

#. Install the software packages::

    ./install.sh

#. Program the binary images in the nonvolatile flash memories of the devices on your Alveo U30 cards::

    sudo ./u30flashall.sh

#. Cold boot the machine to have the cards use the new binaries::

    sudo shutdown now

#. After the cold boot is complete, check if all the cards are up to date::

    cd ./video-sdk/release/<os>
    sudo ./u30flashall.sh

   + If all cards are up to date, the script will report so and terminate. You can proceed to the next step. 
   + Otherwise, you will see one or more messages indicating "Actions to perform: Program Satellite Controller (SC) image" and the script will perform the required Satellite Controller updates. After all updates are completed, perform a warm reboot of the machine.

#. Test that the installation was successful with the command below. This script will run a validation test on each of the devices in your system. For each of the tests, you should see a message indicating "Validated successfully [1 device(s)]"::

    ./u30validateall.sh

|

******************************************
Set Up the Runtime Environment
******************************************

Note: These steps below should be performed each time you open a new terminal on your system.

#. Set up the runtime environment::

    source /opt/xilinx/xcdr/setup.sh

   + At the prompt, enter the root password to start the :ref:`Xilinx Resource Manager (XRM) daemon <xrmadm-and-xrmd-commands>`. 
   + The script finishes with messages indicating that the devices and XRM plugins have been successfully loaded.

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
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.