#####################################################
FFmpeg Tutorials for the Xilinx Video SDK
#####################################################

This page provides tutorials on how to use FFmpeg with the Xilinx Alveo U30. Detailed documentation for this specific topic can be found in the :doc:`Xilinx Video SDK User Guide </using_ffmpeg>`.


.. contents:: Table of Contents
    :local:
    :depth: 3


The tutorials break down the commands, starting with simple steps using a single device. These are built upon to show 4K, faster than realtime, and multiple operations on the same device.

************
System Setup
************

Ensure you have properly set up the environment via ``source /opt/xilinx/xcdr/setup.sh`` as described in the :doc:`setup instructions </setup>`.

Xilinx provides a precompiled binary for ffmpeg, which is moved to the top of the system PATH.

Some of the examples read or write RAW files from disk (encode-only or decode-only pipelines). There is a chance that due to the massive bandwidth required for operating on these RAW files, you will notice a drop in FPS; this is not due to the U30 but the disk speeds. We recommend reading/writing from ``/dev/shm`` which is a RAM disk.

|

**********************
Simple FFmpeg Examples
**********************

.. _decode-only:

Decode Only
===========
:download:`01_ffmpeg_decode_only.sh </../examples/ffmpeg/tutorials/01_ffmpeg_decode_only.sh>`

**Usage**::

    ./01_ffmpeg_decode_only.sh <1080p60 H.264 file>

This example accepts a clip that is already encoded in H.264, and will decode the file into a RAW format and save it to disk.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i <INPUT> \
    -vf xvbm_convert -pix_fmt yuv420p -y /tmp/xil_dec_out.yuv

To break down the flags:

- ``ffmpeg``

  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-vf xvbm_convert``

  + Internally, the decoder operates on Xilinx-typed buffers to improve performance and enable scalable options for future accelerated filters. To convert back to a host-buffer, you must execute this filter.

- ``-pix_fmt yuv420p``

  + We need to define the colorspace in the output

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_dec_out.yuv``

  + The decoder will save the file to the path above



Encode Only
===========
:download:`02_ffmpeg_encode_only_1080p.sh </../examples/ffmpeg/tutorials/02_ffmpeg_encode_only_1080p.sh>`

**Usage**::

    ./02_ffmpeg_encode_only_1080p.sh <1080p60 YUV file>

This example accepts a RAW 1080p60 clip in YUV420 format. It will pass the clip to the encoder to produce an H.264 encoded MP4 output with a target bitrate of 8Mbps and save it to disk.

**Command Line**::

    ffmpeg -f rawvideo -s 1920x1080 -r 60 -pix_fmt yuv420p -i <INPUT> \
    -b:v 8M -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_enc_out.mp4

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-f rawvideo``
  
  + This signifies that the video is in a raw format, without container or other metadata/information about the clip

- ``-s 1920x1080``

  + Since there is no container or metadata in a RAW clip, the user must define the input clip's resolution/size. This example states the input is 1080p

- ``-r 60``

  + Again, without metadata, the encoder requires the framerate of the incoming stream

- ``-pix_fmt yuv420p``

  + The colorspace of the encoder is by default yuv420p. this example is defining the input clip as being this same colorspace 

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-f mp4``

  + Sets the output video container to MP4

- ``-b:v 8M``

  + The target bitrate of the encoded stream. 8M signifies a target bitrate of 8 Megabits per second. You can also use 8000K or 8000000.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_enc_out.mp4``

  + Save the output in the path above
    
Basic Transcode
===============
:download:`03_ffmpeg_transcode_only.sh </../examples/ffmpeg/tutorials/03_ffmpeg_transcode_only.sh>`

**Usage**::

    ./03_ffmpeg_transcode_only.sh <1080p60 H.264 file>

This example takes an H.264 clip and reencodes it to H.264 with a new bitrate of 8Mbps. The output is writen into :file:`/tmp/xil_xcode.mp4`. 

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i <INPUT> \
    -f mp4 -b:v 8M -c:v mpsoc_vcu_h264 -y /tmp/xil_xcode.mp4 

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-b:v 8M``

  + The target bitrate of the encoded stream. 8M signifies a target bitrate of 8 Megabits per second. You can also use 8000K or 8000000.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_xcode.mp4``

  + This is the output path; most scripts will route here. Change to any output path at your discretion.


