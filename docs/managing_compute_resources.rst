#####################################################
Managing Video Acceleration Compute Resources
#####################################################

.. note::
   This page describes how to manage video acceleration resources and run multiple jobs on given machine through the Xilinx Resource Manager (XRM). For information about managing multiple tasks across a cluster of machines using orchestration services (i.e Kubernetes or EKS), refer to the following page: :ref:`Deploying with Kubernetes <deploying-with-kubernetes>`.

.. highlight:: none

.. contents:: Table of Contents
    :local:
    :depth: 1
.. .. section-numbering::

**********************************
Introduction
**********************************

Each Xilinx device on an Alveo U30 card can process an aggregate load of 4K pixels at 60 frames per second. The |SDK| supports running multiple jobs simultaenously on a given device if the overall throughput does not exceed the limit of 4kp60. When the user has access to more than one device on a given machine, the |SDK| also supports running one or more jobs across multiple devices. In this situation, it becomes important to manage the pool of video acceleration resources in order to get the most out of the total compute bandwidth available in the system. 

The notion of compute units (CUs) and CU pool is central to resource managent. A typical video transcode pipeline needs a conjunction of multiple CUs like decoder, scaler, lookahead, and encoder, together forming a CU pool. Based on the input resolution and type of transcode, the load of CUs within a CU pool varies. This in turn determines how many jobs can be run real-time in parallel, and which devices has enough free resources to run a job. 

CUs and CU ppol are managed by the Xilinx® FPGA resource manager (XRM). XRM is a software layer responsible for managing the hardware accelerators available in the system. XRM keeps track of total system capacity for each of the compute units (i.e. decoder, scaler, encoder...), ensures capacity for a given use case and prevents over-allocation. 

The rest of this guide explains how to:

#. Assign jobs to specific devices :ref:`using explicit device identifiers <using-explicit-device-ids>`
#. Measure device load and determine where to run jobs using either :ref:`manual <manual-resource-management>` or :ref:`automated  <using-job-descriptions>` resource management techniques
 

.. rubric:: System Considerations

* On certain servers, it may be needed to reduce the stack size (using ``ulimit -s 1000``) in order to run more than 200 processes simultaneously.
* When splitting a job across two devices, the characteristics of the host server may impact overall performance as more data transfers are generally involved in this case.  

|

.. include:: ./include/managing_jobs/explicit_device_ids.rst

|

.. include:: ./include/managing_jobs/manual_resource_management.rst

|

.. include:: ./include/managing_jobs/automated_resource_management.rst

|

.. include:: ./include/managing_jobs/xrm_reference.rst


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
