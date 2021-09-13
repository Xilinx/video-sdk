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

Each Xilinx device on an Alveo U30 card can process an aggregate load of 4K pixels at 60 frames per second. The |SDK| supports running multiple jobs simultaenously on a given device if the overall throughput does not exceed the limit of 4kp60. When the user has access to more than one device on a given machine, the |SDK| also supports running multiple jobs across multiple devices. In this situation, it becomes important to manage the pool of video acceleration resources in order to get the most out of the total compute bandwidth available in the system. 

The |SDK| provides two distinct methods of managing resources and mapping transcoding jobs to Xilinx devices:

#. :ref:`Using explicit device IDs <using-explicit-device-ids>` - This is the recommended method. It is the simplest to use and will work for most use cases.
#. :ref:`Using the job description and reservation service <using-job-descriptions>` - This method is considered an advanced feature. It is provided as a starting point for users who need to develop custom job management services.

Both methods rely on the Xilinx® FPGA resource manager (XRM). XRM is a software layer responsible for managing the hardware accelerators available in the system. XRM keeps track of total system capacity for each of the compute units (i.e. decoder, scaler, encoder...), ensures capacity for a given use case and prevents over-allocation. 

.. rubric:: System Considerations

* When a job ends and releases CU resources to XRM, it takes a few milliseconds before they become available for another job. 
* On certain servers, it may be needed to reduce the stack size (using ``ulimit -s 1000``) in order to run more than 200 processes simultaneously.

|

.. _using-explicit-device-ids:

**********************************
Using Explicit Device IDs
**********************************
The simplest way to run multiple FFmpeg jobs across all available devices is to use the :option:`-xlnx_hwdev` option.

By default (if the option is not specified) the job will be submitted to device 0. When running multiple jobs, device 0 will likely run out resources rapidly and additional jobs will error out due to insufficient resources. 

The :option:`-xlnx_hwdev` option is used to specify the device on which the job should be run. This makes it easy and straightforward to leverage the entire video acceleration capacity of your system, regardless of the number of cards and devices. In addition, this method allows running different types of jobs on the same device (i.e.: jobs with different resolutions, frame rates, etc...). 


Examples using Explicit Device IDs
==================================

Specific examples of how to run multiple FFmpeg processes using explicit device IDs can be found in the :ref:`FFmpeg Tutorials <multiple-ffmpeg-jobs-example>` included in this repository.


Managing Resource Utilization
==================================

Given that each device has a 2160p60 (4K) input and output bandwidth limit, the user is responsible for only submitting jobs which will not exceed the capacity of the specified device. The :ref:`Managing Resource Utilization <managing-resources>` section of the Card Management guide provides information on how to estimate CU requirements and check current device load.

|

.. _using-job-descriptions:

**********************************
Using Job Descriptions
**********************************

Job descriptions and the associated job reservation system are an alternate way to manage resources and run multiple jobs across one or more devices. This method is more involved than using explicit device IDs, and it is intended for users who need to develop custom job management applications.

.. note::
    A current limitation of this method is that only one job description file can be used at a time, and a job description file can only contain a single job description. The system will only manage multiple jobs if they match the same description. Simultaneously managing different job types is not supported currently. The jobs need to be stopped before a new job description can be loaded.


Video Transcode Job Description
==================================

The Xilinx video transcode pipeline needs a conjunction of multiple compute units (CUs) like decoder, scaler, lookahead, and encoder, together forming a CU pool. Based on the input resolution and type of transcode, the load of CUs within a CU pool varies. This in turn determines how many instances of such a job can be run real-time in parallel on all the cards managed by XRM. If there is free capacity, XRM will send the job to an available device.

The video transcode job description provides information to the resource manager about what kind of transcode is intended to run on the card. With this information, XRM calculates the CU load for the specified job as well as the maximum possible number of jobs that can be run real-time in parallel.

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

Several examples of JSON job slot descriptions can be found in the ``/opt/xilinx/launcher/scripts/describe_job`` folder once the |SDK| has been installed.

