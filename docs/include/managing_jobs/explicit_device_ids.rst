.. _using-explicit-device-ids:

**********************************
Assigning Jobs to Specific Devices
**********************************

By default a job is submitted to device 0. When running multiple jobs in parallel, device 0 is bound to run out of resources rapidly and additional jobs will error out due to insufficient resources. By using explicit device identifiers, the different components (decoder, scaler, encoder) of a job can be individually submitted to a specific device. This makes it easy and straightforward to leverage the entire video acceleration capacity of your system, regardless of the number of cards and devices. 

The FFmpeg :option:`-lxlnx_hwdev` and GStreamer :option:`dev-idx` options can be used to specify the device on which a specific job component (decoder, scaler, encoder) should be run. This makes it possible to split a job across two devices by mapping some components of the job to one device and other components of the job to another device. This ability is useful when there are not enough compute resources to run the entire job on a single device. Typical use cases include 4K ladders or 1080p ladders with more outputs that can be scaled or encoded on a single device. When splitting a job across two devices, the video frames must be transfered from the first device to the host, and then from the host to the second device. When using FFmpeg, this transfer must be done explicitly using the :option:`xvbm_convert` filter. When using GStreamer, this transfer is handled automatically by the plugins.

The FFmpeg :option:`-xlnx_hwdev` option is similar to the :option:`-lxlnx_hwdev` option, but it applies globally instead of locally. This option can used to specify the device on which all components of a given job should be run. The :option:`-xlnx_hwdev` and :option:`-lxlnx_hwdev` options can be used together. If present, the :option:`-xlnx_hwdev` option sets the default device index for all job components. Any subsequent :option:`-lxlnx_hwdev` option overwrites the device index for the particular component that it is set on.

Determining on which device(s) to run a job can be done using either the :ref:`manual <manual-resource-management>` or :ref:`automated <using-job-descriptions>` methods described in the following sections.


Performance Considerations
==================================

For higher performance, Xilinx recommends running all the different components on the same device whenever possible. Splitting a job across multiple devices leads to extra data transfers to and from the host which may impact total performance.


Examples using Explicit Device IDs
==================================


.. rubric:: FFmpeg example of two different jobs run on two different devices

In this example, two different FFmpeg jobs are run in parallel. The :option:`-xlnx_hwdev` option is used to submit each job to a different device::

  ffmpeg -xlnx_hwdev 0 -c:v mpsoc_vcu_h264 -i INPUT1.h264 -f mp4 -c:v mpsoc_vcu_hevc -y /dev/null &
  ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i INPUT2.h264 -f mp4 -c:v mpsoc_vcu_hevc -y /dev/null &


.. rubric:: FFmpeg example of a 4K ladder split across two devices

In this example, a 4K ABR ladder with 6 outputs is split across two devices, as shown in the picture below:

.. figure:: ./images/4k_ladder_on_two_devices.png
   :alt: 4K Ladder Running on Two Devices
   :align: center

   4K ABR ladder split across two devices

This 4K ladder can be implemented in FFmpeg with the command below. The :option:`-lxlnx_hwdev` option is used to map each component of the FFmpeg job to the desired device. Two :option:`multiscale_xma` filters are used since there are scaling operations on both devices. The output of the scaler on device 0 is transfered back to the host with the :option:`xvbm_convert` filter. The same data is then automatically copied to the other device since it is used by the second :option:`multiscale_xma` filter::

  ffmpeg -re -lxlnx_hwdev 0 -c:v mpsoc_vcu_h264 -i INPUT.h264 -max_muxing_queue_size 1024 -filter_complex " \
    [0]split[a][b]; \
    [b]multiscale_xma=outputs=1:lxlnx_hwdev=0:out_1_width=1920:out_1_height=1080:out_1_rate=full [c]; [c]xvbm_convert[d]; \
    [d]multiscale_xma=outputs=4:lxlnx_hwdev=1:out_1_width=1280:out_1_height=720:out_1_rate=full: \
                                              out_2_width=848:out_2_height=480:out_2_rate=half: \
                                              out_3_width=640:out_3_height=360:out_3_rate=half: \
                                              out_4_width=288:out_4_height=160:out_4_rate=half [e][f][g][h]; \
    [e]split[i][j]; [j]fps=30[k]" \
    -map '[a]' -lxlnx_hwdev 0 -b:v 16M   -max-bitrate 16M   -level 4.2 -slices 4 -cores 4 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null \
    -map '[i]' -lxlnx_hwdev 1 -b:v 4M    -max-bitrate 4M    -level 4.2 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null \
    -map '[k]' -lxlnx_hwdev 1 -b:v 3M    -max-bitrate 3M    -level 4.2 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null \
    -map '[f]' -lxlnx_hwdev 1 -b:v 2500K -max-bitrate 2500K -level 4.2 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null \ 
    -map '[g]' -lxlnx_hwdev 1 -b:v 1250K -max-bitrate 1250K -level 4.2 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null \
    -map '[h]' -lxlnx_hwdev 1 -b:v 625K  -max-bitrate 625K  -level 4.2 -c:v mpsoc_vcu_h264 -c:a copy -max_interleave_delta 0 -f mp4 -y /dev/null


.. rubric:: GStreamer example of a transcoding pipeline split across two devices

