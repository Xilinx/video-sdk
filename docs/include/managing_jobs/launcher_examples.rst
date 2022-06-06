.. _launcher-examples:

Automated Job Launching
=======================

The Job Slot Reservation tool automatically reserves job slots, but actual jobs still need to be manually launched using the generated reservations IDs. It is possible to create custom orchestration layers to automatically handle the reservation of job slots and the launching of jobs.

The |SDK| includes two examples of "launcher" applications for FFmpeg and GStreamer, respectively. Source code for the FFmpeg Launcher example, the GStreamer Launcher and the Job Slot Reservation tool are included in the Github repository of |SDK| and can be used as a starting point for developing custom orchestration layers. 

.. _using-ffmpeg-launcher:

The FFmpeg Launcher Example
---------------------------

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


.. _using-gstreamer-launcher:

The GStreamer Launcher Example
------------------------------

The GStreamer launcher is an example application which automates the dispatching of GStreamer jobs across multiple devices. It simplifies the process of manually setting up XRM reservation IDs and launching GStreamer for many video streams. The GStreamer launcher takes a transcode job description, input source files, corresponding GStreamer run commands and automatically launches child GStreamer processes based on the job slot availability on the server. In case there are more input streams listed than available job slots, the excess are queued and launched when a job slot becomes available. Note that only a single launcher per server is supported.

.. note::
   The GStreamer launcher is only an example application. It is provided as an illustration of how an orchestration layer can use Job Descriptions, but it is not an official feature of the |SDK|.

The following steps show how to use the GStreamer launcher for an ABR transcode use case with the |SDK|. In this use case, one encoded stream is transcoded to five unique renditions based on resolution, bit rate, and other variations.

#. Environment setup ::

    source /opt/xilinx/xcdr/setup.sh

#. To run the GStreamer launcher, use the following command: ::

    launcher <source files file name> <run params file name>

   Here is an example of the command: ::

    launcher sources.txt /opt/xilinx/launcher/scripts/vvas_run_params/Run_1080p60_ABR_h264_lowLatencyTranscode_mr_null.txt

   ``sources.txt`` is a text file that lists the input stream names. The launcher parses this list and inserts the sources in the GStreamer command one after another and launches them as a separate process. A sample ``sources.txt`` is as follows. ::

    # List all sources here 
    Input1.mp4 
    Video.flv 
    Input2.h264

   ``run_params.txt`` is a text file that consists of two fields. The first field is the video transcode job description, and the second field is the GStreamer command line that needs to be launched and is matching the described job. Description of the job is given through a json file as described already in the previous sections. A sample ``run_params.txt`` is as follows. ::

    job_description = /opt/xilinx/launcher/scripts/describe_job/describe_job.json 
    cmdline = gst-launch-1.0 filesrc ! h264parse ! vvas_xvcudec num-entropy-buf=2 ! queue max-size-buffers=1   ! vvas_xabrscaler kernel-name="scaler:scaler_1" scale-mode=2 name=sc sc.src_0 ! queue max-size-buffers=1   ! video/x-raw, width=1280, height=720, format=NV12 ! queue max-size-buffers=1   ! tee name=t t. ! queue max-size-buffers=1   ! videorate ! video/x-raw, framerate=60/1 ! vvas_xvcuenc target-bitrate=4000 max-bitrate=4000 ! h264parse ! video/x-h264, profile=high, level=(string)4.2 ! fpsdisplaysink name=sink_720p60 video-sink="fakesink async=false" text-overlay=false fps-update-interval=5000 sync=false t. ! queue max-size-buffers=1   ! videorate ! video/x-raw, width=1280, height=720, framerate=30/1 ! vvas_xvcuenc target-bitrate=3000 max-bitrate=3000 ! h264parse ! video/x-h264, profile=high, level=(string)4.2 ! fpsdisplaysink name=sink_720p30 video-sink="fakesink async=false" text-overlay=false fps-update-interval=5000 sync=false sc.src_1 ! queue max-size-buffers=1   ! video/x-raw, width=848, height=480, format=NV12 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xvcuenc target-bitrate=2500 max-bitrate=2500 ! h264parse ! video/x-h264, profile=high, level=(string)4.2 ! fpsdisplaysink name=sink_480p30 video-sink="fakesink async=false" text-overlay=false fps-update-interval=5000 sync=false sc.src_2 ! queue max-size-buffers=1   ! video/x-raw, width=640, height=360, format=NV12 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xvcuenc target-bitrate=1250 max-bitrate=1250 ! h264parse ! video/x-h264, profile=high, level=(string)4.2 ! fpsdisplaysink name=sink_360p30 video-sink="fakesink async=false" text-overlay=false fps-update-interval=5000 sync=false sc.src_3 ! queue max-size-buffers=1   ! video/x-raw, width=288, height=160, format=NV12 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xvcuenc target-bitrate=625 max-bitrate=625 ! h264parse ! video/x-h264, profile=high, level=(string)4.2 ! fpsdisplaysink name=sink_160p30 video-sink="fakesink async=false" text-overlay=false fps-update-interval=5000 sync=false -v


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.