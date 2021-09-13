.. _getting-started-on-vt1:

####################################################
Getting Started on |VT1|
####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1

****************************************************
|VT1| Configurations
****************************************************

The following table summarizes the number of Alveo U30 cards and Xilinx devices for each of the different instance sizes of the |VT1| family.
For more details about the transcoding performance of these different configurations, refer to the :ref:`performance tables <perf-tables>` in the :doc:`Specs and Features </specs_and_features>` page of this documentation. 

.. VT1 Instance Types and Configurations
.. list-table:: 
   :widths: 34 33 33
   :header-rows: 1

   * - Instance Type
     - Number of U30 Cards
     - Number of Transcode devices
   * - vt1.3xl
     - 1
     - 2
   * - vt1.6xl
     - 2
     - 4
   * - vt1.24xl
     - 8
     - 16

|

****************************************************
Quick Start Guide for |VT1|
****************************************************

.. DOCS_TODO: provide link to AMI once released

#. Launch an |VT1| Instance

   - Follow the instructions at `Launch an Amazon EC2 Instance <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html#ec2-launch-instance>`_

   - When choosing the instance type in the EC2 console, make sure to select the |VT1| instance family. 

   - When choosing an Amazon Machine Image (AMI), make sure to select the |SDK| AMI.

   - To get more information about |VT1| instances sizes and pricing see the `VT1 web page <https://aws.amazon.com/ec2/instance-types/vt1/>`_

#. Connect to the |VT1| Instance

   - After launching the instance, follow the instructions in `Connect to your instance <https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html#ec2-connect-to-instance-linux>`_ to connect to the instance. 

#. Clone the |SDK| repository::

    git clone https://github.com/Xilinx/video-sdk

   - Cloning the repo downloads the source for the examples and tutorials.
   - It is not needed to install the binary packages included in the repository as they already installed in the public |SDK| AMI. 

#. Set up the runtime environment::

    source /opt/xilinx/xcdr/setup.sh

   .. note::
      The script may issue a "Failed to start xrmd.service" message which can safely be ignored.

   - This step should be performed each time you open a new terminal on your instance. 
   - You are now ready to use the video acceleration features of your |VT1| instance. 

#. Run your first examples

   - See the :ref:`tutorials and examples <tutorials-and-examples>` page to learn how to run video transcoding jobs on your |VT1| instance.

|

****************************************************
Installing the |SDK| on an existing AMI
****************************************************

In order to use a custom AMI on a |VT1| instance, it is necessary to install the |SDK| in the AMI. This section explains how to do this. These steps are not necessary when using the public |SDK| AMI since it already includes a complete installation of the |SDK| and can readily be used on a |VT1| instance.

Prerequisites
====================================================
Install the following packages if they are not already present in your AMI.

- For Ubuntu::

    sudo apt install linux-modules-extra-$(uname -r)
    sudo apt install python

- For AL2::

    sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
    sudo yum install kernel-devel-$(uname -r)
    sudo yum install kernel-headers-$(uname -r)
    sudo yum install python
    sudo yum install boost-devel
    sudo yum install gcc-c++


Installation Procedure
====================================================

#. Open a bash shell in your existing AMI

#. Download the packages included in this repository::

    git clone https://github.com/Xilinx/video-sdk

#. Navigate to the directory containing the packages corresponding to your Operating System::

    cd ./video-sdk/release/<os>

#. Install the software components of the |SDK|::

    ./install.sh -sw

   A successfull installation will end with the following messages::

    Successfully wrote (76557 bytes) to the output file: /opt/xilinx/xcdr/xclbins/transcode.xclbin
    Leaving xclbinutil.

#. Always set up the runtime environment for the |SDK| before running video transcoding jobs on your |VT1| instance::

    source /opt/xilinx/xcdr/setup.sh

   .. note::
      The script may issue a "Failed to start xrmd.service" message which can safely be ignored.

|

****************************************************
Working with Containers
****************************************************

For instructions on how to work with Docker containers, refer to the :doc:`Container Setup </container_setup>` page of this documentation.

In addition, the user guide for Amazon ECS can be found here: https://docs.aws.amazon.com/AmazonECS/latest/developerguide/getting-started.html

|

****************************************************
Working with Kubernetes
****************************************************

For instructions on how to deploy Docker containers with Kubernetes and EKS, refer to the :doc:`Deploying with Kubernetes </deploying_with_kubernetes>` page of this documentation.

In addition, the user guide for Amazon EKS can be found here: https://docs.aws.amazon.com/eks/latest/userguide/getting-started.html



..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.