#############################
FFmpeg Introductory Tutorials
#############################

.. highlight:: none

This page provides tutorials on how to use FFmpeg with the |SDK|. The complete reference guide for the FFmpeg version included in the |SDK| can be found :doc:`here </using_ffmpeg>`.

The tutorials break down the commands, starting with simple steps using a single device. These are built upon to show 4K, faster than real-time, and multiple operations on the same device.


.. contents:: Table of Contents
    :local:
    :depth: 3
.. .. section-numbering::


*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst

The setup script exports important environment variables, starts the Xilinx Resource Manager (XRM) daemon, and ensures that the Xilinx devices and the XRM plugins are properly loaded. It also moves to the top of the system PATH the FFmpeg binary provided as part of the |SDK|.

Sourcing the setup script should be performed each time you open a new terminal on your system. This is required for the environment to be correctly configured. 

|

**********************
Simple FFmpeg Examples
**********************

Some of the examples read or write RAW files from disk (encode-only or decode-only pipelines). There is a chance that due to the massive bandwidth required for operating on these RAW files, you will notice a drop in FPS; this is not due to the |SDK| but the disk speeds. We recommend reading/writing from ``/dev/shm`` which is a RAM disk.


.. _decode-only:

Decode Only
===========
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/01_ffmpeg_decode_only.sh`

**Usage**::

    ./01_ffmpeg_decode_only.sh <1080p60 H.264 file>

This example accepts a clip that is already encoded in H.264, and will decode the file into a RAW format and save it to disk.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i <INPUT> \
    -vf xvbm_convert -pix_fmt yuv420p -y /tmp/xil_dec_out.yuv

Explanation of the flags:

- ``ffmpeg``

  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated decoder in the Xilinx device

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-vf xvbm_convert``

  + Internally, the decoder operates on Xilinx-typed buffers to improve performance and enable scalable options for future accelerated filters. To convert back to a host-buffer, you must execute this filter.

- ``-pix_fmt yuv420p``

  + We need to define the color space in the output

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_dec_out.yuv``

  + The decoder will save the file to the path above



Encode Only
===========
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/02_ffmpeg_encode_only_1080p.sh`

**Usage**::

    ./02_ffmpeg_encode_only_1080p.sh <1080p60 YUV file>

This example accepts a RAW 1080p60 clip in YUV420 format. It will pass the clip to the encoder to produce an H.264 encoded MP4 output with a target bitrate of 8Mbps and save it to disk.

**Command Line**::

    ffmpeg -f rawvideo -s 1920x1080 -r 60 -pix_fmt yuv420p -i <INPUT> \
    -b:v 8M -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_enc_out.mp4

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-f rawvideo``
  
  + This signifies that the video is in a raw format, without container or other metadata/information about the clip

- ``-s 1920x1080``

  + Since there is no container or metadata in a RAW clip, the user must define the input clip's resolution/size. This example states the input is 1080p

- ``-r 60``

  + Again, without metadata, the encoder requires the framerate of the incoming stream

- ``-pix_fmt yuv420p``

  + The color space of the encoder is by default yuv420p. this example is defining the input clip as being this same color space 

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-f mp4``

  + Sets the output video container to MP4

- ``-b:v 8M``

  + The target bitrate of the encoded stream. 8M signifies a target bitrate of 8 Megabits per second. You can also use 8000K or 8000000.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated encoder in the Xilinx device

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_enc_out.mp4``

  + Save the output in the path above
    
Basic Transcode
===============
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/03_ffmpeg_transcode_only.sh`

**Usage**::

    ./03_ffmpeg_transcode_only.sh <1080p60 H.264 file>

This example takes an H.264 clip and reencodes it to H.264 with a new bitrate of 8Mbps. The output is written into :file:`/tmp/xil_xcode.mp4`. 

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i <INPUT> \
    -f mp4 -b:v 8M -c:v mpsoc_vcu_h264 -y /tmp/xil_xcode.mp4 

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated decoder in the Xilinx device

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-b:v 8M``

  + The target bitrate of the encoded stream. 8M signifies a target bitrate of 8 Megabits per second. You can also use 8000K or 8000000.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated encoder in the Xilinx device

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_xcode.mp4``

  + This is the output path; most scripts will route here. Change to any output path at your discretion.


.. _decode-and-scale-only:

Decode Only Into Multiple-Resolution Outputs
============================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/04_ffmpeg_decode_plus_scale.sh`

**Usage**::

    ./04_ffmpeg_decode_plus_scale.sh <1080p60 h264 clip>
    