In this example, the :option:`dev-idx` option is used to specify that the decoder should run on device 0 and that the encoder should run on device 1
::

  gst-launch-1.0 filesrc location=bbb_sunflower_1080p_60fps_normal.264 ! h264parse ! vvas_xvcudec dev-idx=0 ! queue ! vvas_xvcuenc dev-idx=1 target-bitrate=8000 max-bitrate=8000 ! video/x-h264 ! fpsdisplaysink video-sink="filesink location=/tmp/xil_xcode_out_0.mp4" text-overlay=false sync=false -v


.. rubric:: GStreamer example of a 4K ladder split across two devices

The GStreamer equivalent of the FFmpeg example described above is shown below. It uses :option:`dev-idx` to map each component to the desired device::

  gst-launch-1.0 filesrc num-buffers=-1 location=~/videos/bbb_sunflower_2160p_60fps_normal.mp4 \
        ! qtdemux ! queue ! h264parse \
        ! vvas_xvcudec dev-idx=0 avoid-output-copy=1 \
        ! queue \
        ! tee name=dectee \
        dectee. \
                ! queue \
                ! vvas_xvcuenc name=enc_2160p60_dev0 dev-idx=0 b-frames=2 target-bitrate=16000 max-bitrate=16000 num-slices=4 num-cores=4 \
                ! queue ! h264parse ! qtmux \
                ! fpsdisplaysink name=sink_xcode_scale_2160p60_dev0_83 video-sink="filesink location=/tmp/xil_xcode_scale_2160p60_dev__0.mp4" async=false text-overlay=false sync=false \
        dectee. \
                ! queue \
                ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 enable-pipeline=1 avoid-output-copy=true name=sc1_0 \
                ! queue ! video/x-raw, width=1920, height=1080 \
                ! queue \
                ! tee name=sctee \
                sctee. \
                        ! queue \
                        ! vvas_xvcuenc name=enc_1080p60_dev1 dev-idx=1 target-bitrate=6000 max-bitrate=6000 \
                        ! queue ! h264parse ! qtmux \
                        ! fpsdisplaysink name=sink_xcode_scale_1080p60_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_1080p60_dev__1.mp4" async=false text-overlay=false sync=false \
                sctee. \
                        ! queue \
                        ! vvas_xabrscaler dev-idx=1 ppc=4 scale-mode=2 avoid-output-copy=true name=sc2_1 \
                        sc2_1.src_0 \
                                ! queue ! video/x-raw, width=1280, height=720 \
                                ! tee name=tee_1 \
                                tee_1. \
                                  ! queue ! videorate ! video/x-raw, framerate=60/1 \
                                  ! vvas_xvcuenc name=enc_720p60_dev1 dev-idx=1 target-bitrate=4000 \
                                  ! queue ! h264parse ! qtmux \
                                  ! fpsdisplaysink name=sink_xcode_scale_720p60_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_720p60_dev__1.mp4" async=false text-overlay=false sync=false \
                               tee_1. \
                                  ! queue ! videorate ! video/x-raw, framerate=30/1 \
                                  ! vvas_xvcuenc name=enc_720p30_dev1 dev-idx=1 target-bitrate=3000 \
                                  ! queue ! h264parse ! qtmux \
                                  ! fpsdisplaysink name=sink_xcode_scale_720p30_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_720p30_dev__1.mp4" async=false text-overlay=false sync=false \
                        sc2_1.src_1 \
                                ! queue ! video/x-raw, width=848, height=480 \
                                ! videorate ! video/x-raw, framerate=30/1 \
                                ! vvas_xvcuenc name=enc_480p30_dev1 dev-idx=1 target-bitrate=2500 \
                                ! queue ! h264parse ! qtmux \
                                ! fpsdisplaysink name=sink_xcode_scale_480p30_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_480p30_dev__1.mp4" async=false text-overlay=false sync=false \
                        sc2_1.src_2 \
                                ! queue ! video/x-raw, width=640, height=360 \
                                ! videorate ! video/x-raw, framerate=30/1 \
                                ! vvas_xvcuenc name=enc_360p30_dev1 dev-idx=1 target-bitrate=1250 \
                                ! queue ! h264parse ! qtmux \
                                ! fpsdisplaysink name=sink_xcode_scale_360p30_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_360p30_dev__1.mp4" async=false text-overlay=false sync=false \
                        sc2_1.src_3 \
                                ! queue ! video/x-raw, width=288, height=160 \
                                ! videorate ! video/x-raw, framerate=30/1 \
                                ! vvas_xvcuenc name=enc_160p30_dev1 dev-idx=1 target-bitrate=625 \
                                ! queue ! h264parse ! qtmux \
                                ! fpsdisplaysink name=sink_xcode_scale_160p30_dev1_83 video-sink="filesink location=/tmp/xil_xcode_scale_160p30_dev__1.mp4" async=false text-overlay=false sync=false -v


**NOTE**: Use ``enable-pipeline=1`` whenever there is a data copy required. In this example, on vvas_xbarscaler where data is copied from the host to the second device.


.. rubric:: Additional Examples

Additional examples of how to run multiple processes using explicit device IDs can be found in the tutorials included in this repository:

- :ref:`FFmpeg Tutorials <ffmpeg-device-id-examples>` 
- :ref:`GStreamer Tutorials <gstreamer-device-id-examples>`


..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
