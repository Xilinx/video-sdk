#####################################################
XMA Examples for the |SDK|
#####################################################

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

The examples in the ``examples/xma`` folder illustrate how C-based applications can natively interact with Xilinx devices using the |SDK| plugins and the XMA (Xilinx Media Accelerator) APIs. The examples are for advanced use-cases where using the FFmpeg command-line interface is not appropriate.

Detailed documentation on the |SDK| plugin interface and the XMA APIs can be found in the :doc:`C API Programming Guide </c_apis>` section of the documentation.

.. rubric:: Requirements

- The XMA applications has been built for and verified on Ubuntu 18.04, Ubuntu 20.04, RHEL 7.8 and AWS Linux V2.
- The XMA applications only work with elementary streams. Container formats such as mp4 are not supported. 

****************************************************
Build Instructions
****************************************************

.. rubric:: Pre-requisites

To build and test the XMA applications, the |SDK| packages must be installed on the server, and the Xilinx devices must to be flashed with the shell provided in the release package.

.. rubric:: Build

#. Export the XRT and XRM to build the xma apps::

    source /opt/xilinx/xcdr/setup.sh

#. Do make clean and make dev in the application directory::

    cd examples/xma
    make clean
    make dev

   The executables are placed in the ``Debug/xma_apps/examples`` directory.

#. Optional: to generate DEB and RPM packages for the XMA apps, run one of the two commands below::

    make DEB
    make RPM

.. rubric:: Test

The ``build_and_test.bash`` script included in the ``./examples/xma`` directory can be used to build and test all the XMA applications at once. This script sets up the environment, builds all the applications and then runs a simple test to validate that each of the applications works correctly. Run the script as follows::

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