.. _decode-and-scale-only:


Decode Only Into Multiple-Resolution Outputs
============================================
:download:`04_ffmpeg_decode_plus_scale.sh </../examples/ffmpeg/tutorials/04_ffmpeg_decode_plus_scale.sh>`

**Usage**::

    ./04_ffmpeg_decode_plus_scale.sh <1080p60 h264 clip>
    
This example decodes an existing H.264 file and then scales it into multiple resolutions as defined below. It will not re-encode them and save the RAW output to disk under ``/tmp/xil_dec_scale<res>.yuv``

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

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

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
:download:`05_ffmpeg_encode_plus_scale_1080p.sh </../examples/ffmpeg/tutorials/05_ffmpeg_encode_plus_scale_1080p.sh>`

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

To break down the flags:

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

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

- ``-f mp4``

  + Sets the output video container to MP4

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_scale_enc_<resolution><fps>.mp4``

  + Saves the output clips to the location listed

Transcode With Multiple-Resolution Outputs
==========================================
:download:`06_ffmpeg_transcode_plus_scale.sh </../examples/ffmpeg/tutorials/06_ffmpeg_transcode_plus_scale.sh>`

**Usage**::
    
    ./06_ffmpeg_transcode_plus_scale.sh <1080p60 h264 clip>   


This example implements a complete transcoding pipeline on an 1080p60 H.264 input. It decodes the input stream, scales it down to different resolutions and frame rates, encodes each of the scaled streams to H.264 and saves them to disk under :file:`xil_xcode_scale_<resolution>.mp4`

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab];[ab]fps=30[abb]" \
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_720p60.mp4 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_720p30.mp4 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_480p30.mp4 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_360p30.mp4 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_xcode_scale_288p30.mp4

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

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

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

- ``-f mp4``

  + Sets the output video container to MP4

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_scale_enc_<resolution><fps>.mp4``

  + Saves the output clips to the location listed

Lower-Latency Transcode With Multiple-Resolution Outputs
========================================================
:download:`07_ffmpeg_transcode_plus_scale_lowlatency.sh </../examples/ffmpeg/tutorials/07_ffmpeg_transcode_plus_scale_lowlatency.sh>`

**Usage**::

    ./ffmpeg_transcode_plus_scale_low_latency.sh <1080p60 h264 clip>

This example is the same as #6, which is a full transcode pipeline (decode, scale, encode), saving the scaled outputs into the files :file:`/tmp/xil_ll_xcode_scale_<reso>.mp4`. This differs in that is is a "low latency" version, which removes the B-frames, and reduces the lookahead. This, in short, decreases the latency at the cost of visual quality.

This example will output corrupt data if you provide an input file that contains B-Frames.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -entropy_buffers_count 2 -low_latency 1 -i $1 \
    -filter_complex "multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720: out_1_rate=full:   \
    out_2_width=848:  out_2_height=480: out_2_rate=half:   \ 
    out_3_width=640:  out_3_height=360: out_3_rate=half:   \
    out_4_width=288:  out_4_height=160: out_4_rate=half    \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
    -map "[aa]"  -b:v 4M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_720p60.mp4 \
    -map "[abb]" -b:v 3M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_720p30.mp4 \
    -map "[b]"   -b:v 2500K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_480p30.mp4 \
    -map "[c]"   -b:v 1250K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_360p30.mp4 \
    -map "[d]"   -b:v 625K  -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_288p30.mp4

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

- ``-low_latency 1``
  
  + This flag disables the Decoder's ability to handle out-of-order frames (i.e. B-Frames). Decoding I and P frames only decreases the latency of the system.
 
  + **If your stream contains B-Frames, you will receive a corrupt output**

  + Remove ``-low_latency 1`` from the commandline if your input has B-Frames

