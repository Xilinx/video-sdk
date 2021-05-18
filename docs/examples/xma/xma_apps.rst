#####################################################
 XMA Examples for Xilinx U30
#####################################################

.. contents:: Table of Contents
    :local:
    :depth: 1

The examples in the ``examples/xma`` folder illustrate how C-based applications can natively interact with U30 cards using the U30 plugins and the XMA (Xilinx Media Accelerator) APIs. The examples are for advanced use-cases where using the FFmpeg command-line interface is not appropriate.

Detailed documentation on the U30 plugin interface and the XMA APIs can be found in the :doc:`C API Programming Guide </c_apis>` section of the documentation.

****************************************************
Build and Test Instructions
****************************************************

To build and test the sample XMA applications, the U30 release packages must be installed on the server, and the U30 devices must to be flashed with the shell provided in the release package.

To build and test all the XMA applications at once, run the following command from within the ``./examples/xma`` directory::

	source ./setup.sh

This script sets up the environment, builds all the applications and then runs a simple test to validate that each of the applications works correctly.
The executables are placed in the ``build`` directory of the corresponding application subfolder.

To build and test each application individually, refer to the sections below.

.. _xma-decoder-example:
.. include:: ./include/decode_only.rst
.. include:: ./include/decode_only_build.rst
.. include:: ./include/decode_only_test.rst

.. _xma-encoder-example:
.. include:: ./include/encode_only.rst
.. include:: ./include/encode_only_build.rst
.. include:: ./include/encode_only_test.rst

.. _xma-scaler-example:
.. include:: ./include/scale_only.rst
.. include:: ./include/scale_only_build.rst
.. include:: ./include/scale_only_test.rst

.. _xma-transcoder-example:
.. include:: ./include/transcode.rst
.. include:: ./include/transcode_build.rst
.. include:: ./include/transcode_test.rst

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
