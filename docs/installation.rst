.. _installation-instructions:

####################################################
Xilinx Video SDK Installation Instructions 
####################################################

This page contains instructions on how to download the Xilinx Video SDK packages and install them on the supported Operating Systems:

- Ubuntu 20.04 (Kernel 5.4)
- Ubuntu 18.04 (Kernel 5.4)
- RHEL 7.8 (Kernel 4.9.184)

.. contents:: Table of Contents
    :local:
    :depth: 2

******************************************
Important Information and Prerequisites
******************************************

- Each Alveo U30 card has 2 PCIe Gen 3x4 compliant interfaces. The host sever must support PCIe bifurcation on each slot with a Alveo U30.

- The installation instructions require a bash shell. Make sure to be using one when installing the packages.

- The installation instructions provided should only be used with the OEM version of the Alveo U30 card. If you do not have an OEM card, or if you are unsure of which card version you have, please `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.

- If you need to upgrade a system with version 0.95.0 (Alpha) or older installed, please `contact Xilinx <https://github.com/Xilinx/video-sdk/issues>`_.


******************************************
Installation Instructions
******************************************

#. Download the packages by cloning this repository::

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

.. _end-of-installation:

You are now ready to use the Alveo U30 cards installed in your system. See the :doc:`FFmpeg tutorials page </examples/ffmpeg/tutorials>` to learn how to run jobs on your system.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.