- ``-filter_complex``

  + The FFmpeg ``-filter_complex`` flag allows combining multiple filters together using a graph-like syntax. This example uses the :option:`multiscale_xma`, ``split`` and ``fps`` filters to create 5 output resolutions from the input stream.
  + The :option:`multiscale_xma` filter configures the Xilinx hardware-accelerated scaler to produce 4 output resolutions (1280x720p60, 848x480p30, 640x360p30, and 288x160p30). For each output, the width, height and frame rate are defined with ``out_<n>_width``, ``out_<n>_height`` and  ``out_<n>_rate``. The 4 outputs of the :option:`multiscale_xma` filter are identified as ``a``, ``b``, ``c`` and ``d`` respectively. 
  + The ``split`` and ``fps`` software filters are used to split the ``a`` stream into ``aa`` and ``ab`` and then drop the framerate of ``ab`` to 30 fps to produce the ``abb`` 1280x720p30 stream.

- ``-map "[ID]"``

  + Selects an output of the filter graph. The flags that follow apply to the selected stream.

- ``-b:v <SIZE>``

  + The flag signifies the desired output bitrate for each mapped stream

- ``-bf 0``
  
  + The number of b-frames inserted in the output stream not only increases encode latency in the Alveo U30 card, but decode latency on the player. Setting it to 0 removes them.

- ``scaling-list 0``

  + Disables the scaling list, which is a pre-encode processing which normally adds to the latency of the pipeline.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

- ``-f mp4``

  + Sets the output video container to MP4

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_ll_xcode_scale_<resolution><fps>.mp4``

  + Saves the output clips to the location listed

|

*****************************
Encoding Streams to 4K
*****************************

The U30 Video SDK solution supports real-time decoding and encoding of 4k streams with the following notes:

- The U30 video pipeline is optimized for live-streaming use cases. For 4k streams with bitrates significantly higher than the ones typically used for live streaming, it may not be possible to sustain real-time performance.
- When decoding 4k streams with a high bitrate, increasing the number of entropy buffers using the :option:`-entropy_buffers_count` option can help improve performance
- When encoding raw video to 4k, set the :option:`-s` option to ``3840x2160`` to specify the desired resolution.
- When encoding 4k streams to H.264, the :option:`-slices` option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.
- The lookahead feature is not supported for 4k. FFmpeg will give an error if :option:`-lookahead_depth` is enabled when encoding to 4k.


4k H.264 Real-Time Encode Only
==============================
:download:`08_ffmpeg_encode_only_4k.sh </../examples/ffmpeg/tutorials/08_ffmpeg_encode_only_4k.sh>`

**Usage**::
    
    ./08_ffmpeg_encode_only_4k.sh <2160p60 YUV file>

This example takes an 8-bit, YUV420, 2160p60 RAW file, encodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_4k_enc_out.mp4`. The :option:`-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::

    ffmpeg -f rawvideo -s 3840x2160 -r 60 -pix_fmt yuv420p -i <INPUT> \
    -b:v 20M -c:v mpsoc_vcu_h264 -slices 4 -f mp4 -y /tmp/xil_4k_enc_out.mp4

4k H.264 Real-Time Transcode
============================
:download:`09_ffmpeg_transcode_only_4k.sh </../examples/ffmpeg/tutorials/09_ffmpeg_transcode_only_4k.sh>`

**Usage**::
    
    ./09_ffmpeg_transcode_only_4k.sh <2160p60 HEVC file>

This example takes an 2160p60 HEVC file, transcodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_4k_enc_out.mp4`. The :option:`-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_hevc -i <INPUT> \
    -b:v 20M -c:v mpsoc_vcu_h264 -slices 4 -f mp4 -y /tmp/xil_4k_xcode.mp4 

|

.. _multiple-ffmpeg-jobs-example:

********************************
Running Multiple FFmpeg Jobs
********************************

So far we've run one job at a time, even if the job does not use all the resources available on the device. The Video SDK makes it possible to run multiple FFmpeg jobs in parallel on a device or across multiple devices. 

This script transcodes three H264 streams to HEVC, sending the outputs to /tmp/xil_xcode_{n}.mp4. The three transcodes are run in parallel in individual xterms. The FFmpeg :option:`-xlnx_hwdev` is used to control on which device each job is run. The first job is run on device #0 and the two others jobs are run on device #1. After the jobs are launched, a JSON system load report is generated.

.. note::
   This example leverages the ``xterm`` program. Make sure it is installed on your system before proceeding.


:download:`10_ffmpeg_multiple_jobs.sh </../examples/ffmpeg/tutorials/10_ffmpeg_multiple_jobs.sh>`