Below is a modified version of the ``/opt/xilinx/launcher/scripts/describe_job/describe_job.json`` example. This JSON example describes an ABR transcode job which uses a decoder, scaler, and encoder to generate 5 output renditions. A job-count entry has been added to explicitly request 4 instances of this job per device::

    {
        "request": {
            "name": "describe_job_h264",
            "request_id": 1,
            "parameters": {
                "name": "testjob",
                "job-count": 4,
                "resources": 
                [
                    {
                        "function": "DECODER",
                        "format":   "H264",
                        "resolution": { "input": { "width": 1920, "height": 1080, "frame-rate": { "num":60, "den":1} } }
                    },
                    {
                        "function": "SCALER",
                        "format":   "yuv420p",
                        "resolution": 
                        {
                            "input": { "width": 1920, "height": 1080, "frame-rate": { "num":60, "den":1} },
                            "output": 
                            [
                                { "width": 1280, "height": 720, "frame-rate": { "num":60, "den":1} },
                                { "width":  848, "height": 480, "frame-rate": { "num":60, "den":1} },
                                { "width":  640, "height": 360, "frame-rate": { "num":60, "den":1} },
                                { "width":  288, "height": 160, "frame-rate": { "num":60, "den":1} }
                            ]
                        }
                    },
                    {
                        "function": "ENCODER",
                        "format":   "H264",
                        "resolution": { "input": { "width": 1280, "height": 720, "frame-rate": { "num":60, "den":1} } } 
                    },
                    {
                        "function": "ENCODER",
                        "format":   "H264",
                        "resolution": { "input": { "width": 1280, "height": 720, "frame-rate": { "num":30, "den":1} } } 
                    },
                    {
                        "function": "ENCODER",
                        "format":   "H264",
                        "resolution": { "input": { "width":  848, "height": 480, "frame-rate": { "num":30, "den":1} } }  
                    },
                    {
                        "function": "ENCODER",
                        "format":   "H264",
                        "resolution": { "input": { "width":  640, "height": 360, "frame-rate": { "num":30, "den":1} } } 
                    },
                    {
                        "function": "ENCODER",
                        "format":   "H264",
                        "resolution": { "input": { "width":  288, "height": 160, "frame-rate": { "num":30, "den":1} } }  
                    }
                ]
            }
        }
    }


The next sections document the two different ways of using job descriptions to run multiple FFmpeg jobs across one or more devices:

- :ref:`Using the job slot reservation application <using-job-slot-reservations>`
- :ref:`The FFmpeg Launcher example <using-ffmpeg-launcher>`


.. _using-job-slot-reservations:

Using Job Slot Reservations
==================================

The job slot reservation application takes as input a video transcode job reservation in the form of a JSON file as described in the previous section. The video transcode job description provides information to the resource manager about what kind of transcode is intended to run on the card. With this information the resource manager calculates the CU load for the specified job as well as the maximum possible number of jobs that can be run real-time in parallel. 

Once the maximum possible number of jobs is known, CUs and job slots are reserved, and corresponding reservation IDs are stored in a bash file at ``/var/tmp/xilinx/xrm_jobReservation.sh``. A reservation ID is a unique identifier which is valid while the job slot reservation application is running. These reservation IDs are passed to individual FFmpeg process via an environment variable XRM_RESERVE_ID. The FFmpeg processes then use this reservation ID to retrieve the corresponding CUs reserved earlier.

The reserved resourced are released by ending the job reservation process. Reserved slots can be reused after an FFmpeg job finishes as long as the job reservation process is still running.

The `source code of the job slot reservation application <https://github.com/Xilinx/app-jobslot-reservation-xrm/tree/master/jobSlot_reservation.cpp>`_ is included in the Github repository of |SDK| and can be used as a starting point for developing custom orchestration layers. 

The following steps show how to use the job slot reservation to dispatch multiple parallel instances of an ABR ladder job:

#. Setup the environment::

    source /opt/xilinx/xcdr/setup.sh

#. Run the job slot reservation application with the desired JSON job description. For example::

    jobSlotReservation /opt/xilinx/launcher/scripts/describe_job/describe_job.json

    For describe_job.json, the possible number of job slots available = 8 
    -------------------------------------------------------------------------------------- 
    The Job_slot_reservations are alive as long as this application is alive! 
    (press Enter to end) 
    --------------------------------------------------------------------------------------

   The job slot reservation application creates a ``/var/tmp/xilinx/xrm_jobReservation.sh`` with XRM_RESERVE_ID_n set to unique IDs generated by XRM (with n ranging from 1 to the number of possible job slots for the given job). Here is an example of this generated file::

    source /opt/xilinx/xrt/setup.sh 
    source /opt/xilinx/xrm/setup.sh 
    export XRM_RESERVE_ID_1=9 
    export XRM_RESERVE_ID_2=10 
    export XRM_RESERVE_ID_3=11 
    export XRM_RESERVE_ID_4=12 
    export XRM_RESERVE_ID_5=13 
    export XRM_RESERVE_ID_6=14 
    export XRM_RESERVE_ID_7=15 
    export XRM_RESERVE_ID_8=16

