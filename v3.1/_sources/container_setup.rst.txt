
.. _container-setup:

###############
Container Setup
###############

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::

The |SDK| supports multiple container orchestration technologies including those based on standard `runc <https://github.com/opencontainers/runc>`_ and Xilinx's own extension to runc, the Xilinx Container Runtime (XCR). The following sections describe both the XCR and the standard Docker methods of instantiating containers. 


************
Restrictions
************

- **IMPORTANT:** Regardless of selected orchestration method, each Xilinx device can have a single point of access (whether host or container). Having multiple container instances access the same device is not currently supported and can lead to unpredictable errors.

- On |VT1| instances

  - ``xbmgmt`` card management functions are not supported either on the host or within containers.
  - The ``xbutil reset`` command is not supported and should not be used within containers. The command will kill any job running on the devices, but it will not reset the devices.

- On-premises

  - The ``xbutil reset`` command will reset the specified device as well as all other devices present on the same Xilinx card, even if these other devices are not mapped to the container. In order to avoid reset conflicts, Xilinx strongly recommends mapping all the devices of a given card when running the container. For details on how to determine which devices are on the same card, consult the following instructions: :ref:`Mapping Devices to Cards <mapping-devices-to-cards>`. 
  - In the case where a single device is mapped to the container, Xilinx recommends against using the ``xbutil reset`` command.

|

.. _xcr-overview:

***************************************
Xilinx Container Runtime (XCR) Overview
***************************************

Starting with version 3.0, the core package of the |SDK| includes the `Xilinx Container Runtime (XCR) <https://github.com/Xilinx/Xilinx_Container_Runtime>`_. XCR is an extension of runc container runtime and as such is compatible with all `OCI <https://opencontainers.org/>`_ compliant container tools such as Docker, Podman, Singularity, etc.

XCR provides a convenient method for assigning either individual devices or cards to a container. For details refer to the `XCR documentation <https://xilinx.github.io/Xilinx_Container_Runtime/index.html>`_.

.. _xcr-cli:

XCR Command Line Tool
=====================

The ``xilinx-container-runtime`` command line tool allows for interrogating the host system for Xilinx devices and setting up containers in a seamless manner. For example, the following command will list all available cards on a system: 

.. code-block:: bash

   xilinx-container-runtime lscard

   CardIndex       SerialNum       DeviceBDF       UserPF                  MgmtPF                  ShellVersion
   0               XFL1VCYC4XSP    0000:21:00.1    /dev/dri/renderD128     /dev/xclmgmt8448        xilinx_u30_gen3x4_base_2(ama_u30)
   0               XFL1VCYC4XSP    0000:22:00.1    /dev/dri/renderD129     /dev/xclmgmt8704        xilinx_u30_gen3x4_base_2(ama_u30)
   1               XFL1RT5PHT31    0000:e2:00.1    /dev/dri/renderD130     /dev/xclmgmt57856       xilinx_u30_gen3x4_base_2(ama_u30)
   1               XFL1RT5PHT31    0000:e3:00.1    /dev/dri/renderD131     /dev/xclmgmt58112       xilinx_u30_gen3x4_base_2(ama_u30)

For a complete reference of the ``xilinx-container-runtime`` command line tool, consult the `xilinx-container-runtime documentation <https://xilinx.github.io/Xilinx_Container_Runtime/cli.html>`_

.. _xcr-setup:

Setting XCR as Docker Runtime
=============================

Follow the steps below to configure Docker to run with XCR:

1. Update :file:`/etc/docker/daemon.json` to contain the following text:

  .. code-block:: json

    {
        "runtimes": {
            "xilinx": {
                "path": "/usr/bin/xilinx-container-runtime",
                "runtimeArgs": []
            }
        }
    }

2. Restart the Docker service:

  .. code-block:: bash

    sudo systemctl restart docker

For more details see the `XCR docker integration guide <https://xilinx.github.io/Xilinx_Container_Runtime/docker.html>`_.

|

.. _create-docker-image:

***********************
Creating a Docker Image
***********************

There are no specific requirements in creating Docker images, for U30. Images need to include application specific packages and access to |SDK|.
As an example, the following is a minimal sample dockerfile, for Ubuntu 18.04 container::

      FROM ubuntu:18.04
      ARG DEBIAN_FRONTEND="noninteractive"
      
      SHELL ["/bin/bash", "-c"]
      RUN echo 'deb [trusted=yes] https://packages.xilinx.com/artifactory/debian-packages bionic main' > /etc/apt/sources.list.d/xilinx.list
      RUN echo 'Acquire { https::Verify-Peer false }' > /etc/apt/apt.conf.d/99verify-peer.conf
      RUN apt-get update
      RUN apt-get -y install git apt-utils sudo xrt=2.11.722 xilinx-alveo-u30-core xilinx-alveo-u30-examples && \
          apt-mark hold xrt
      RUN git clone https://github.com/gdraheim/docker-systemctl-replacement.git /usr/local/share/docker-systemctl-replacement
      RUN echo "alias systemctl='python3 /usr/local/share/docker-systemctl-replacement/files/docker/systemctl3.py'" >> /root/.bashrc
      RUN ln -s /usr/local/share/docker-systemctl-replacement/files/docker/systemctl3.py /usr/local/bin/systemctl
      #These two lines are needed only for cent, rhl or al2
      #RUN mv /usr/bin/systemctl /usr/bin/systemctl.fac
      #RUN ln -s /usr/local/bin/systemctl /usr/bin/systemctl
      RUN export PATH=/usr/local/bin/:$PATH

