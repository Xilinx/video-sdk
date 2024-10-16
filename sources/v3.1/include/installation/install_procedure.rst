#. Disable automatic kernel updates on your system

#. Install the following required packages if they are not already present

   + RHEL

   ::

    sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

   + Amazon Linux 2

   ::

    sudo amazon-linux-extras install epel -y


#. Remove older versions of the |SDK| packages in case any where previously installed

   + Ubuntu

   ::

    sudo apt-get remove xvbm xilinx-u30-xvbm xrmu30decoder xrmu30scaler xrmu30encoder xmpsoccodecs xmultiscaler xlookahead xmaapps xmapropstojson xffmpeg launcher jobslotreservation xcdr
    sudo apt-get remove xrm xilinx-container-runtime xilinx-xvbm xilinx-u30-xrm-decoder xilinx-u30-xrm-encoder xilinx-u30-xrm-multiscaler xilinx-u30-xma-multiscaler xilinx-u30-xlookahead xilinx-u30-xmpsoccodecs xilinx-u30-xma-apps xilinx-u30-xmapropstojson xilinx-u30-xffmpeg xilinx-u30-launcher xilinx-u30-jobslotreservation xilinx-u30-xcdr xilinx-u30-gstreamer-1.16.2 xilinx-u30-vvas xilinx-sc-fw-u30 xilinx-u30-gen3x4-base xilinx-u30-gen3x4-validate

   + RHEL and Amazon Linux 2

   ::

    sudo yum remove xvbm xilinx-u30-xvbm xrmu30decoder xrmu30scaler xrmu30encoder xmpsoccodecs xmultiscaler xlookahead xmaapps xmapropstojson xffmpeg launcher jobslotreservation xcdr
    sudo yum remove xrm xilinx-container-runtime xilinx-xvbm xilinx-u30-xrm-decoder xilinx-u30-xrm-encoder xilinx-u30-xrm-multiscaler xilinx-u30-xma-multiscaler xilinx-u30-xlookahead xilinx-u30-xmpsoccodecs xilinx-u30-xma-apps xilinx-u30-xmapropstojson xilinx-u30-xffmpeg xilinx-u30-launcher xilinx-u30-jobslotreservation xilinx-u30-xcdr xilinx-u30-gstreamer-1.16.2 xilinx-u30-vvas xilinx-sc-fw-u30 xilinx-u30-gen3x4-base xilinx-u30-gen3x4-validate

#. Ensure your package management client points to the remote package repository for the |SDK|, according to the instructions described in the :ref:`package feed configuration <package-feed-configuration>` page. 

#. Install the core package of the |SDK|. **Note**: it is imperative for the XRT package to be set to version 2.11.722.

   + Ubuntu

   ::

    sudo apt-get update
    sudo apt-get install xrt=2.11.722
    sudo apt-mark hold xrt
    sudo apt-get install xilinx-alveo-u30-core   

   + RHEL and Amazon Linux 2

   ::

    sudo yum update
    sudo yum install yum-plugin-versionlock
    sudo yum install xrt-2.11.722-1.x86_64
    sudo yum versionlock xrt-2.11.722
    sudo yum install xilinx-alveo-u30-core

#. Optionally install the other packages of the |SDK|. Refer to the :ref:`package description <package-feed>` table for more details on the different packages included in the |SDK|.  **Note**: on RHEL, installing the GStreamer package requires an :ref:`active RHEL subscription <rhel-subscription>`.


   + Ubuntu

   ::

    sudo apt-get install xilinx-alveo-u30-ffmpeg   
    sudo apt-get install xilinx-alveo-u30-gstreamer   
    sudo apt-get install xilinx-alveo-u30-examples   

   + RHEL and Amazon Linux 2

   ::

    sudo yum install xilinx-alveo-u30-ffmpeg   
    sudo yum install xilinx-alveo-u30-gstreamer   
    sudo yum install xilinx-alveo-u30-examples 


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.