**Usage**::

    ./10_ffmpeg_multiple_jobs.sh <input_h264_1_mp4> <input_h264_2_mp4> <input_h264_3_mp4>

**Commands**::

  # Launch the three jobs in parallel
  xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 0 -c:v mpsoc_vcu_h264 -i $1 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_1.mp4" &
  xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $2 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_2.mp4" &
  xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $3 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_3.mp4" &

  # Wait until the jobs are started to generate a system load report
  sleep 2s
  xrmadm /opt/xilinx/xrm/test/list_cmd.json &


**Tutorial steps**

- Prepare 3 input H264 videos with the following resolutions: 4k60, 1080p60 and 720p30

- Confirm there are a least two devices available in your system::

    xbutil examine

- Run the example script with the 3 input videos::

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

|

.. _faster-than-realtime-example:

************************
Faster than Realtime
************************

The Alveo U30 card is optimized for low latency "realtime" applications. That is to say, it provides deterministic low latency transcoding, while operating at the FPS the human eye would normally process/watch it. This is ideal for ingesting a live video stream where there is minimal buffering.

Faster Than Real Time (FTRT) is almost the contrary: you have the entire file/clip saved and can therefore "divide and conquer". There are two main flags to consider when processing in this flow: :option:`-cores` and :option:`-slices`.

FTRT on a single Alveo U30 device
=================================
:download:`11_ffmpeg_ftrt_transcode_only.sh </../examples/ffmpeg/tutorials/11_ffmpeg_ftrt_transcode_only.sh>`

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i <INPUT> \
    -f mp4 -b:v 8M -c:v mpsoc_vcu_h264 -cores 4 -slices 4 -y /tmp/xil_ftrt_xcode.mp4 

To break down the flags:

- ``ffmpeg``
  
  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``-c:v mpsoc_vcu_h264``
  
  + Declares the decoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated decoder in the Alveo U30 card

- ``-i <INPUT>``

  + The input file to be transcoded

- ``-b:v``

  + The target bitrate for video will follow

- ``8M``

  + This signifies 8-Megabits per second target bitrate. You can also use 10000K or 10000000.

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card
 
- ``-cores 4``

  + Each device on the Alveo U30 internally uses 4x engines/subcores of 1080p60; when targeting 4k resolutions, it will automatically detect and utilize more subcores to maintain realtime performance. 
  + Forcing the device to use more cores than necessary will enable more FPS
  + Using more cores on a clip/stream that cannot provide more FPS will waste resources and have no effect

- ``-slices 4``

  + A video bitstream is broken down into segments called "slices". 
  + Enabling the system to operate on multiple slices at a time has the benefit of improving FPS, but will adversely affect Visual Quality, as motion estimation within a segment of the frame is being operated on separately, will not be able to communicate with each other, so they may have visual artifacts. 
  + Please see our Visual Quality page to run an analysis to determine if the FPS gain is worth a reduction in VQ.

- ``-y``

  + Enable overwrite without prompting the user if they're sure

- ``/tmp/xil_ftrt_xcode.mp4``

  + This is the output path; most scripts will route here. Change to any output path at your discretion.


FTRT across multiple Alveo U30 devices for maximum FPS
======================================================

When processing file-based solutions, where you have the entire clip to operate on in a single command, you have the option to split the video into segments, and distribute the segments to individual encoder instances (for Alveo U30, these would be devices on the card). While there is some overhead in "splitting" the clip to begin with, and "stitching" the output files into a single output file, these costs are almost always outweighed by the improvement in FPS.

The function below will split the clip on a boundary that ensures no visual quality is lost (a closed GOP boundary), and will distribute the clip across all available Alveo U30 cards in the system. If you inspect the script, you may edit it to apply the :option:`-cores` and :option:`-slices` flags as listed above for an even larger increase in FPS. Remember that :option:`-slices` will adversely affect VQ.

The load-balancing of this example uses the :option:`-xlnx_hwdev` flag, which manually utilizes available slots as they are available, systematically filling the cards from "index 0" to "index N" 

:download:`13_ffmpeg_transcode_only_split_stitch.py </../examples/ffmpeg/tutorials/13_ffmpeg_transcode_only_split_stitch.py>`

