.. _package-feed:

#########################
Distribution Package Feed
#########################

.. highlight:: none
.. contents:: Table of Contents
    :local:
    :depth: 2


Package Descriptions
====================

.. list-table:: 
   :widths: 30 70
   :header-rows: 1

   * - Name
     - Description
   * - xilinx-alveo-u30-core
     - This package includes all the core elements that are required for developing and running applications using the C-based API and runtime library of the |SDK|.
   * - xilinx-alveo-u30-ffmpeg
     - This package includes all the relevant software for running hardware-accelerated FFmpeg pipelines. Installing this package also install the xilinx-alveo-u30-core package.
   * - xilinx-alveo-u30-gstreamer
     - This package includes all the relevant software for running hardware-accelerated GStreamer pipelines. Installing this package also install the xilinx-alveo-u30-core package.
   * - xilinx-alveo-u30-examples
     - This package includes various examples of FFmpeg and GStreamer pipelines, and C-based applications. Installing this package also install the xilinx-alveo-u30-core, xilinx-alveo-u30-ffmpeg and xilinx-alveo-u30-gstreamer packages.

|

Minimal Required Packages
=========================

The following table indicates the minimal required packages, on either host or container, to run a video pipeline, using :ref:`C APIs <c_api_programming_guide>`. Note that only one running environment at a time can be active.

.. list-table::
   :widths: 30 30 30
   :header-rows: 1

   * - Running Environment
     - Host Packages
     - Container Packages
   * - Host
     - xilinx-alveo-u30-core
     - N/A
   * - Container
     - xilinx-alveo-u30-core
     - xilinx-alveo-u30-core

|

.. _package-feed-configuration:

Configuring the Package Feed
============================

Distribution based package feeds allow for convenient and robust methods to update and upgrade relevant packages of the |SDK|. In order to be able to install the |SDK| packages from the package feed, point your package management client to the remote |SDK| package repository according to the instructions described below.

Ubuntu
------

Add the following line to the :file:`/etc/apt/sources.list.d/xilinx.list` file::

  deb [trusted=yes] https://packages.xilinx.com/artifactory/debian-packages <distro name> main

Where <distro name> can be either ``bionic``, ``focal`` or ``jammy``. If needed, the distro name can be found using the ``lsb_release -c`` command.


Red Hat Linux 7.8
-----------------

Add the following lines to the :file:`/etc/yum.repos.d/xilinx.repo` file::

  [MetaArtifactory]
  name=MetaArtifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/7/
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/7/repodata/repomd.xml.key
  sslverify=0

  [Artifactory]
  name=Artifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/repodata/repomd.xml.key
  sslverify=0

  [BaseArtifactory]
  name=BaseArtifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/noarch/
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/noarch/repodata/repomd.xml.key
  sslverify=0


Amazon Linux 2
--------------

Add the following lines to the :file:`/etc/yum.repos.d/xilinx.repo` file::

  [Amzn2Artifactory]
  name=Amzn2Artifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/2
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/repodata/repomd.xml.key
  sslverify=0
   
  [Artifactory]
  name=Artifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/x86_64/repodata/repomd.xml.key
  sslverify=0
   
  [BaseArtifactory]
  name=BaseArtifactory
  baseurl=https://packages.xilinx.com/artifactory/rpm-packages/noarch/
  enabled=1
  gpgcheck=0
  gpgkey=https://packages.xilinx.com/artifactory/rpm-packages/noarch/repodata/repomd.xml.key
  sslverify=0



..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