This example decodes an existing H.264 file and then scales it into multiple resolutions as defined below. It will not re-encode them, but save the RAW outputs to disk under ``/tmp/xil_dec_scale<res>.yuv``

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]; \
    [aa]xvbm_convert[aa1];[abb]xvbm_convert[abb1];[b]xvbm_convert[b1];[c]xvbm_convert[c1]; \
    [d]xvbm_convert[d1]" \
    -map "[aa1]"  -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_720p60.yuv \
    -map "[abb1]" -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_720p30.yuv \
    -map "[b1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_480p30.yuv \
    -map "[c1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_360p30.yuv \
    -map "[d1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_288p30.yuv

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated decoder in the Xilinx device

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-filter_complex``

  + The FFmpeg ``-filter_complex`` flag allows combining multiple filters together using a graph-like syntax. This example uses the :option:`multiscale_xma`, ``split``, ``fps`` and ``xvbm_convert`` filters to create 5 output resolutions from the input stream.
  + The :option:`multiscale_xma` filter configures the Xilinx hardware-accelerated scaler to produce 4 output resolutions (1280x720p60, 848x480p30, 640x360p30, and 288x160p30). For each output, the width, height and frame rate are defined with ``out_<n>_width``, ``out_<n>_height`` and ``out_<n>_rate``. The 4 outputs of the :option:`multiscale_xma` filter are identified as ``a``, ``b``, ``c`` and ``d`` respectively. 
  + The ``split`` and ``fps`` software filters are used to split the ``a`` stream into ``aa`` and ``ab`` and then drop the framerate of ``ab`` to 30 fps to produce the ``abb`` 1280x720p30 stream.
  + The :option:`xvbm_convert` filters are used to transfer the outputs of the hardware scaler back to the host and convert them to AV frames for further processing by FFmpeg

- ``-map "[ID]"``

  + Selects an output of the filter graph. The flags that follow apply to the selected stream.

- ``-pix_fmt yuv420p``

  + Use a yuv420p output format

- ``-f rawvideo``

  + This tells ffmpeg to output the video into a RAW video file

- ``/tmp/xil_dec_scale_<resolution><fps>.yuv``

  + Save the output files to the paths listed


.. _encode-only-multiple-res-outputs:


Encode Only Into Multiple-Resolution Outputs
============================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/05_ffmpeg_encode_plus_scale_1080p.sh`

**Usage**::
    
    ./05_ffmpeg_encode_plus_scale_1080p.sh <1080p60 YUV file>

This example takes a raw 1080p60 YUV file, scales it down to different resolutions and frame rates, encodes each of the scaled streams to H.264 and saves them to disk under :file:`xil_scale_enc_<resolution>.mp4`

**Command Line**::

    ffmpeg -f rawvideo -s 1920x1080 -r 60 -pix_fmt yuv420p -i $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720: out_1_rate=full:   \
    out_2_width=848:  out_2_height=480: out_2_rate=half:   \
    out_3_width=640:  out_3_height=360: out_3_rate=half:   \
    out_4_width=288:  out_4_height=160: out_4_rate=half    \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]"  \
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_720p60.mp4 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_720p30.mp4 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_480p30.mp4 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_360p30.mp4 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_288p30.mp4

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-f rawvideo``
  
  + This signifies that the video is in a raw format, without container or other metadata/information about the clip

- ``-s 1920x1080``

  + Since there is no container or metadata in a RAW clip, the user must define the input clip's resolution/size. This example states the input is 1080p

- ``-r 60``

  + Without metadata, the encoder requires the framerate of the incoming stream


- ``-i <INPUT>``

  + The input file to be transcoded

- ``-filter_complex``

  + The FFmpeg ``-filter_complex`` flag allows combining multiple filters together using a graph-like syntax. This example uses the :option:`multiscale_xma`, ``split`` and ``fps`` filters to create 5 output resolutions from the input stream.
  + The :option:`multiscale_xma` filter configures the Xilinx hardware-accelerated scaler to produce 4 output resolutions (1280x720p60, 848x480p30, 640x360p30, and 288x160p30). For each output, the width, height and frame rate are defined with ``out_<n>_width``, ``out_<n>_height`` and  ``out_<n>_rate``. The 4 outputs of the :option:`multiscale_xma` filter are identified as ``a``, ``b``, ``c`` and ``d`` respectively. 
  + The ``split`` and ``fps`` software filters are used to split the ``a`` stream into ``aa`` and ``ab`` and then drop the framerate of ``ab`` to 30 fps to produce the ``abb`` 1280x720p30 stream.

- ``-map "[ID]"``

  + Selects an output of the filter graph. The flags that follow apply to the selected stream.

- ``-b:v <SIZE>``

  + The flag signifies the desired output bitrate for each mapped stream

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated encoder in the Xilinx device

- ``-f mp4``

  + Sets the output video container to MP4

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_scale_enc_<resolution><fps>.mp4``

  + Saves the output clips to the location listed

.. _transcode-with-abr-ladder:

Transcode With Multiple-Resolution Outputs
==========================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/06_ffmpeg_transcode_plus_scale.sh`

**Usage**::
    
    ./06_ffmpeg_transcode_plus_scale.sh <1080p60 h264 clip>   


This example implements a complete transcoding pipeline on an 1080p60 H.264 input. It decodes the input stream, scales it down to different resolutions and frame rates, encodes each of the scaled streams to H.264 and saves them to disk under :file:`xil_xcode_scale_<resolution>.mp4`

The command included in the script doesn't handle the audio channel of the input video. For an example of how to include audio in the output streams, refer to the example commented out at the bottom of the script and to the section of the documentation about :ref:`Mapping Audio Streams <mapping-audio-streams>`.


**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720: out_1_rate=full: \
    out_2_width=848:  out_2_height=480: out_2_rate=half: \
    out_3_width=640:  out_3_height=360: out_3_rate=half: \
    out_4_width=288:  out_4_height=160: out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_720p60.mp4 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_720p30.mp4 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_480p30.mp4 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_360p30.mp4 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_288p30.mp4

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated decoder in the Xilinx device

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-filter_complex``

  + The FFmpeg ``-filter_complex`` flag allows combining multiple filters together using a graph-like syntax. This example uses the :option:`multiscale_xma`, ``split`` and ``fps`` filters to create 5 output resolutions from the input stream along with the corresponding audio streams.
  + The :option:`multiscale_xma` filter configures the Xilinx hardware-accelerated scaler to produce 4 output resolutions (1280x720p60, 848x480p30, 640x360p30, and 288x160p30). For each output, the width, height and frame rate are defined with ``out_<n>_width``, ``out_<n>_height`` and  ``out_<n>_rate``. The 4 outputs of the :option:`multiscale_xma` filter are identified as ``a``, ``b``, ``c`` and ``d`` respectively. 
  + The ``split`` and ``fps`` software filters are used to split the ``a`` stream into ``aa`` and ``ab`` and then drop the framerate of ``ab`` to 30 fps to produce the ``abb`` 1280x720p30 stream.

- ``-map "[ID]"``

  + Selects a video output of the filter graph. The flags that follow apply to the selected stream.

- ``-b:v <SIZE>``

  + The flag signifies the desired output bitrate for each mapped stream

- ``-c:v mpsoc_vcu_h264``

  + Selects an audio output of the filter graph. The selected audio stream will be combined with the selected video stream. 

- ``-f mp4``

  + Sets the output video container to MP4

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_scale_enc_<resolution><fps>.mp4``

  + Saves the output clips to the location listed

Ultra Low-Latency Transcode Pipeline
====================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/07_ffmpeg_transcode_lowlatency.sh`

**Usage**::

    ./07_ffmpeg_transcode_lowlatency.sh

This example utilizes low latency flags in both the decoder via :option:`-low_latency` along with :option:`-splitbuff_mode` and encoder via :option:`-disable-pipeline` along with :option:`-avc-lowlat`.

**Command Line**::

        ffmpeg -c:v mpsoc_vcu_h264  -low_latency 1 -splitbuff_mode 1 -latency_logging 1 \
         -i $1 -c:v mpsoc_vcu_h264  -disable-pipeline 1 -avc-lowlat 0 \
         -bf 0 -latency_logging 1 -f mp4 /tmp/low_latency.mp4

Explanation of the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated decoder in the Xilinx device

- ``-low_latency 1``
  
  + This flag enables low-latency decoding
  
  + **B-frames are not supported in this mode**.

  + Remove ``-low_latency 1`` from the command line if your input has B-Frames

- ``-splitbuff_mode 1``

  + This flag configures the decoder in split/unsplit input buffer mode, which reduces latency by handing off buffers to the next pipeline stage earlier. 

  + This flag must be enabled together with the ``low_latency`` one to reduce decoding latency.

- ``-disable-pipeline``

  + This flag enables Ultra Low Latency in the encoder.

- ``-avc-lowlat``

  + This flag is needed, when encoding to AVC. See :option:`-avc-lowlat`.

- ``-bf 0``
  
  + The number of b-frames inserted in the output stream not only increases encode latency in the Xilinx device, but decode latency on the player. Setting it to 0 removes them.

- ``-latency_logging 1``
  
  + The option, which is applicable to both the encoder and decoder, enables latency specific logs, observable via journalctl.


**********************
Encoding Streams to 4K
**********************

The |SDK| supports real-time decoding and encoding of 4k streams with the following notes:

- The Xilinx video pipeline is optimized for live-streaming use cases. For 4k streams with bitrates significantly higher than the ones typically used for live streaming, it may not be possible to sustain real-time performance.
- When decoding 4k streams with a high bitrate, increasing the number of entropy buffers using the :option:`-entropy_buffers_count` option can help improve performance
- When encoding raw video to 4k, set the :option:`-s` option to ``3840x2160`` to specify the desired resolution.
- When encoding 4k streams to H.264, the :option:`-slices` option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.


4k H.264 Real-Time Encode Only
==============================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/08_ffmpeg_encode_only_4k.sh`

**Usage**::
    
    ./08_ffmpeg_encode_only_4k.sh <2160p60 YUV file>

This example takes an 8-bit, YUV420, 2160p60 RAW file, encodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_4k_enc_out.mp4`. The :option:`-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::

    ffmpeg -f rawvideo -s 3840x2160 -r 60 -pix_fmt yuv420p -i <INPUT> \
    -b:v 20M -c:v mpsoc_vcu_h264 -slices 4 -f mp4 -y /tmp/xil_4k_enc_out.mp4

4k H.264 Real-Time Transcode
============================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/09_ffmpeg_transcode_only_4k.sh`

**Usage**::
    
    ./09_ffmpeg_transcode_only_4k.sh <2160p60 HEVC file>

This example takes an 2160p60 HEVC file, transcodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_4k_enc_out.mp4`. The :option:`-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_hevc -i <INPUT> \
    -b:v 20M -c:v mpsoc_vcu_h264 -slices 4 -f mp4 -y /tmp/xil_4k_xcode.mp4 

|

.. _ffmpeg-device-id-examples:

***************************
Running on Multiple Devices
***************************

Explicit Device Management
==========================

The |SDK| supports running multiple jobs simultaenously on a given device if the overall throughput does not exceed an aggregate load of 4K pixels at 60 frames per second. It is also possible to running multiple jobs across multiple devices when more than one device is available in the system.

This example shows how run multiple jobs in parallel while explicitly specifying on which device each job should be run in order to manage compture resources.

This script transcodes three H264 streams to HEVC, sending the outputs to :file:`/tmp/xil_xcode_{n}.mp4`. The three transcodes are run in parallel in individual xterms. The :option:`-xlnx_hwdev` option is used to control on which device each job is run. The first job is run on device #0 and the two others jobs are run on device #1. After the jobs are launched, a JSON system load report is generated.

.. note::
   This example leverages the ``xterm`` program. Make sure it is installed on your system before proceeding.


- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/10_ffmpeg_multiple_jobs.sh`

**Usage**::

    ./10_ffmpeg_multiple_jobs.sh <input_h264_1_mp4> <input_h264_2_mp4> <input_h264_3_mp4>

**Commands**::

  # Launch the three jobs in parallel
  xterm -fa mono:size=9 -e "ffmpeg -xlnx_hwdev 0 -c:v mpsoc_vcu_h264 -i $1 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_1.mp4; sleep 5s" &
  xterm -fa mono:size=9 -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $2 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_2.mp4; sleep 5s" &
  xterm -fa mono:size=9 -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $3 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_3.mp4; sleep 5s" &  

  # Wait until the jobs are started to generate a system load report
  sleep 2s
  xrmadm /opt/xilinx/xrm/test/list_cmd.json &


**Tutorial steps**

- Prepare 3 input H264 videos with the following resolutions: 4k60, 1080p60 and 720p30

- Confirm that there are a least two devices available in your system::

    xbutil examine

- Run the - - Example script  with the 3 input videos::

    ./10_ffmpeg_multiple_jobs.sh 4k60.mp4 1080p60.mp4 720p30.mp4

- The script opens three xterm windows and runs a transcode job in each of them. After 2 seconds, to ensure all jobs are running, the script executes the ``xrmadm /opt/xilinx/xrm/test/list_cmd.json`` command to generate a report of the system load.

- In each of the xterm windows, inspect the FFmpeg transcript and observe that it indicates on which device the job is run::
    
    device_id   :  0 

- Inspect the system load report (in JSON format) in the main terminal. For each device, the loading percentage is reported in the ``usedLoad`` field for each of the decoder, scaler, and encoder compute units. A value of 0 indicates that a particular resources is completely free. A value of 1000000 indicates that a particular resource is fully loaded and can no longer accept jobs. In the example shown below, the decoder is 25% utilized and can therefore accept more jobs. ::

    "cu_3": {
        "cuId         ": "3",
        "cuType       ": "IP Kernel",
        "kernelName   ": "decoder",
        "kernelAlias  ": "DECODER_MPSOC",
        "instanceName ": "decoder_1",
        "cuName       ": "decoder:decoder_1",
        "kernelPlugin ": "/opt/xilinx/xma_plugins/libvcu-xma-dec-plg.so",
        "maxCapacity  ": "497664000",
        "numChanInuse ": "1",
        "usedLoad     ": "250000 of 1000000",
        "reservedLoad ": "0 of 1000000",
        "resrvUsedLoad": "0 of 1000000"
    }

- Close the three xterm windows

- Now rerun the script with the input files in a different order::

    ./10_ffmpeg_multiple_jobs.sh 720p30.mp4 4k60.mp4 1080p60.mp4 

  This will try to simultaneously run the 4k60 and the 1080p60 jobs on device #1. The compute requirements of these two combined jobs will exceed the capacity of a single device. Only one of the two jobs will proceed and the second one will error out due to insufficient resources. 



.. _ffmpeg-tutorial-splitting-across-two-devices:

Splitting a Job across Two Devices
==================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/14_ffmpeg_multidevice_abr_ladder.sh`

**Usage**::

    ./14_ffmpeg_multidevice_abr_ladder.sh <4Kp60 HEVC clip>

This example builds upon the ABR ladder concepts presented in example #6 and the 4K considerations presented in #9. The script accepts a pre-encoded 4K60 file and generates 7 different output resolutions encoded to HEVC. The processing requirement of this job cannot be accomodated by a single device. This example shows how to split the job across two devices.

The first device is used to decode the input, encode it to 4K60 HEVC and scale it to 1080p60. The scaled 1080p60 output is sent to the second device, where it goes through an ABR ladder and is scaled and encoded into multiple resolutions. Scaling the 4K60 input  to 1080p60 on device 0 reduces the size of the buffer which needs to be transferred from device 0 to device 1, which is better for overall performance.

The 4K60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):

- Device 0:    4K60    16 Mbps
- Device 1: 1080p60     6 Mbps
- Device 1:  720p60     4 Mbps
- Device 1:  720p60     3 Mbps
- Device 1:  480p60  2500 Kbps
- Device 1:  360p60  1250 Kbps
- Device 1:  160p60   625 Kbps


**Command Line**::

    ffmpeg -re -c:v mpsoc_vcu_hevc -lxlnx_hwdev 0 -i $1 -max_muxing_queue_size 1024 \
    -filter_complex "[0]split=2[dec1][dec2]; \
                     [dec2]multiscale_xma=outputs=1:lxlnx_hwdev=0:out_1_width=1920:out_1_height=1080:out_1_rate=full[scal]; \
                     [scal]xvbm_convert[host]; [host]split=2[scl1][scl2]; \
                     [scl2]multiscale_xma=outputs=4:lxlnx_hwdev=1:out_1_width=1280:out_1_height=720:out_1_rate=full:\
                                                                  out_2_width=848:out_2_height=480:out_2_rate=half:\
                                                                  out_3_width=640:out_3_height=360:out_3_rate=half:\
                                                                  out_4_width=288:out_4_height=160:out_4_rate=half \
                     [a][b30][c30][d30]; [a]split[a60][aa];[aa]fps=30[a30]" \
    -map '[dec1]' -c:v mpsoc_vcu_hevc -b:v 16M   -max-bitrate 16M   -lxlnx_hwdev 0 -slices 4 -cores 4 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_4k.mp4 \
    -map '[scl1]' -c:v mpsoc_vcu_hevc -b:v 6M    -max-bitrate 6M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_1080p60.mp4               \
    -map '[a60]'  -c:v mpsoc_vcu_hevc -b:v 4M    -max-bitrate 4M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_720p60.mp4                \
    -map '[a30]'  -c:v mpsoc_vcu_hevc -b:v 3M    -max-bitrate 3M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_720p30.mp4                \
    -map '[b30]'  -c:v mpsoc_vcu_hevc -b:v 2500K -max-bitrate 2500K -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_480p30.mp4                \
    -map '[c30]'  -c:v mpsoc_vcu_hevc -b:v 1250K -max-bitrate 1250K -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_360p30.mp4                \
    -map '[d30]'  -c:v mpsoc_vcu_hevc -b:v 625K  -max-bitrate 625K  -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_160p30.mp4


Explanation of key flags not covered in previous examples:

- :option:`-lxlnx_hwdev` 

  + This option is used to specify on which device each specific operation must be executed. For more details about this option, refer to the documentation regarding :ref:`Assigning Jobs to Specific Devices <using-explicit-device-ids>`.

- :option:`xvbm_convert` 

  + This filter is used to transfer frame buffers from a device back to the host. In this example, the buffers are then automatically transfered to the other device for further processing. For more details about this filter, refer to the documentation regarding :ref:`Explicit Data Movement with FFmpeg <ffmpeg-explicit-data-movement>`.



.. _ffmpeg-tutorial-job-slot-reservation:


Using the Job Slot Reservation Tool
===================================

This example demonstrates two features of the |SDK|: 

- How to split a job across two devices
- How to use the job slot reservation tool to reserve the required resources for running multiple instances of a given job


- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/15_ffmpeg_transcode_2dev_4k.sh`

The :file:`15_ffmpeg_transcode_2dev_4k.sh` script takes two arguments: 

#. The full path to a pre-encoded 4K60 YUV420 HEVC file
#. The ID of a job slot separately allocated using the job slot reservation tool and the :file:`15_ffmpeg_transcode_2dev_4k.json` file associated with this example


The FFmpeg command uses two devices to transcode the input stream to 4K H264 and 1080p HEVC. The first device is used to decode the 4K60 input, scale it to 1080p60 and encode the 4K H264 output. The second device is used to encode the 1080p60 HEVC output. The :option:`-lxlnx_hwdev` option is used to specify the device on which a specific job component (decoder, scaler, encoder) should be run.

Instead of being hardcoded to specific device IDs, the values for the :option:`-lxlnx_hwdev` options are taken from variables set by the :file:`/var/tmp/xilinx/xrm_jobReservation.sh` script, which itself is generated by the :ref:`job slot reservation tool <using-job-slot-reservations>` based on the accompanying :ref:`JSON job description <job-descriptions-files>`. 

The :file:`15_ffmpeg_transcode_2dev_4k.json` JSON job description file specifies the video resources needed by the job, allowing the job slot reservation tool to reserve the resources needed to run as many instances as possible of the specified job on your system. The number of total possible jobs depends on the number of cards in the system and the load of each device. For instance, on a server with a single card, only one instance of this specific example can be run in parallel. On a 2 card server, up to 3 instances of this job can be run in parallel. And on a 8 card server, up to 12 jobs can be run. The job slot reservation tool will reserve the corresponding resources and assign specific reservation IDs in the :file:`/var/tmp/xilinx/xrm_jobReservation.sh` script.


**Tutorial steps**

- Prepare at least one 4K60 YUV420 HEVC input video

- Confirm that there are a least two devices available in your system::

    xbutil examine

- Run the job slot reservation tool::

    jobSlotReservation ./15_ffmpeg_transcode_2dev_4k.json

  The tool will print out the maximum number of jobs which can be run in parallel and will generate the reservation IDs in the :file:`/var/tmp/xilinx/xrm_jobReservation.sh` script. In that file, for is a given job slot {n}, :envvar:`XRM_RESERVE_ID_{n}` indicates the reservation ID generated by XRM while :envvar:`var_dev_{n}_0` and :envvar:`var_dev_{n}_1` indicate the identifiers of the two devices which should be used. For more details, consult the :ref:`job slot reservation tool documentation <using-job-slot-reservations>`.

  The resources will stay reserved until the job slot reservation tool is ended.

- Open a new terminal, and launch the job on the first reserved job slot::

    ./15_ffmpeg_transcode_2dev_4k.sh <4Kp60 HEVC clip> 1

  The script automatically sources the :file:`/var/tmp/xilinx/xrm_jobReservation.sh` script and uses the :envvar:`XRM_RESERVE_ID_{n}`, :envvar:`var_dev_{n}_0` and :envvar:`var_dev_{n}_1` reservation variables corresponding to the specified slot. 

- If your system has enough devices to run more than one job, open a new terminal and launch the job on the second reserved job slot::

    ./15_ffmpeg_transcode_2dev_4k.sh <4Kp60 HEVC clip> 2

- After the first job finishes, the corresponding resources can be used to run another instance of the job. In the same terminal where the first job was run, launch another instance using the first job slot::

    ./15_ffmpeg_transcode_2dev_4k.sh <4Kp60 HEVC clip> 1

- Press **Enter** in the job reservation app terminal to release the resources after all the jobs are complete.

NOTE: The :file:`15_ffmpeg_transcode_2dev_4k_run_all.sh` script can also be used to run all the above steps automatically.

|


.. _faster-than-realtime-example:

*********************
Faster than Real-Time
*********************

Xilinx devices and the |SDK| are optimized for low latency "real-time" applications. That is to say, they provide deterministic low latency transcoding, while operating at the FPS the human eye would normally process/watch it. This is ideal for ingesting a live video stream where there is minimal buffering.

When processing file-based video clips, it is possible to run faster than real time (FTRT) by using a map-reduce approach. With this method, the file-based video clip is split into multiple smaller segments, and each of these segments is individually transcoded. The more devices are available, the more segments can be processed in parallel and the faster the process is. While there is some overhead in "splitting" the clip into segments, and "stitching" the results of each segment into a single output file, these costs are almost always outweighed by the improvement in FPS.

The ``13_ffmpeg_transcode_only_split_stitch.py`` script starts by automatically detecting the number of devices available in the system and then determines how many jobs can be run on each device based on the resolution of the input file. The input file is then split in as many segments aligning on GOP boundaries. Parallel FFmpeg jobs are submited to transcode all the segments simultaneously. The :option:`-xlnx_hwdev` option is used to dispatch each job on a specific device. Once all the segments have been processed, FFmpeg is used to concatenate the results and form the final output stream.

This - - Example script  is provided for demonstration purposes. It is not intended to work for all input clips and all use cases. 

- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/13_ffmpeg_transcode_only_split_stitch.py`

**Command Line**::

    python3 13_ffmpeg_transcode_only_split_stitch.py -s <INPUT_FILE> -d <OUTPUT_FILE> -c <OUTPUT_CODEC> -b <BITRATE>

Explanation of the flags:

- ``-s <INPUT_FILE>``

  + This is the name of the pre-encoded input file (not RAW) in either H.264 or HEVC format.

- ``-d <OUTPUT_FILE>``

  + This is the name of the output file. The default output file name is "out.mp4".

- ``-c <OUTPUT_CODEC>``

  + This defines the desired output encoder format: supported formats are ``h264``, ``hevc``, and ``h265``. Note that ``h265`` and ``hevc`` are identical; they are provided for ease of customer use. The default output codec is ``hevc``.

- ``-b <BITRATE>``

  + This is a float or integer value which defines the output file's target bitrate in Mbits/s. Valid values are comprised between 1.0 and 25.0. The default value is 5.0. Example: use -b 3 to specify an output bitrate of 3Mbits/s.
      

In addition to the primary flags listed above, the script also supports the following optional flags:

- ``-j <NUM_JOBS>``

  + Number of transcode jobs per device. By default the script estimates how many jobs can be run simultaneously on each device. Using this option allows to overwrite to number computed by the script.

- ``-n <NUM_DEVICES>``

  + Number of devices on which to transcode the segments. By default the script will use all available devices. Using this options allows running the script on a subset of the available devices. For example, use ``-n 12`` to run on 12 out of 16 available devices in a vt1.24xlarge instance.

- ``-x <ENCODE_OPTIONS>``

  + Additional options for the encoder, specified as a string. For example, use ``-x "-bf 1"`` to set the number of B frames to 1 in the output video. Bitrate values set with this options take precedence over values set with -b.


|

******************
Streaming Examples
******************

Streaming Examples operate largely on the same principles (and command line strings) as file-based operations. However, the main difference is how streams are received and transmitted.


These examples is will leverage example #6, which is a full transcode pipeline (decode, scale, encode), however, instead of saving the scaled outputs into monolithic MP4 files, will create a "manifest" file ``.m3u8`` for streaming along with several ``.ts`` files with the actual playback data. These manifest files, when inspected, will contain a "playlist" of clips with ``.ts`` extensions, which are of duration ``hls_time``. Creating separate clips enables the remote playback players to "drop quality" instantaneously without any buffering to the viewer, or trying to figure out and seek to "where we are in the clip". This is how most live streaming is done, however there are other, similar protocols (e.g. DASH) which operate on similar principles.

These flags, and others, are defined further on the `FFmpeg main help page <https://ffmpeg.org/ffmpeg-formats.html>`_

Replay Saved Files with Downscaling
===================================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/12_ffmpeg_streaming_transcode_from_file.sh`

**Usage**::

    ./12_ffmpeg_streaming_transcode_from_file.sh <1080p60 h264 clip>

The flows is for representative use.

The command included in the script doesn't handle the audio channel of the input video. For an example of how to include audio in the output streams, refer to the example commented out at the bottom of the script and to the section of the documentation about :ref:`Mapping Audio Streams <mapping-audio-streams>`.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i  $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p60.m3u8 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p30.m3u8 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_480p30.m3u8 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_360p30.m3u8 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_288p30.m3u8


Explanation of the flags:

- ``ffmpeg -c:v mpsoc_vcu_h264 -i $1``
  
  + This calls the Xilinx FFmpeg, decodes using the Xilinx hardware decoder, an input file ``$1``

- ``-filter_complex``

  + This takes the 1080p60 input, converts it to 5x video streams of 720p60, 720p30, 480p30, 360p30, and 160p30 and creates the corresponding audio streams

- ``-b:v <SIZE>``

  + The flag signifies the desired output bitrate for each mapped stream

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-accelerated encoder in the Xilinx device

- ``-f hls``

  + Sets the output video container to an HLS manifest file ``.m3u8`` and the actual clip data ``.ts`` files.

- ``-hls_time 4``
  
  + This sets the duration of all the HLS clips to 4 seconds

- ``-hls_list_size 5``
  
  + This sets the list of accessible/available clips to 5. Can be used to prebuffer the player at the remote end.

- ``-hls flags delete_segments``

  + This flag will delete all segments after the ``hls_list_size`` is reached, saving disk space.

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/var/www/html/xil_xcode_stream_scale<resolution><fps>.m3u8``

  + Saves the output clips, split into size of ``hls_time`` into ``.ts`` clips, indexed by the ``.m3u8`` manifest file.

Live HLS Streaming
==================
- Example script : :url_to_examples:`examples/u30/ffmpeg/tutorials/16_ffmpeg_live_hls.sh`

**Usage**::

    ./16_ffmpeg_live_hls.sh


This script begins by starting a simple web server to serve HLS segments that will be located under ${HLS_DIR}. It then proceeds to generate live HLS using test video and audio signals, for duration specified by variable ${DUR}.

**Command Line**::

    ffmpeg  -f lavfi -i "testsrc=duration=${DUR}:size=1920x1080:rate=30" \
    -f lavfi -i "sine=frequency=5000:duration=${DUR}" \
    -f lavfi -i "sine=frequency=4000:duration=${DUR}" \
    -f lavfi -i "sine=frequency=3000:duration=${DUR}" \
    -f lavfi -i "sine=frequency=2000:duration=${DUR}" \
    -f lavfi -i "sine=frequency=1000:duration=${DUR}" \
    -filter_complex "multiscale_xma=outputs=5: \
     out_1_width=1920: out_1_height=1080: out_1_rate=full: \
     out_2_width=1280: out_2_height=720:  out_2_rate=full: \
     out_3_width=848:  out_3_height=480:  out_3_rate=full: \
     out_4_width=640:  out_4_height=360:  out_4_rate=full: \
     out_5_width=288:  out_5_height=160:  out_5_rate=full  \
     [vid1][vid2][vid3][vid4][vid5]; [1]volume=1[aud1]; [2]volume=1[aud2]; [3]volume=1[aud3]; [4]volume=1[aud4]; [5]volume=1[aud5]" \
    -map "[vid1]" -b:v:0 2M   -minrate:v:0 2M   -maxrate:v:0 2M   -bufsize:v:0 4M   -c:v:0 mpsoc_vcu_h264 \
    -map "[vid2]" -b:v:1 1M   -minrate:v:1 1M   -maxrate:v:1 1M   -bufsize:v:1 1M   -c:v:1 mpsoc_vcu_h264 \
    -map "[vid3]" -b:v:2 750K -minrate:v:2 750K -maxrate:v:2 750K -bufsize:v:2 750K -c:v:2 mpsoc_vcu_h264 \
    -map "[vid4]" -b:v:3 375K -minrate:v:2 375K -maxrate:v:2 375K -bufsize:v:3 375K -c:v:3 mpsoc_vcu_h264 \
    -map "[vid5]" -b:v:4 250k -minrate:v:4 250k -maxrate:v:4 250k -bufsize:v:4 250k -c:v:4 mpsoc_vcu_h264 \
    -map "[aud1]" -c:a:0 aac \
    -map "[aud2]" -c:a:1 aac \
    -map "[aud3]" -c:a:2 aac \
    -map "[aud4]" -c:a:3 aac \
    -map "[aud5]" -c:a:4 aac \
    -var_stream_map "v:0,a:0 v:1,a:1 v:2,a:2 v:3,a:3 v:4,a:4" \
    -f hls \
    -hls_wrap 5 \
    -hls_time 6 \
    -master_pl_name "test.m3u8" -hls_segment_filename  "${HLS_DIR}/test_%v-%d.ts" "${HLS_DIR}/test_%v.m3u8"


Explanation of the flags:

- ``-f lavfi -i testsrc=duration=${DUR}:size=1920x1080:rate=30``

  + This filter generates a 1080p30 test card with a running timer, for duration of ``${DUR}`` seconds

- ``-f lavfi -i "sine=frequency=XXXX:duration=${DUR}"``

  + This filter generates a single tone of frequency ``XXXX``, for duration of ``${DUR}`` seconds

- ``[X]volume=1[audX]``

  + This filter maps audio stream ``X`` to stream aud ``X`` with unity gain

- ``-b:v:X YM   -minrate:v:X YM   -maxrate:v:X YM   -bufsize:v:X ZM``

  + The above combination requests a CBR stream of ``Y`` Mbps for stream index ``X``, using buffer size ``Z``

- ``-map "[audY]" -c:a:X aac``

  + The above encodes raw audio stream aud ``Y`` to aac with stream index ``X``

- ``-var_stream_map "v:0,a:0 v:1,a:1 v:2,a:2 v:3,a:3 v:4,a:4"``

  + This directive groups pair of audio and video streams into a single container

- ``-hls_wrap 5``

  + This specifies the number of segments within the moving window.

- ``-master_pl_name``

  + This sets the name of the master playlist file

- ``-hls_segment_filename  "${HLS_DIR}/test_%v-%d.ts"``

  + Sets the name of the moving-window TS segments

- ``${HLS_DIR}/test_%v.m3u8"``

  + Assigns the name of each variant m3u8 file

To play back the generated HLS, simply point your player or browser to ``http://SERVER_IP:8080/test.m3u8``. If you browser is attempting to download the manifest file instead of playing it, ensure that you have a proper plugin installed, e.g., Native HLS. If you are not able to access port 8080, from outside, you may tunnel and forward this port to your client machine using:
``ssh -AfNL 8080:localhost:8080 USER_NAME@SERVER_IP``
Once the tunnel is established, you may access the manifest file through ``http://localhost:8080/test.m3u8``

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
