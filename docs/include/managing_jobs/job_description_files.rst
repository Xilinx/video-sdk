.. _job-descriptions-files:


Video Transcode Job Descriptions
==================================

A video transcode job description provides information to the resource manager about what resources are needed to run a particular job. With this information, the resource manager can calculate the CU load for the specified job as well as the maximum possible number of jobs that can be run real-time in parallel.

A video transcode job description is specified through a JSON file and the key-value pairs specify the functions, formats, and resolutions needed.

function
    Which HW resource to use (DECODER, SCALER, ENCODER)

format
    Input/output format (H264, HEVC, yuv420p)

resolution
    Input/output height, width, and frame-rate as a numerator / denominator fraction 

job-count
    Optional entry to specify the number of instances of the specified job which can run on one device.
    When this entry is used, the CU load is calculated based on the specified job-count. Any channel-load value is ignored.
    This option is useful to provide an accurate number of possible jobs in the case where the load calculation by XRM is optimistic. This can happen because device memory is currently not an XRM managed resource; and in the case of high-density low-resolution jobs, it is possible to run out device memory before running out of compute resources.  

channel-load
    Optional entry to specify a different compute load for a given function than calculated by the resource manager.
    This option will be deprecated and removed in a future release. The job-count option should be used instead.

resources
    All the resources listed in this section of the job description will be allocated on the same device. If the job requires a single device, this is the section in which resources should be specified. 

additionalresources_1
    Optional entry to specify the resources which need be allocated on a second device. If a job cannot fit on a single device and must be split across two devices, then the resources which should be allocated on the first device should be listed in the "resources" section and the resources which should be allocated on the second device should be listed in the "additionalresources_1" section.  

Several examples of JSON job slot descriptions can be found in the ``/opt/xilinx/launcher/scripts/describe_job`` folder once the |SDK| has been installed.

Below is the ``/opt/xilinx/launcher/scripts/describe_job/describe_job_h264.json`` example. This JSON example describes an ABR transcode job which uses a decoder, scaler, and encoder to generate 5 output renditions. The job-count entry is used to explicitly request 4 instances of this job per device:

.. literalinclude:: ./include/json/describe_job_h264.json
   :language: none

Below is the ``/opt/xilinx/launcher/scripts/describe_job/testjob_4k_2dev.json`` example. This JSON example describes a 4K ABR transcode job split across two devices and generating 6 output renditions. The resources allocated on the first device are listed in the "resources" section and the resources allocated on the second device are listed in the "additionalresources_1" section. This job corresponds to the one pictured above in the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` topic.

.. literalinclude:: ./include/json/testjob_4k_2dev.json
   :language: none


The next sections document the two different ways of using job descriptions to run multiple jobs across one or more devices:

- :ref:`Using the job slot reservation tool <using-job-slot-reservations>`
- Automated job launcher examples for :ref:`FFmpeg <using-ffmpeg-launcher>` and :ref:`GStreamer <using-gstreamer-launcher>`


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.