#. Launch individual FFmpeg processes in distinct shells after sourcing the ``/var/tmp/xilinx/xrm_jobReservation.sh`` file and setting XRM_RESERVE_ID environment to a unique XRM_RESERVE_ID_n. 

   For job 1::

    source /var/tmp/xilinx/xrm_jobReservation.sh 
    export XRM_RESERVE_ID=${XRM_RESERVE_ID_1} 
    ffmpeg -c:v mpsoc_vcu_h264 ...

   For job 2::

    source /var/tmp/xilinx/xrm_jobReservation.sh 
    export XRM_RESERVE_ID=${XRM_RESERVE_ID_2} 
    ffmpeg -c:v mpsoc_vcu_h264 ...

   And so forth for the other jobs.

#. Press **Enter** in the job reservation app terminal to release the resources after the jobs are complete.


.. rubric:: Ill-formed JSON Job Descriptions

If you run the jobSlotReservation tool with a syntactically incorrect JSON description, you will see the following messages::

  decoder plugin function=0 fail to run the function
  scaler plugin function=0 fail to run the function
  encoder plugin function=0 fail to run the function

This indicates that the job description is ill-formed and needs to be corrected.

.. _using-ffmpeg-launcher:

The FFmpeg Launcher Example
==================================

The FFmpeg launcher is an example application which automates the dispatching of FFmpeg jobs across multiple devices. It simplifies the process of manually setting up XRM reservation IDs and launching FFmpeg for many video streams. The FFmpeg launcher takes a transcode job description, input source files, corresponding FFmpeg run commands and automatically launches child FFmpeg processes based on the job slot availability on the server. In case there are more input streams listed than available job slots, the excess are queued and launched when a job slot becomes available. Note that only a single launcher per server is supported.

.. note::
   The FFmpeg launcher is only an example application. It is provided as an illustration of how an orchestration layer can use Job Descriptions, but it is not an official feature of the |SDK|.

The following steps show how to use the FFmpeg launcher for an ABR transcode use case with the |SDK|. In this use case, one encoded stream is transcoded to five unique renditions based on resolution, bit rate, and other variations.

#. Environment setup ::

    source /opt/xilinx/xcdr/setup.sh

#. To run the FFmpeg launcher, use the following command: ::

    launcher <source files file name> <run params file name>

   Here is an example of the command: ::

    launcher sources.txt /opt/xilinx/launcher/scripts/run_params/Run_ABR_h264_lowLatencyTranscode_mr_null.txt

   ``sources.txt`` is a text file that lists the input stream names. The launcher parses this list and inserts the sources in the FFmpeg command after ``-i`` one after another and launches them as a separate process. A sample ``sources.txt`` is as follows. ::

    # List all sources here 
    Input1.mp4 
    Video.flv 
    Input2.h264

   ``run_params.txt`` is a text file that consists of two fields. The first field is the video transcode job description, and the second field is the FFmpeg command line that needs to be launched and is matching the described job. Description of the job is given through a json file as described already in the previous sections. A sample ``run_params.txt`` is as follows. ::

    job_description = /opt/xilinx/launcher/scripts/describe_job/describe_job.json 
    cmdline = ffmpeg -c:v mpsoc_vcu_h264 -i -filter_complex "multiscale_xma=:outputs=4:out_1_width=1280:out_1_height=720:out_1_pix_fm t=vcu_nv12:out_2_width=848:out_2_height=480:out_2_pix_fmt=vcu_nv12:out_3_ width=640:out_3_height=360:out_3_pix_fmt=vcu_nv12:out_4_width=288:out_4_h eight=160:out_4_pix_fmt=vcu_nv12 [a][b][c][d]; [a]split[aa][ab]" -map '[aa]' -b:v 4M -max-bitrate 4M -c:v mpsoc_vcu_h264 -f h264 -y out_720p60.264 -map '[ab]' -r 30 -b:v 3M -max-bitrate 3M -c:v mpsoc_vcu_h264 -f h264 -y out_720p30.264 -map '[b]' -r 30 -b:v 2500K - max-bitrate 2500K -c:v mpsoc_vcu_h264 -f h264 -y out_480p30.264 -map '[c]' -r 30 -b:v 1250K -max-bitrate 1250K -c:v mpsoc_vcu_h264 -f h264 -y out_360p30.264 -map '[d]' -r 30 -b:v 625K -max-bitrate 625K -c:v mpsoc_vcu_h264 -f h264 -y out_160p30.264