Once a dockerfile is created, the corresponding Docker image is built as follows::

    docker build -t video_sdk_image:v3 -f <DOCKERFILE_PATH> .

, where (``video_sdk_image:v3``) is the name assigned to the Docker image and <DOCKERFILE_PATH> is the path to the customized dockerfile

NOTE: To create and instantiate a Docker image, a working `installation <https://docs.docker.com/engine/install/>`_ of Docker is required.


|

.. _launch-docker-image:

****************************
Launching a Docker Container
****************************

A Docker container can be launched with or without XCR, the Xilinx Container Runtime. XCR provides a convenient method for assigning either individual devices or cards to a container. Launching a Docker image without using the Xilinx Container Runtime requires explicitly assigning one of more devices to the container. 


.. _launch-docker-xcr:

Launching a Docker Container with XCR
=====================================

Whether on |VT1| or on-premises, follow the steps below to launch a container using the Xilinx Container Runtime (XCR):

#. Create Docker image (``video_sdk_image:v3``), as per :ref:`Creating a Docker Image <create-docker-image>`.

#. Configure Docker to use XCR, as per :ref:`Setting XCR as Docker Runtime <xcr-setup>`.

#. Run the Docker container using either the :envvar:`XILINX_VISIBLE_CARDS` or the :envvar:`XILINX_VISIBLE_DEVICES` environment variable::

    docker run -it --rm --runtime=xilinx -e XILINX_VISIBLE_DEVICES=0,1 video_sdk_image:v3 /bin/bash -c ". /opt/xilinx/xrt/setup.sh && xbutil examine"

 - The :envvar:`XILINX_VISIBLE_CARDS` variable is used to specify a list of cards to be mapped
 - The :envvar:`XILINX_VISIBLE_DEVICES` variable is used to specify a list of devices to be mapped
 - Both variables either take ``all`` or comma separated indices for card or devices 
 - In the example above, the command maps devices 0 and 1 to the launched container


.. rubric:: NOTES 

- By default, XCR will perform exclusive assignment of devices to containers: a device will be locked to the specific container from the time of container being created till the container is stopped. While this mode can be `disabled <https://xilinx.github.io/Xilinx_Container_Runtime/docker.html#disable-device-exclusive-mode>`_, Xilinx recommends against doing so, as having different containers access the same device is not supported for U30 cards.


.. _launch-docker-vt1:

Launching a Docker Container without XCR on |VT1|
=================================================

#. Create Docker image (``video_sdk_image:v3``), as per :ref:`Creating a Docker Image <create-docker-image>`.

#. Gather the User BDFs and renderIDs of the Xilinx devices present in your system. For detailled information on how to do this, refer to the instructions for looking-up the :ref:`User BDF <device-bdf>` and :ref:`renderID <device-render-id>`. ::

    xbutil examine
    ...
    ls /sys/bus/pci/devices/<Domain>\:<Bus>\:<Device>.<Function>/drm 

#. Run the Docker container using the ``--device`` option with the renderID (user function port) of the desired devices to make them accessible by the container::

    docker run -it --device=/dev/dri/renderD<ID1>:/dev/dri/renderD<ID1> 
                   --device=/dev/dri/renderD<ID2>:/dev/dri/renderD<ID2> 
                   video_sdk_image:v3 /bin/bash

#. Once in the container, validate that the desired devices have been successfully mapped and can properly be used::

    source /opt/xilinx/xrt/setup.sh
    xbutil examine 


.. rubric:: NOTES 

- AWS has created a script to automatically generate the ``--device`` option with the renderID of all available devices. The script can be found `here <https://raw.githubusercontent.com/Xilinx/Xilinx_Base_Runtime/master/utilities/xilinx_aws_docker_setup.sh>`_ Details about how to use this script can be found on the AWS Compute Blog: https://aws.amazon.com/blogs/compute/deep-dive-on-amazon-ec2-vt1-instances/

.. _launch-docker-on-prem:

Launching a Docker Container without XCR on Premises
====================================================

#. Create Docker image (``video_sdk_image:v3``), as per :ref:`Creating a Docker Image <create-docker-image>`.

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
                   video_sdk_image:v3 /bin/bash

#. Once in the container, validate that the desired devices have been successfully mapped and can properly be used::

    source /opt/xilinx/xrt/setup.sh
    xbutil examine 

.. rubric:: NOTES 

- Mapping the management port using the xclmgmtID allows using to the ``xbmgmt`` card management utility from within the container. Doing so is optional. 


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.