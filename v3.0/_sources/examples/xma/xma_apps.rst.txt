#####################################################
XMA Examples for the |SDK|
#####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

The examples in the ``/opt/examples/u30/xma`` folder illustrate how C-based applications can natively interact with Xilinx devices using the |SDK| plugins and the XMA (Xilinx Media Accelerator) APIs. The examples are for advanced use-cases where using the FFmpeg command-line interface is not appropriate.

Detailed documentation on the |SDK| plugin interface and the XMA APIs can be found in the :doc:`C API Programming Guide </c_apis>` section of the documentation.

.. rubric:: Requirements

- The XMA applications has been built for and verified on Ubuntu 18.04, Ubuntu 20.04, RHEL 7.8 and AWS Linux V2.
- The XMA applications only work with elementary streams. Container formats such as mp4 are not supported. 

*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst

The setup script exports important environment variables, starts the Xilinx Resource Manager (XRM) daemon, and ensures that the Xilinx devices and the XRM plugins are properly loaded. It also moves to the top of the system PATH the FFmpeg binary provided as part of the |SDK|.

Sourcing the setup script should be performed each time you open a new terminal on your system. This is required for the environment to be correctly configured. 


****************************************************
Build Instructions
****************************************************

.. rubric:: Build

#. Create a local copy of the XMA examples::

    cp -rp /opt/xilinx/examples/u30/xma xma_local

#. Switch to the local copy and build the examples::

    cd xma_local
    make clean
    make dev

   The executables are placed in the ``Debug/xma_apps/examples`` directory.


.. rubric:: Test

The ``build_and_test.bash`` script included with examples can be used to build and test all the XMA applications at once. This script sets up the environment, builds all the applications and then runs a simple test to validate that each of the applications works correctly. Run the script as follows::

    ./build_and_test.bash

To test each application individually, refer to the sections below.

|

.. _xma-decoder-example:
.. include:: ./include/decoder.rst

|

.. _xma-encoder-example:
.. include:: ./include/encoder.rst

|

.. _xma-scaler-example:
.. include:: ./include/scaler.rst

|

.. _xma-transcoder-example:
.. include:: ./include/transcoder.rst

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