|

.. _xrm-reference:
.. _xrmadm-and-xrmd-commands:

*****************************************
XRM Reference Guide
*****************************************

The Xilinx® FPGA resource manager (XRM) is the software which manages the hardware accelerators available in the system. XRM includes the following components:

- ``xrmd``: the XRM daemon, a background process supporting reservation, allocation, and release of hardware acceleration resources. 
- ``xrmadm`` the command line tool is used to interact with the XRM daemon (``xrmd``). 
- a C Application Programming Interface (API)

Command Line Interface
=========================================

The XRM ``xrmadm`` command line tool is used to interact with the XRM daemon (``xrmd``). It provides the following capabilities and uses a JSON file as input for each action:

- Generate status reports for each device
- Load and unload the hardware accelerators
- Load and unload the software plugins

The XRM related files are installed under ``/opt/xilinx/xrm/`` and device-specific XRM commands are available at ``/opt/xilinx/xcdr/scripts/xrm_commands/``.



Setup
-----------------------------------------

When sourced, the ``/opt/xilinx/xcdr/setup.sh`` script takes care of setting up the enviroment for the |SDK|, including its XRM components:

- The XRM daemon (``xrmd``) is started 
- The hardware accelerators (xclbin) and software plugins are loaded on the Xilinx devices


Generating Status Reports
-----------------------------------------

``xrmadm`` can generate reports with the status of each device in the system. This capability is particularly useful to check the loading of each hardware accelerator.

To generate a report for all the devices in the system::

  xrmadm /opt/xilinx/xrm/test/list_cmd.json


To generate a report for a single device specified in the json file::

  xrmadm /opt/xilinx/xrm/test/list_onedevice.json


A sample JSON file for generating a report for device 0 is shown below::

    {
        "request": {
            "name": "list",
            "requestId": 1,
            "device": 0
        }
    }


Loading/Unloading Hardware Accelerators
-----------------------------------------

``xrmadm`` can be used to load or unload the hardware accelerators on the programmable devices of the Alveo U30 card. The hardware accelerators must be reloaded after rebooting a card.

To load the hardware accelerators on a given device::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/load_multiple_devices/load_device0_cmd.json

To unload the hardware accelerators from a given device::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/unload_multiple_devices/unload_device_0_cmd.json

A sample JSON file for loading two devices (0 and 1) is shown below::

    {
        "request": {
            "name": "load",
            "requestId": 1,
            "parameters": [
                {
                "device": 0,
                "xclbin": "/opt/xilinx/xcdr/xclbins/transcode.xclbin"
                },
                {
                "device": 1,
                "xclbin": "/opt/xilinx/xcdr/xclbins/transcode.xclbin"
                }
            ]
        }
    }


Loading/Unloading Software Plugins
-----------------------------------------

``xrmadm`` can be used to load or unload the software plugins required to manage the compute resources. The software plugins perform resource management functions such as calculating CU load and CU max capacity. Once a plugin is loaded, it becomes usable by a host application through the XRM APIs. The XRM plugins need to be loaded before executing an application (such as FFmpeg) which relies on the plugins.

To load the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/load_multi_u30_xrm_plugins_cmd.json


To unload the plugins::

  xrmadm /opt/xilinx/xcdr/scripts/xrm_commands/unload_multi_u30_xrm_plugins_cmd.json


Controlling the xrmd Daemon
-----------------------------------------
The following commands can be used to start, stop, restart, or get the status of the daemon::

    sudo /opt/xilinx/xrm/tools/start_xrmd.sh
    sudo /opt/xilinx/xrm/tools/stop_xrmd.sh
    sudo /opt/xilinx/xrm/tools/restart_xrmd.sh
    sudo systemctl status xrmd


C Application Programming Interface
=========================================

XRM provides a C Application Programming Interface (API) to reserve, allocate and release CUs from within a custom application. For complete details about this programming interface, refer to the :ref:`XRM API Reference Guide <xrm-api-reference>` section of the documentation. 




..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
