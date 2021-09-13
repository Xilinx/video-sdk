.. _c_api_programming_guide:

#####################################################
C API Programming Guide
#####################################################

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::


********
Overview
********

The |SDK| provides a C-based application programming interface (API) which facilitates the integration of Xilinx transcoding capabilities in proprietary frameworks. This API is provided in the form of plugins leveraging the Xilinx Media Accelerator (XMA) library and the Xilinx Resource Manager (XRM) library.

.. rubric:: The XMA Library

The XMA library (libxmaapi) is meant to simplify the development of applications managing and controlling video accelerators such as decoders, scalers, filters, and encoders. The libxmaapi is comprised of two API interfaces: the lower-edge interface and the upper-edge interface: 

- The lower-edge API is an interface intended for plugin developers responsible for implementing hardware control of specific Xilinx acceleration kernels. These plugins are specialized user space drivers that are aware of the low-level interface of the hardware accelerators. 

- The upper-edge API is a higher-level, generalized interface intended for application developers responsible for integrating control of Xilinx accelerators into software frameworks such as FFmpeg, GStreamer, or proprietary frameworks.

The |SDK| includes plugins optimized for the Xilix video accelerators such as the ones found on Alveo U30 cards. A software developer integrating the hardware-accelerated features of Xilinx devices in a proprietary framework only needs to be familiar with the XMA upper-edge API and the properties of each plugin. 

The XMA library is included as part of the Xilinx Runtime (XRT) library. General documentation on XMA can be found in the `XRT documentation <https://xilinx.github.io/XRT/master/html/xma_user_guide.html>`_. The `XMA Upper Edge API Library <https://xilinx.github.io/XRT/master/html/xmakernels.main.html>`_ section of the XRT documentation provides a complete reference of the XMA upper-edge API.


.. rubric:: The XRM Library

The XRM library is used to manage the hardware accelerators available in the system. XRM keeps track of total system capacity for each of the compute units such as the decoder, scaler, and encoder. The XRM library makes it possible to perform actions such as reserving, allocating and releasing resources; calculating resource load and max capacity.

Details about XRM can be found in `general XRM documentation <https://xilinx.github.io/XRM/index.html>`_.


.. rubric:: The |SDK| Plugins

The |SDK| provides 4 different plugins, each corresponding to a specific hardware accelerated feature of the card:

- The decoder plugin
- The encoder plugin
- The lookahead plugin
- The scaler plugin

Any combination of plugins can be used when integrating with a proprietary framework.

Sample source code and applications using the |SDK| plugins and the XMA APIs to do video encoding, decoding, scaling and transcoding can be found in the :doc:`XMA Tutorials </examples/xma/xma_apps>` included in this repository.

|

*************************************
General Application Development Guide
*************************************

Integration layers for applications using the |SDK| are organized around the following steps:

#. Initialization
#. Resource Reservation
#. Session Creation
#. Runtime Processing
#. Cleanup

Initialization
==============
Applications using the plugins must first create a XRM context using the :c:func:`xrmCreateContext()` function in order to establish a connection with the XRM daemon. The application must then initialize the XMA library using the :c:func:`xma_initialize()` function. 

Detailled examples of how to perform these two steps in order to initialize an application can be found in all of the sample XMA applications, for instance :url_to_repo:`examples/xma/decode_only/src/xlnx_dec_xrm_interface.c`.

Further information about these APIs can be found in the online `XRT <https://xilinx.github.io/XRT/master/html/xmakernels.main.html>`_ and `XRM <https://xilinx.github.io/XRM/lib.html#global-functions>`_ documentation.

Resource Allocation
===================
After the initialization step, the application must determine on which device to run and reserve the necessary hardware resources (CUs) on that device. This is done using the XRM APIs, as described in detail in the :ref:`XRM API Reference Guide <xrm-api-reference>` below.

Session Creation
================
Once the resources have been allocated, the application must create dedicated plugin sessions for each of the hardware accelerators that need to be used (decoder, scaler, encoder, lookahead).

To create a session, the application must first initialize all the required properties and parameters of the particular plugin. It must then call the  corresponding session creation function. A complete reference for all the plugins is provided below.

Runtime Processing
==================
The plugins provide functions to send data from the host and receive data from the device. The data is in the form of video frames (:c:struct:`XmaFrame`) or encoded video data (:c:struct:`XmaFrameData`), depending on the nature of the plugin. It is also possible to do zero-copy operations where frames are passed from one hardware accelerator to the next without being copied back to the host. The send and receive functions are specific to each plugin and the return code should be used to determine the next suitable action. A complete reference for all the plugins is provided below.

Cleanup
=======
When the application finishes, it should destroy each plugin session using the corresponding destroy function. Doing so will free the resources on the Xilinx devices for other jobs and ensure that everything is released and cleaned-up properly.

The application should also use the :c:func:`xrmDestroyContext()` function to destroy the XRM session, stop the connection to the daemon and ensure all resources are properly released.

|

*****************************************************************
Compiling and Linking with the |SDK| Plugins
*****************************************************************

The plugins can be dynamically linked to the application. The required packages to build applications are XRT, XRM and XVBM. These packages provided as part of the |SDK|. 

To provide the necessary declarations in your application, include the following headers in your source code::

  #include <xrm.h>
  #include <xmaplugin.h>
  #include <xma.h>
  #include <xvbm.h> 

To compile and link your application with the plugins, add the following lines to your Makefile::

  CFLAGS  += $(shell pkg-config --cflags libxma2api libxma2plugin xvbm libxrm)
  LDFLAGS += $(shell pkg-config --libs   libxma2api libxma2plugin xvbm libxrm)

These should add the following switches to your gcc commands::

  -I/opt/xilinx/xrt/include/xma2 -I/opt/xilinx/xrt/include -I/opt/xilinx/xvbm/include -I/opt/xilinx/xrm/include

  -L/opt/xilinx/xrt/lib -L/opt/xilinx/xvbm/lib -L/opt/xilinx/xrm/lib -lxma2api -lxma2plugin -lxvbm -lstdc++ -lxrm -lboost_system -lboost_filesystem -lboost_thread -lboost_serialization -luuid -ldl -lxrt_core

|

.. include:: ./include/c_apis/xma_common_data_structures.rst

|

.. include:: ./include/c_apis/xma_decoder_plugin_api.rst

|

.. include:: ./include/c_apis/xma_scaler_plugin_api.rst

|

.. include:: ./include/c_apis/xma_encoder_plugin_api.rst

|

.. include:: ./include/c_apis/xma_lookahead_plugin_api.rst

|

.. include:: ./include/c_apis/xvbm_api.rst

|

.. include:: ./include/c_apis/xrm_api.rst

|


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
