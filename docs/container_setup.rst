
.. _container-setup:

######################################
Container Setup
######################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

|

The Github repository for the |SDK| includes predefined :url_to_repo:`dockerfiles` which can be used to easily create Docker images with the |SDK| for different OS configurations. There are different Dockerfiles for on-prem and |VT1| usage. The Dockerfiles included in the repository can be used as-is or customized based on end-user needs.  

The instructions below explain how to create and then launch a Docker image from one of the Dockerfiles included in the |SDK| repository. 

A working installation of Docker is required.

|

*****************************************
Working with Docker |VT1|
*****************************************

Creating a Docker Image for |VT1| Usage
=========================================

#. If not already available, clone the video-sdk repository on your machine::

    git clone https://github.com/Xilinx/video-sdk

#. Navigate to the root of the video-sdk repository::

    cd path/to/video-sdk

#. Build the docker image using the supplied Dockerfile corresponding to the desired OS::

    docker build -t video_sdk_image:v1 -f dockerfiles/vt1/Dockerfile.<Desired OS> . 

.. _launch-docker-vt1:

Launching a Docker Container on |VT1|
=========================================

#. Gather the User BDFs and renderIDs of the Xilinx devices present in your system. For detailled information on how to do this, refer to the instructions for looking-up the :ref:`User BDF <device-bdf>` and :ref:`renderID <device-render-id>`. ::

    xbutil examine
    ...
    ls /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/drm 

#. Run the Docker container using the ``--device`` option with the renderID (user function port) of the desired devices to make them accessible by the container::

    docker run -it --device=/dev/dri/renderD<ID1>:/dev/dri/renderD<ID1> 
                   --device=/dev/dri/renderD<ID2>:/dev/dri/renderD<ID2> 
                   video_sdk_image:v1 /bin/bash

#. Once in the container, validate that the desired devices have been successfully mapped and can properly be used::

    source /opt/xilinx/xrt/setup.sh
    xbutil examine 

.. rubric:: NOTES 

- The ``xbutil reset`` command should not be used from within containers running on |VT1| instances. The command will kill any job running on the devices, but it will not reset the devices.


|

*****************************************
Working with  Docker On-Premises
*****************************************

Creating a Docker Image for On-Prem Usage
=========================================

#. If not already available, clone the video-sdk repository on your machine::

    git clone https://github.com/Xilinx/video-sdk

#. Navigate to the root of the video-sdk repository::

    cd path/to/video-sdk

#. Build the docker image using the supplied Dockerfile corresponding to the desired OS::

    docker build -t video_sdk_image:v1 -f dockerfiles/on_prem/Dockerfile.<Desired OS> . 

.. _launch-docker-on-prem:

Launching a Docker Container On-Premises
=========================================

#. Gather the User BDFs and renderIDs of the Xilinx devices present in your system. For detailled information on how to do this, refer to the instructions for looking-up the :ref:`User BDF <device-bdf>` and :ref:`renderID <device-render-id>`. ::

    xbutil examine
    ...
    ls /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/drm 

#. Gather the Management BDFs and xclmgmtID of the Xilinx devices present in your system. For detailled information on how to do this, refer to the instructions for looking-up the :ref:`Management BDF <device-management-bdf>` and :ref:`xclmgmtID <device-xclmgmt-id>`. ::

    xbmgmt examine
    ...
    cat /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/instance 

#. Run the Docker container using the ``--device`` option with the renderID (user function port) and the xclmgmtID (management function port) of the desired devices to make them accessible by the container::

    docker run -it --device=/dev/dri/renderD<ID1>:/dev/dri/renderD<ID1> --device=/dev/xclmgmt<ID1>:/dev/xclmgmt<ID1>
                   --device=/dev/dri/renderD<ID2>:/dev/dri/renderD<ID2> --device=/dev/xclmgmt<ID2>:/dev/xclmgmt<ID2>
                   video_sdk_image:v1 /bin/bash

#. Once in the container, validate that the desired devices have been successfully mapped and can properly be used::

    source /opt/xilinx/xrt/setup.sh
    xbutil examine 

.. rubric:: NOTES 

- The ``xbutil reset`` command will reset the specified device as well as all other devices present on the same Xilinx card, even if these other devices are not mapped to the container. In order to avoid reset conflicts, Xilinx strongly recommends mapping all the devices of a given card when running the container. For details on how to determine which devices are on the same card, consult the following instructions: :ref:`Mapping Devices to Cards <mapping-devices-to-cards>`. 

- In the case where a single device is mapped to the container, Xilinx recommends against using the ``xbutil reset`` command.

- Mapping the management port using the xclmgmtID allows using to the ``xbmgmt`` card management utility from within the container. Doing so is optional. 

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.