**Command Line**::

    python 13_ffmpeg_transcode_only_split_stitch.py \
    -s <INPUT> -d /tmp/xil_split_stitch.mp4 -u <SPLIT_COUNT> -i <INPUT_CODEC> -o <OUTPUT_CODEC> -b <BITRATE>

To break down the flags:

- ``python``

  + This calls the local system python to execute. This has been tested on Python3 only.

- ``-s <INPUT>``

  + This is the pre-encoded input file (not RAW) in either H.264 or h.265 (HEVC) format

- ``-d /tmp/xil_split_stitch.mp4``

  + This is the output path; most scripts will route here. Change to any output path at your discretion

- ``-u <SPLIT_COUNT>``

  + ``<SPLIT_COUNT>`` is an integer number from ``{1..MAXCARDS*2}``. This is the number of Alveo U30 devices you wish to deploy your workload on to. Since each Alveo U30 card has two devices, the maximum number will be the number of cards in your system times two.

- ``-i <INPUT_CODEC>``

  + This defines the input file's pre-encoded format: supported formats are ``h264``, ``hevc``, and ``h265``. Note that ``h265`` and ``hevc`` are identical; they are provided for ease of customer use.

- ``-o <OUTPUT_CODEC>``

  + This defines the desired output encoder format: supported formats are ``h264``, ``hevc``, and ``h265``. Note that ``h265`` and ``hevc`` are identical; they are provided for ease of customer use.

- ``-b <BITRATE>``

  + This is a float or integer value which defines the output file's target bitrate in Mbits/s. Valid values are comprised between 1.0 and 25.0. The default value is 5.0. Example: use -b 3 to specify an output bitrate of 3Mbits/s.
      
|

******************
Streaming Examples
******************

Streaming Examples operate largely on the same principles (and commandline strings) as file-based operations. However, the main difference is how streams are received and transmitted.


These examples is will leverage example #6, which is a full transcode pipeline (decode, scale, encode), however, instead of saving the scaled outputs into monolithic MP4 files, will create a "manifest" file ``.m3u8`` for streaming along with several ``.ts`` files with the actual playback data. These manifest files, when inspected, will contain a "playlist" of clips with ``.ts`` extentions, which are of duration ``hls_time``. Creating separate clips enables the remote playback players to "drop quality" instanatneously without any buffering to the viewer, or trying to figure out and seek to "where we are in the clip". This is how most live streaming is done, however there are other, similar protocolos (e.g. DASH) which operate on similar principles.

These flags, and others, are defined further on the `FFmpeg main help page <https://ffmpeg.org/ffmpeg-formats.html>`_

Replay Saved Files with Downscaling
===================================
:download:`12_ffmpeg_streaming_transcode_from_file.sh </../examples/ffmpeg/tutorials/12_ffmpeg_streaming_transcode_from_file.sh>`

**Usage**::

    ./12_ffmpeg_streaming_transcode_from_file.sh <1080p60 h264 clip>


The flows is for representative use

**Command Line**::

    ffmpeg -c:v mpsoc_vcu_h264 -i  $1 \
    -filter_complex "multiscale_xma=outputs=4: \  
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]"\
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p60.m3u8 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p30.m3u8 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_480p30.m3u8 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_360p30.m3u8 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_288p30.m3u8

To break down the flags:

- ``ffmpeg -c:v mpsoc_vcu_h264 -i $1``
  
  + This calls the Xilinx FFmpeg, decodes using the Alveo U30 hardware decoder, an input file ``$1``


- The filter graph::
  
    -filter_complex "multiscale_xma=outputs=4: \  
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]"\
    -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p60.m3u8 \
    -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p30.m3u8 \
    -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_480p30.m3u8 \
    -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_360p30.m3u8 \
    -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_288p30.m3u8

  + This takes the 1080p60 input and converts it to 5x streams of 720p60, 720p30, 480p30, 360p30, and 160p30

- ``-b:v <SIZE>``

  + The flag signifies the desired output bitrate for each mapped stream

- ``-c:v mpsoc_vcu_h264``

  + Declares the encoder's codec for video (as opposed to audio ``-c:a ...``) is the hardware-acclerated encoder in the Alveo U30 card

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



..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
