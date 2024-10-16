.. _gstreamer-tutorials:

#####################################################
GStreamer Introductory Tutorials
#####################################################

.. highlight:: none

This page provides tutorials on how to use GStreamer with the |SDK|. The complete reference guide for the GStreamer version included in the |SDK| can be found :doc:`here </using_gstreamer>`.


.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::


*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst

The setup script exports important environment variables, starts the Xilinx Resource Manager (XRM) daemon, and ensures that the Xilinx devices and the XRM plugins are properly loaded. It also sets up the plugins needed for GStreamer solution of the |SDK| and makes the pre-built GStreamer abrscaler and compositor applications available in the $PATH.

Sourcing of the setup scripts should be performed each time you open a new terminal on your system. This is required for the environment to be correctly configured.  If the runtime environment is not correctly set up, running the examples below will result in errors like “xbutil: command not found” or "plugins not loaded" errors 

|

**********************************************************
Simple GStreamer Examples
**********************************************************

By default, all the example scripts use the filesink plug-in and write the output files into the /tmp directory. 

Some of the examples read or write RAW files from disk (encode-only or decode-only pipelines). There is a chance that due to the massive bandwidth required for operating on these RAW files, you will notice a drop in FPS; this is not due to the |SDK| but the disk speeds. We recommend reading/writing from ``/dev/shm`` which is a RAM disk.

Most of the scripts also have provision to use the fakesink plug-in which only displays performance numbers and will not write outputs to disk. This is done by setting the “fakesink” argument to 1. 

Each script contains error checks before passing arguments to GStreamer pipeline command to help users avoid giving incorrect arguments.

Most of the example scripts included in this directory take H.264 input streams. To use H.265 input streams, update the scripts to use the ``h265parse`` GStreamer plug-in instead of ``h264parse``. 

For brevity purposes, explanations of the GStreamer element properties are not repeated after they have been explained once. The detailed explanation of the each GStreamer pipeline element property can be obtained by using ``gst-inspect-1.0 <element name>`` (e.g. ``gst-inspect-1.0 vvas_xvcudec``).

.. _gstreamer-decode-only:

Decode only
===========

- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/01_gst_decode_only.sh`


**Usage**::

  ./01_gst_decode_only.sh <device index> <1080p60 H.264 file 8-bit/10-bit> <Number of decoder instances, 1 to 8> <Number of buffers> <fakesink 0/1>
  
  <device index> : On which device the pipeline should run
  <1080p60 H264 file> : Input file location of 8-bit/10-bit
  <num instances, 1 to 8> : Number of instances of the same pipeline to be run
  <Number of buffers> : Num of Buffers to be processed. Useful to specify if we want to process small portion of bigger input stream. ``-1`` will run the complete video stream 
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example accepts a clip that is already encoded in H.264 and will decode it using vvas_xvcudec plugin into a raw NV12 format. The raw file is saved to disk at /tmp/xil_dec_out_*.nv12 or /tmp/xil_dec_out_*.nv12_10le32 based on 8-bit or 10-bit input encoded stream. To get more understanding of the vvas_xvcudec plug-in properties, refer to the :ref:`Decoder Plug-in <gst-decoder-plugin>` section of the GStreamer reference guide.

**Command Line**::

    e.g. ./01_gst_decode_only.sh 0 ~/videos/Test_1080p60.h264 1 -1 0

    gst-launch-1.0 -v filesrc num-buffers=-1 location=~/videos/Test_1080p60.h264 \
    ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! fpsdisplaysink name=fpsdisplaysink_0 video-sink="filesink location=/tmp/xil_dec_out_1920x1080_8_0.nv12" text-overlay=false sync=false

Explanation of the pipeline elements and their properties:

- ``gst-launch-1.0``

  + The GStreamer application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``filesrc location``

  + Location of the file to read

- ``filesrc num-buffers``

  + Number of 4K sized buffers to be read from the input stream. Giving "-1" to this argument will read complete stream
  
- ``h264parse``

  + Parses H.264 streams

- ``vvas_xvcudec``

  + GStreamer plug-in that provides functionality to decode H.264/H.265 encoded streams using Xilinx VCU decoder for PCIe platforms. This plug-in accepts input encoded stream in byte-stream/NALU format only and produces NV12 frames.

  + ``vvas_xvcudec dev-idx=<device id>`` : Device on which the VCU decoder to be run.

- ``fpsdisplaysink``
 
  + Can display the current and average frame rate as a testoverlay or on stdout.

  + ``fpsdisplaysink video-sink=fakesink`` : Video sink to use. It can be `fakesink`. It is a dummy sink that swallows everything and used to showcase maximum performance capability of the pipeline.
 
  + ``fpsdisplaysink video-sink="filesink location=/tmp/xil_dec_out_1920x1080_8_0.nv12"`` : Video sink to use. It can be `filesink`. It writes incoming data to a file in the local file system.

  + ``fpsdisplaysink text-overlay=<true/false>`` : Whether to use text-overlay. Enabling this will display rendered frames data. For performance mode, this has to be set to false.

  + ``fpsdisplaysink sync=<true/false>`` : Sync on the clock. For performance mode, this has to be set to false. Enabling this may lower the performance or drop the frames to match with the input video stream fps value

.. _gstreamer-encode-only:


Encode only
===========
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/02_gst_h264_encode_only_1080p.sh`

**Usage**::

  ./02_gst_h264_encode_only_1080p.sh <device index> <Input 1080p60 NV12 file> <Number of encoder instances, 1 to 4> <10bit-input 0/1> <fakesink 0/1>

  <device index> : On which device the pipeline should run
  <1080p60 H264 file> : Input file location
  <num instances, 1 to 8> : Number of instances of the same pipeline to be run
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example accepts a RAW 1080p60 clip in nv12/nv12-10le32 format. It will pass the clip to the encoder using vvas_xvcuenc plug-in to produce an H.264 encoded MP4 output with a target bitrate of 8Mbps and save it to disk at /tmp/xil_enc_out_*.mp4. 

To get more understanding of the vvas_xvcudec plug-in properties, refer to the :ref:`Encoder Plug-in <gst-encoder-plugin>` section of the GStreamer reference guide.

**Command Line**::

    e.g. ./02_gst_h264_encode_only_1080p.sh 0 ~/videos/Test_1080p60.nv12 1 0 0

    gst-launch-1.0 filesrc location=~/videos/Test_1080p60.nv12 blocksize=3110400
    ! queue \
    ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 \
    ! vvas_xvcuenc dev-idx=0 target-bitrate=8000 max-bitrate=8000 enable-pipeline=true \
    ! h264parse \
    ! qtmux \
    ! fpsdisplaysink video-sink="filesink location=/tmp/xil_enc_out_0.mp4 " text-overlay=false sync=false -v

Explanation of the pipeline elements and their properties:

- ``gst-launch-1.0``

  + The GStreamer application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``filesrc location``

  + Location of the file to read

- ``blocksize``

  + By default, ``filesrc`` reads data in blocks of 4096 bytes. The ``blocksize`` option is used to override this default and specify how many bytes of data should be read in one go. Reading data from file in small increments may impact performance. When reading raw data, it is recommended to set the ``blocksize`` to the frame size (in bytes) so that a full frame is read each time.   

- ``queue``

  + The queue will create a new thread on the source pad to decouple the processing on sink and source pad. This element needed between any two processing elements of a GStreamer pipeline, e.g. between vvas_xabrscaler and vvas_xvcuenc
  
- ``rawvideoparse``

  + This element parses incoming data as raw video frames and timestamps these.

  + ``rawvideoparse format`` : Format of frames in raw stream. Supported values are 8-bit (nv12) or 10-bit (nv12-10e32)

  + ``rawvideoparse width`` : Width of frames in raw stream

  + ``rawvideoparse height`` : Height of frames in raw stream

  + ``rawvideoparse framerate`` : Rate of frames in raw stream

- ``h264parse``

  + Placed at the output of the encoder, this elements tells the encoder to output an H.264 stream
  + Used along the ``video/x-h264`` caps filter, this can be used to specify the encoding profile type (baseline/high/high-10), e.g. ``h264parse ! video/x-h264, profile=high``
  + Specifying other profile type leads to unexpected behavior or incorrect output stream

- ``qtmux``

  + This element muxes streams into QuickTime(qt) files. This is needed whenever we are writing the output file as .mp4 container format instead of elementary H.264/H.265 format. Please note that qtmux causes printing of lot of hex characters on console about header info. User can ignore them


- ``vvas_xvcuenc``

  + GStreamer plug-in that provides functionality to encode the raw frames (nv12/nv12-10le32) using Xilinx VCU encoder for PCIe platforms.

  + ``vvas_xvcuenc dev-idx=<device id>`` : Device on which to run the VCU encoder.
  
  + ``vvas_xvcuenc target-bitrate=8000`` : Target bit rate of the encoded stream in Kbps. 8000 signifies a target bit rate of 8 Megabits per second. This Value should be <= max-bitrate
  
  + ``vvas_xvcuenc max-bitrate=8000`` : Max bit rate in Kbps, only used if control-rate=variable


.. _gstreamer-basic-transcode:


Basic Transcode
===============
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/03_gst_h264_transcode_only.sh`

**Usage**::

  ./03_gst_h264_transcode_only.sh <device index> <Input H.264 file> <Number of transcode instances, 1 to 8> <Number of buffers> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input H264 file> : Input file location
  <num instances, 1 to 8> : Number of instances of the same pipeline
  <Number of buffers> : Num of Buffers to be processed. Useful to specify if we want to process small portion of bigger input stream. ``-1`` will run the complete video stream
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example demonstrates how to achieve simple transcoding, i.e. It takes an H.264 clip and re encodes it to H.264 with a new bit rate of 8Mbps. The output is written into :file:`/tmp/xil_xcode_*.mp4`. Input H.264 stream, from file source is decoded using hardware decoder and then re-encoded using vcu hardware encoder to H.264 format with same or different bit rate.


**Command Line**::

    e.g. ./03_gst_h264_transcode_only.sh  0 ~/videos/Test_1080p60.h264 1 -1 0

    gst-launch-1.0 -v filesrc num-buffers=-1 location=~/videos/Test_1080p60.h264 \
    ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! queue \
    ! vvas_xvcuenc dev-idx=0 target-bitrate=8000 max-bitrate=8000 \
    ! h264parse \
    ! qtmux \
    ! fpsdisplaysink name=fpsdisplaysink_0 video-sink="filesink location=/tmp/xil_xcode_out_0.mp4" text-overlay=false sync=false

Explanation of the pipeline elements and their properties:

Refer to the :ref:`Encode Only <gstreamer-encode-only>` and :ref:`Decode Only <gstreamer-decode-only>` examples descriptions for an illustration of the elements used in this pipeline.

.. _gstreamer-decode-and-scale:

Decode only into Multiple-Resolution outputs
============================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/04_gst_decode_plus_scale.sh`

**Usage**::

  ./04_gst_decode_plus_scale.sh <device index> <Input 1080p60 H264 file> <num instances, 1 to 4> <Number of buffers> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input 1080p60 H264 file> : Input file location
  <num instances, 1 to 4> : Number of instances of the same pipeline
  <Number of buffers> : Number of buffers to be processed, Useful to specify if we want to use small portion of bigger stream. ``-1`` will run the complete video stream
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example decodes an existing 8-bit/10-bit H.264 file and then scales it into multiple resolutions as defined below. It will not re-encode them, but save the RAW outputs to disk under /tmp/xil_dec_scal*.nv12 or /tmp/xil_dec_scal*.nv12_10le32.

The 1080p60 input is scaled down to the following resolutions and frame rates (respectively): 720p60, 720p30, 480p30, 360p30, 288p30. ``vvas_xabrscaler`` generates multiple resolution outputs. ``tee`` is used along with ``videorate`` to generate outputs with same resolution but with different frame rate.

**Command Line**::

  e.g. ./04_gst_decode_plus_scale.sh  0 ~/videos/Test_1080p60.h264 1 2000 0

  gst-launch-1.0 filesrc num-buffers=2000 location=/home/siva/videos/Test_1080p60.h264 ! h264parse \
  ! vvas_xvcudec dev-idx=0 \
  ! queue \
  ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 name=sc_00 \
  sc_00.src_0 \
    ! queue ! video/x-raw, width=1280, height=720 \
    ! tee name=tee_00 \
    tee_00. \
      ! queue \
      ! videorate ! video/x-raw, framerate=60/1 \
      ! fpsdisplaysink name=sink_dec_scale_720p60_dev0_0 video-sink="filesink location=/\tmp/\xil_dec_scale_720p60_dev_8_0_0.nv12" text-overlay=false sync=false \
    tee_00. \
      ! queue \
      ! videorate \
      ! video/x-raw, framerate=30/1 \
      ! fpsdisplaysink name=sink_dec_scale_720p30_dev0_0 video-sink="filesink location=/\tmp/\xil_dec_scale_720p30_dev_8_0_0.nv12" text-overlay=false sync=false \
  sc_00.src_1 \
      ! queue \
      ! video/x-raw, width=848, height=480 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! fpsdisplaysink name=sink_dec_scale_480p30_dev0_0 video-sink="filesink location=/\tmp/\xil_dec_scale_480p30_dev_8_0_0.nv12" text-overlay=false sync=false \
  sc_00.src_2 \
     ! queue \
     ! video/x-raw, width=640, height=360 \
     ! videorate ! video/x-raw, framerate=30/1 \
     ! fpsdisplaysink name=sink_dec_scale_360p30_dev0_0 video-sink="filesink location=/\tmp/\xil_dec_scale_360p30_dev_8_0_0.nv12" text-overlay=false sync=false \
  sc_00.src_3 \
     ! queue \
     ! video/x-raw, width=288, height=160 \
     ! videorate ! video/x-raw, framerate=30/1 \
     ! fpsdisplaysink name=sink_dec_scale_160p30_dev0_0 video-sink="filesink location=/\tmp/\xil_dec_scale_160p30_dev_8_0_0.nv12" text-overlay=false sync=false -v


Explanation of the pipeline elements and their properties:

- ``vvas_xabrscaler``

  + GStreamer plug-in developed to accelerate the resize and color space conversion functionality using Xilinx Multiscaler hardware kernel. Based on the `video/x-raw` caps of down stream element properties, this plug-in decides the color conversion format of the output stream. Currently the hardware kernel is supporting NV12 and NV12_10LE32 color formats and hence no other color format can be used. For resizing, any resolution with in 3840x2160 is supported provided that: width x height <= 3840x2160, max width <= 3840 and max height <= 3840. 

  + ``vvas_xabrscaler dev-idx=<device id>`` : Device on which the resize and color space conversion to be run.

  + ``vvas_xabrscaler ppc=4`` : Pixel per clock configuration for the multiscaler kernel. The only valid value is 4.
  
  + ``vvas_xabrscaler scale-mode=2`` : Scale Mode configuration for the multiscaler kernel. The only valid value is 2 (polyphase mode).
  
  + ``vvas_xabrscaler name=<sc_00>`` : The name of the object. Scaler supports single input stream to multiple output streams after resize or color conversion. There can be multiple down stream elements using these multiple outputs. Hence we need an object name of the scaler element that can be referenced by down stream elements. The `.src_0`, `.src_1`,.., are the source pad elements of the scaler object. Hence, each output of the scaler is referred as ``<name>.src_0``, ``<name>.src_1``, so on. In above example they are ``sc_00.src_0``, ``sc_00.src_1``, ``sc_00.src_2``, so on. If there are more up stream elements, we can create multiple objects of the scaler (sc_00, sc_01, sc_02, ..) with each object has the multiple source pads.  

- ``tee``

  + Split data to multiple pads. Branching the data flow is useful when e.g. capturing a video where the video is shown on the screen and also encoded and written to a file. One needs to use separate queue elements (or a multiqueue) in each branch to provide separate threads for each branch. Otherwise a blocked dataflow in one branch would stall the other branches. In above example `tee` along with `videorate` is used to generate 720p60 and 720p30 outputs from 720p60 input. 

- ``videorate``

  + This element takes an incoming stream of timestamped video frames. It will produce a perfect stream that matches the source pad's framerate.

- ``video/x-raw, width=<>, height=<>``

  + This is the source or sink pad capabilities by which the upstream or downstream element negotiates and works accordingly. Based on the capabilities of the GStreamer elements, if the pad capabilities are not matching, the pipeline fails with negotiation errors.


.. _gstreamer-encode-and-scale:

Encode only into Multiple-Resolution outputs
============================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/05_gst_encode_plus_scale_1080p.sh`

**Usage**::

  ./05_gst_encode_plus_scale_1080p.sh <device index> <1080p60 nv12 RAW file> <num instances, 1 to 4> <10bitinput 0/1> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input 1080p60 nv12 file> : Input file location
  <num instances, 1 to 4> : Number of instances of the same pipeline
  <10bitinput 0/1> : Is input raw file is 10-bit or 8-bit. Set 1 for 10-bit and set 0 for 8-bit
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example will take a 8-bit or 10-bit 1080p60 RAW NV12 file and scale it and encode it into the resolutions as defined below and save them to disk under /tmp/xil_scale_enc*.mp4.

**Command Line**::
   
    e.g. ./05_gst_encode_plus_scale_1080p.sh  0 ~/videos/Test_1080p60.nv12 1 0 0

    gst-launch-1.0 filesrc location=~/videos/Test_1080p60.nv12 blocksize=3110400 \
    ! queue ! rawvideoparse format=nv12 width=1920 height=1080 framerate=60/1 \
    ! queue \
    ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 name=sc_00 avoid-output-copy=true enable-pipeline=true \
    sc_00.src_0 ! queue ! video/x-raw, width=1280, height=720 \
      ! queue ! tee name=tee_00 \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=60/1 \
        ! vvas_xvcuenc name=enc_720p60_dev0_0 dev-idx=0 target-bitrate=4000 max-bitrate=4000 \
        ! h264parse ! qtmux \
        ! fpsdisplaysink name=sink_scale_enc_720p60_dev0_0 video-sink="filesink location=/\tmp/\xil_scale_enc_720p60_dev__0_0.mp4" text-overlay=false sync=false \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=30/1 \
        ! vvas_xvcuenc name=enc_720p30_dev0_0 dev-idx=0 target-bitrate=3000 max-bitrate=3000 \
        ! h264parse ! qtmux \
        ! fpsdisplaysink name=sink_scale_enc_720p30_dev0_0 video-sink="filesink location=/\tmp/\xil_scale_enc_720p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_1 ! queue ! video/x-raw, width=848, height=480 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_480p30_dev0_0 dev-idx=0 target-bitrate=2500 max-bitrate=2500 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_scale_enc_480p30_dev0_0 video-sink="filesink location=/\tmp/\xil_scale_enc_480p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_2 ! queue ! video/x-raw, width=640, height=360 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_360p30_dev0_0 dev-idx=0 target-bitrate=1250 max-bitrate=1250 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_scale_enc_360p30_dev0_0 video-sink="filesink location=/\tmp/\xil_scale_enc_360p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_3 ! queue ! video/x-raw, width=288, height=160 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_160p30_dev0_0 dev-idx=0 target-bitrate=625 max-bitrate=625 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_scale_enc_160p30_dev0_0 video-sink="filesink location=/\tmp/\xil_scale_enc_160p30_dev__0_0.mp4" text-overlay=false sync=false -v

Explanation of the pipeline elements and their properties:

Refer to the :ref:`Decode only into Multiple-Resolution outputs <gstreamer-decode-and-scale>` example description for an illustration of the elements used in this pipeline.

.. _gstreamer-transcode-and-scale:

Transcode with Multiple-Resolution outputs
==========================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/06_gst_transcode_plus_scale.sh`

**Usage**::

  ./06_gst_transcode_plus_scale.sh <device index> <Input 1080p60 H264 File> <num instances, 1 to 4> <Number of buffers> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input 1080p60 H264 file> : Input file location
  <num instances, 1 to 4> : Number of instances of the same pipeline
  <Number of buffers> : Number of buffers to be processed instead of complete video stream for quick test. Set this to -1 to process complete input stream
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example implements a complete transcoding pipeline on an 1080p60 H.264 input. It decodes the input stream, scales it down to different resolutions and frame rates, encodes each of the scaled streams to H.264 and saves them to disk under :file:`/tmp/xil_xcode_scale_<resolution>.mp4`.

The 1080p60 input is scaled down and encoded back to the following resolutions and framerates (respectively):
720p60, 720p30, 480p30, 360p30, 288p30.

The command included in the script doesn't handle the audio channel of the input video. For an example of how to include audio in the output streams, refer to the example commented out at the bottom of the script and to the section of the documentation about :ref:`Mapping Audio Streams <gst-mapping-audio-streams>`.


**Command Line**::
   
    e.g. ./06_gst_transcode_plus_scale.sh 0 bbb_sunflower_1080p_60fps_normal.mp4 1 2000 1


    gst-launch-1.0 filesrc num-buffers=-1 location=~/videos/bbb_sunflower_1080p_60fps_normal.mp4 \
    ! qtdemux \
    ! queue ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! queue \
    ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 avoid-output-copy=true name=sc_00 \
    sc_00.src_0 ! queue ! video/x-raw, width=1280, height=720 \
      ! tee name=tee_00 \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=60/1 \
        ! vvas_xvcuenc name=enc_720p60_dev0_0 dev-idx=0 target-bitrate=4000 \
        ! h264parse ! qtmux \
        ! fpsdisplaysink name=sink_xcode_scale_720p60_dev0_0 video-sink="filesink location=/\tmp/\xil_xcode_scale_720p60_dev__0_0.mp4" text-overlay=false sync=false \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=30/1 \
        ! vvas_xvcuenc name=enc_720p30_dev0_0 dev-idx=0 target-bitrate=3000 \
        ! h264parse ! qtmux \
        ! fpsdisplaysink name=sink_xcode_scale_720p30_dev0_0 video-sink="filesink location=/\tmp/\xil_xcode_scale_720p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_1 ! queue ! video/x-raw, width=848, height=480 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_480p30_dev0_0 dev-idx=0 target-bitrate=2500 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_xcode_scale_480p30_dev0_0 video-sink="filesink location=/\tmp/\xil_xcode_scale_480p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_2 ! queue ! video/x-raw, width=640, height=360 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_360p30_dev0_0 dev-idx=0 target-bitrate=1250 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_xcode_scale_360p30_dev0_0 video-sink="filesink location=/\tmp/\xil_xcode_scale_360p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_3 ! queue ! video/x-raw, width=288, height=160 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_160p30_dev0_0 dev-idx=0 target-bitrate=625 \
      ! h264parse ! qtmux \
      ! fpsdisplaysink name=sink_xcode_scale_160p30_dev0_0 video-sink="filesink location=/\tmp/\xil_xcode_scale_160p30_dev__0_0.mp4" text-overlay=false sync=false -v

Explanation of the pipeline elements and their properties:


- ``qtdemux``

  + This element demuxes a QuickTime (qt) file into raw or compressed audio and/or video streams. This is needed whenever we are decoding an input stream of container format type with H.264/H.265 elementary stream in it

- ``vvas_xabrscaler avoid-output-copy=true``

  + Avoid output frames copy on all source pads of the scaler even when downstream does not support GstVideoMeta metadata. when ``tee`` element is used after scaler, 

Refer to the :ref:`Decode only into Multiple-Resolution outputs <gstreamer-decode-and-scale>` example description for an illustration of the other elements used in this pipeline.

.. _gstreamer-transcode-and-scale-low-latency:

Lower-Latency Transcode With Multiple-Resolution outputs
========================================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/07_gst_transcode_plus_scale_lowlatency.sh`

**Usage**::

  ./07_gst_transcode_plus_scale_lowlatency.sh <device index> <Input 1080p60 H264 File> <num instances, 1 to 4> <Number of buffers> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input 1080p60 H264 file> : Input file location of mp4 container with H.264 stream or elementaty H.264 stream
  <num instances, 1 to 4> : Number of instances of the same pipeline
  <Number of buffers> : Number of buffers to be processed instead of complete video stream for quick test. Set this to -1 to process complete input stream
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example is the same as #6, which is a full transcode pipeline (decode, scale, encode), saving the scaled outputs into the files :file:`/tmp/xil_ll_xcode_scale_<reso>.mp4`. This differs in that is a “low latency” version, which removes the B-frames, and reduces the lookahead. Thus it decreases the latency at the cost of visual quality. This example will output corrupt data if you provide an input file that contains B-Frames.

This example will output corrupt data if you provide an input file that contains B-Frames.

The command included in the script doesn't handle the audio channel of the input video. For an example of how to include audio in the output streams, refer to the example commented out at the bottom of the script and to the section of the documentation about :ref:`Mapping Audio Streams <gst-mapping-audio-streams>`.

**Command Line**::
   
    e.g. ./07_gst_transcode_plus_scale_lowlatency.sh 0 bbb_sunflower_1080p_60fps_normal.mp4 1 2000 1

    gst-launch-1.0 filesrc num-buffers=2000 location=/home/siva/videos/bbb_sunflower_1080p_60fps_normal.mp4 \
    ! qtdemux \
    ! queue \
    ! h264parse \
    ! vvas_xvcudec dev-idx=0 low-latency=1 \
    ! queue \
    ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 avoid-output-copy=true name=sc_00 \
    sc_00.src_0 ! queue ! video/x-raw, width=1280, height=720 \
      ! tee name=tee_00 \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=60/1 \
        ! vvas_xvcuenc name=enc_720p60_dev0_0 dev-idx=0 target-bitrate=4000 b-frames=0 scaling-list=0 \
        ! h264parse \
        ! qtmux \
        ! fpsdisplaysink name=sink_ll_xcode_scale_720p60_dev0_0 video-sink="filesink location=/\tmp/\xil_ll_xcode_scale_720p60_dev__0_0.mp4" text-overlay=false sync=false \
      tee_00. \
        ! queue ! videorate ! video/x-raw, framerate=30/1 \
        ! vvas_xvcuenc name=enc_720p30_dev0_0 dev-idx=0 target-bitrate=3000 b-frames=0 scaling-list=0 \
        ! h264parse \
        ! qtmux \
        ! fpsdisplaysink name=sink_ll_xcode_scale_720p30_dev0_0 video-sink="filesink location=/\tmp/\xil_ll_xcode_scale_720p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_1 ! queue ! video/x-raw, width=848, height=480 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_480p30_dev0_0 dev-idx=0 target-bitrate=2500 b-frames=0 scaling-list=0 \
      ! h264parse \
      ! qtmux \
      ! fpsdisplaysink name=sink_ll_xcode_scale_480p30_dev0_0 video-sink="filesink location=/\tmp/\xil_ll_xcode_scale_480p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_2 ! queue ! video/x-raw, width=640, height=360 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_360p30_dev0_0 dev-idx=0 target-bitrate=1250 b-frames=0 scaling-list=0 \
      ! h264parse \
      ! qtmux \
      ! fpsdisplaysink name=sink_ll_xcode_scale_360p30_dev0_0 video-sink="filesink location=/\tmp/\xil_ll_xcode_scale_360p30_dev__0_0.mp4" text-overlay=false sync=false \
    sc_00.src_3 ! queue ! video/x-raw, width=288, height=160 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_160p30_dev0_0 dev-idx=0 target-bitrate=625 b-frames=0 scaling-list=0 \
      ! h264parse \
      ! qtmux \
      ! fpsdisplaysink name=sink_ll_xcode_scale_160p30_dev0_0 video-sink="filesink location=/\tmp/\xil_ll_xcode_scale_160p30_dev__0_0.mp4" text-overlay=false sync=false -v
  
Explanation of the pipeline elements and their properties:

- ``vvas_xvcudec low-latency=1``

  + This flag disables the Decoder's ability to handle out-of-order frames (i.e. B-Frames). Decoding I and P frames only decreases the latency of the system.

- ``vvas_xabrscaler avoid-output-copy=true``

  + Avoid output frames copy on all source pads even when downstream does not support GstVideoMeta metadata

- ``vvas_xvcuenc b-frames=0``

  + Number of B-frames between two consecutive P-frames. The number of b-frames inserted in the output stream not only increases encode latency in the Xilinx device, but decode latency on the player. Setting it to 0 removes them.

- ``vvas_xvcuenc scaling-list=0``

  + Scaling list mode. Value `0` indiactes Flat scaling list mode. Disables the scaling list, which is a pre-encode processing which normally adds to the latency of the pipeline.

Refer to the :ref:`Decode only into Multiple-Resolution outputs <gstreamer-decode-and-scale>` example description for an illustration of the elements used in this pipeline.

*****************************
Encoding Streams to 4K
*****************************

The |SDK| supports real-time decoding and encoding of 4k streams with the following notes:

- The Xilinx video pipeline is optimized for live-streaming use cases. For 4k streams with bitrates significantly higher than the ones typically used for live streaming, it may not be possible to sustain real-time performance.
- When decoding 4k streams with a high bitrate, increasing the number of entropy buffers using the :option:`-entropy_buffers_count` option can help improve performance
- When encoding raw video to 4k, set the `width and height` parameters to ``3840 and 2160`` to specify the desired resolution.
- When encoding 4k streams to H.264, the :option:`num-slices` option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.


4K H.264 Real-Time Encode Only
==============================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/08_gst_encode_only_4k.sh`

**Usage**::

  ./08_gst_encode_only_4k.sh <Device Index> <raw 4K nv12 file> <Number of encode instances, 1> <10bitinput 0/1> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <raw 4K nv12 file> : Input file location of raw 4K 8-bit/10-bit file
  <num instances, 1> : Number of instances are fixed to 1 as each device can support a max of 4K60
  <10bitinput 0/1> : Input raw file is of 10-bit (value 1) or 8-bit type (value 0). For encoder scripts, the user must set this option.
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk


This example takes an 8-bit, 2160p60 RAW file of 8-bit (NV12) or 10-bit (NV12_10LE32), encodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_4k_enc_out.mp4`. The :option:`num-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::
   
    e.g. ./08_gst_encode_only_4k.sh 0 ~/videos/test4K.nv12 1 0 0

    gst-launch-1.0 filesrc location=test4K.nv12 blocksize=12441600 \
    ! queue \
    ! rawvideoparse format=nv12 width=3840 height=2160 framerate=60/1 \
    ! vvas_xvcuenc dev-idx=0 target-bitrate=20000 max-bitrate=20000 num-slices=4 enable-pipeline=true \
    ! h264parse \
    ! qtmux \
    ! fpsdisplaysink video-sink="filesink location=/tmp/xil_4k_enc_out_0.mp4 " text-overlay=false sync=false -v

Explanation of the pipeline elements and their properties:

- ``vvas_xvcudec num-slices=4``

  + Number of slices produced for each frame. This option is required to sustain real-time performance when encoding a 4k stream to H.264.


4K H.264 Real-Time Transcode
==========================================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/09_gst_transcode_only_4k.sh`

**Usage**::

  ./09_gst_transcode_only_4k.sh <Device Index> <Input 4K H265/HEVC file> <Number of transcode instances, 1>  <Number of buffers> <fakesink 0/1>
  
  <device index> :  On which device the pipeline should run
  <Input 4K H.265/HEVC fil> : Input file location of H.265 encoded stream
  <num instances, 1> : Number of instances are fixed to 1 as each device can support a max of 4K60
  <fakesink 0/1> : Whether to write the output to fakesink (for performance) or to write the output stream to a location on the disk

This example takes an 2160p60 HEVC file, transcodes it to H.264 at a rate of 20Mbps and writes the result into :file:`/tmp/xil_xcode_*.mp4`. The :option:`num-slices` option is required to sustain real-time performance when encoding a 4k stream to H.264.

**Command Line**::

  ./09_gst_transcode_only_4k.sh 0 ~/videos/Test_4k_60fps.h265 1 -1 0
  
  gst-launch-1.0 filesrc location=~/videos/Test_4k_60fps.h265 \
  ! h265parse \
  ! vvas_xvcudec num-entropy-buf=3 dev-idx=0 \
  ! vvas_xvcuenc dev-idx=0 b-frames=2 target-bitrate=20000 max-bitrate=20000 prefetch-buffer=true num-slices=4 gop-mode=low-delay-p control-rate=2 \
  ! h264parse \
  ! qtmux \
  ! fpsdisplaysink video-sink="filesink location=/tmp/xil_xcode_4k_0.mp4 " text-overlay=false sync=false -v

Explanation of the pipeline elements and their properties:


- ``vvas_xvcuenc prefetch-buffer=true``

  + Enable/Disable L2Cache buffer in encoding process.

- ``vvas_xvcuenc gop-mode=low-delay-p``

  + Group Of Pictures mode. Setting it to `low-delay-p` gop-mode is set to Single I-frame followed by P-frames only

- ``vvas_xvcuenc control-rate=2``

  + Bitrate control method. It is set to ``Constant`` mode.

- ``h265parse``

  + Placed at the output of the encoder, this elements tells the encoder to output an H.265 stream
  + Used along the ``video/x-h265`` caps filter, this can be used to specify the encoding profile type (main/main-10), e.g. ``h265parse ! video/x-h265, profile=main``
  + Specifying other profile type leads to unexpected behavior or incorrect output stream

  |


.. _gstreamer-device-id-examples:

********************************
Running on Multiple Devices
********************************

So far we've run one job at a time, even if the job does not use all the resources available on the device. The Video SDK makes it possible to run multiple GStreamer jobs in parallel on a device or across multiple devices. 

This script transcodes three H.264 streams to HEVC, sending the outputs to /tmp/xil_xcode_{n}.mp4. The three transcodes are run in parallel in individual xterms. The GStreamer :option:`dev-idx` option is used to control on which device each job is run. The first job is run on device #0 and the two others jobs are run on device #1. After the jobs are launched, a JSON system load report is generated. Ensure that "xbutil list" shows at least 2 devices before running this program.

.. note::
   This example leverages the ``xterm`` program. Make sure it is installed on your system before proceeding.

- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/10_gst_multiple_jobs.sh`

**Usage**::

  ./10_gst_multiple_jobs.sh <input_h264_1_mp4> <input_h264_2_mp4> <input_h264_3_mp4>

**commands**::

  # Launch the three jobs in parallel
  xterm -fa mono:size=9 -hold -e "gst-launch-1.0 filesrc location=$1 ! qtdemux ! queue ! h264parse ! vvas_xvcudec  dev-idx=0 ! vvas_xvcuenc dev-idx=0 target-bitrate=2000 ! h265parse ! video/x-h265 ! qtmux ! fpsdisplaysink video-sink="filesink location=/tmp/xil_xcode_1.mp4" text-overlay=false sync=false -v"
  xterm -fa mono:size=9 -hold -e "gst-launch-1.0 filesrc location=$2 ! qtdemux ! queue ! h264parse ! vvas_xvcudec  dev-idx=0 ! vvas_xvcuenc dev-idx=0 target-bitrate=2000 ! h265parse ! video/x-h265 ! qtmux ! fpsdisplaysink video-sink="filesink location=/tmp/xil_xcode_2.mp4" text-overlay=false sync=false -v"
  xterm -fa mono:size=9 -hold -e "gst-launch-1.0 filesrc location=$3 ! qtdemux ! queue ! h264parse ! vvas_xvcudec  dev-idx=0 ! vvas_xvcuenc dev-idx=0 target-bitrate=2000 ! h265parse ! video/x-h265 ! qtmux ! fpsdisplaysink video-sink="filesink location=/tmp/xil_xcode_3.mp4" text-overlay=false sync=false -v"

  # Wait until the jobs are started to generate a system load report
  sleep 2s
  xrmadm /opt/xilinx/xrm/test/list_cmd.json &

**Tutorial steps**

- Prepare 3 input H.264 videos with the following resolutions: 4k60, 1080p60 and 720p30

- Confirm there are a least two devices available in your system::

    xbutil examine

- Run the example script with the 3 input videos::

    ./10_gst_multiple_jobs.sh 4k60.mp4 1080p60.mp4 720p30.mp4

- The script opens three xterm windows and runs a transcode job in each of them. After 2 seconds, to ensure all jobs are running, the script executes the ``xrmadm /opt/xilinx/xrm/test/list_cmd.json`` command to generate a report of the system load.

- In each of the xterm windows, inspect the GStreamer transcript and observe that it indicates on which device the job is run::

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

    ./10_gst_multiple_jobs.sh 720p30.mp4 4k60.mp4 1080p60.mp4

  This will try to simultaneously run the 4k60 and the 1080p60 jobs on device #1. The compute requirements of these two combined jobs will exceed the capacity of a single device. Only one of the two jobs will proceed and the second one will error out due to insufficient resources.

Running Multiple GStreamer Jobs using the vvas_xabrladder application
=====================================================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/14_gst_app_transcode_plus_scale.sh`

**Usage**::

  ./14_gst_app_transcode_plus_scale.sh <device index> <Input 1080p60 MP4 file with H.264 content>

This script calls four processes of vvas_xabrladder application simultaneously. vvas_xabrladder is a command line utility that implements the GStreamer video transcoding pipeline. This application expects an input video file (mp4 with H.264/H.265 or H.264/H.265 elementary stream) and produces 5 different H.264/H.265 elementary streams based on codec type provided. The output files are stored at /tmp/ladder_outputs directory. More documentation on vvas_xabrladder can be found at :ref:`GStreamer ABR Ladder Application <gst_abrladder>`

**commands**::

  ./14_gst_app_transcode_plus_scale.sh 0 bbb_sunflower_1080p_60fps_normal.mp4

.. _gstreamer-lookahead-and-scale:

Transcode with lookahead for Multiple-Resolution outputs
==========================================================
- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/15_gst_transcode_plus_scale_la.sh`

**Usage**::

  ./15_gst_transcode_plus_scale_la.sh <device index> <Input MP4 file with H.264 content> <num instances, 1 to 4> <Number of buffers> <fakesink 0/1>

This example will do a full transcode pipeline on an 1080p60 H.264 input, scale it into the resolutions below, and re-encode them, saving them in /tmp/sink_xcode_scale_*.h264. The raw output from scaler is padded through lookahead before going to encoder. Lookahead is used to improve the accuracy of rate control by enabling the encoder to buffer a specified number of frames (using the parameter). The 1080p60 input is scaled down and encoded back to the following resolutions and framerates (respectively):
720p60, 720p30, 480p30, 360p30, 288p30.

**commands**::

  e.g. ./15_gst_transcode_plus_scale_la.sh 0 bbb_sunflower_1080p_60fps_normal.mp4 1 2000 0
  
  gst-launch-1.0 filesrc num-buffers=2000 location=~/Videos/bbb_sunflower_1080p_60fps_normal.mp4 ! qtdemux ! queue ! h264parse ! vvas_xvcudec dev-idx=0 ! queue ! vvas_xabrscaler avoid-output-copy=true dev-idx=0 ppc=4 scale-mode=2 name=sc_00 sc_00.src_0 ! queue ! video/x-raw, width=1280, height=720 ! tee name=tee_00 tee_00. ! queue ! videorate ! video/x-raw, framerate=60/1 ! vvas_xlookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 dev-idx=0 ! vvas_xvcuenc name=enc_720p60_dev0_0 dev-idx=0 target-bitrate=4000 rc-mode=1 ! h264parse ! qtmux ! fpsdisplaysink name=sink_xcode_scale_720p60_dev0_0 video-sink="filesink location=/\tmp/\xil_la_xcode_scale_720p60_dev__0_0.mp4 async=false" text-overlay=false sync=false tee_00. ! queue ! videorate ! video/x-raw, framerate=30/1 ! vvas_xlookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 dev-idx=0 ! vvas_xvcuenc name=enc_720p30_dev0_0 dev-idx=0 target-bitrate=3000 rc-mode=1 ! h264parse ! qtmux ! fpsdisplaysink name=sink_xcode_scale_720p30_dev0_0 video-sink="filesink location=/\tmp/\xil_la_xcode_scale_720p30_dev__0_0.mp4 async=false" text-overlay=false sync=false sc_00.src_1 ! queue ! video/x-raw, width=848, height=480 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xlookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 dev-idx=0 ! vvas_xvcuenc name=enc_480p30_dev0_0 dev-idx=0 target-bitrate=2500 rc-mode=1 ! h264parse ! qtmux ! fpsdisplaysink name=sink_xcode_scale_480p30_dev0_0 video-sink="filesink location=/\tmp/\xil_la_xcode_scale_480p30_dev__0_0.mp4 async=false" text-overlay=false sync=false sc_00.src_2 ! queue ! video/x-raw, width=640, height=360 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xlookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 dev-idx=0 ! vvas_xvcuenc name=enc_360p30_dev0_0 dev-idx=0 target-bitrate=1250 rc-mode=1 ! h264parse ! qtmux ! fpsdisplaysink name=sink_xcode_scale_360p30_dev0_0 video-sink="filesink location=/\tmp/\xil_la_xcode_scale_360p30_dev__0_0.mp4 async=false" text-overlay=false sync=false sc_00.src_3 ! queue ! video/x-raw, width=288, height=160 ! videorate ! video/x-raw, framerate=30/1 ! vvas_xlookahead codec-type=0 spatial-aq=1 temporal-aq=1 lookahead-depth=8 dev-idx=0 ! vvas_xvcuenc name=enc_160p30_dev0_0 dev-idx=0 target-bitrate=625 rc-mode=1 ! h264parse ! qtmux ! fpsdisplaysink name=sink_xcode_scale_160p30_dev0_0 video-sink="filesink location=/\tmp/\xil_la_xcode_scale_160p30_dev__0_0.mp4 async=false" text-overlay=false sync=false -v

.. note::

  For any script that is being run for multiple instances, user should ensure that the resolution of input stream*number of instances is not beyond maximum hardware supported resolution i.e. 4K60. Failing this requirement, device may go to unknown state and host needs to be cold rebooted.

.. note::

  In a pipeline which handles 4K streams, set async=false on all sink elements (fakesink, filesink) to avoid pipeline hangs as queue elements gets filled with raw 4K frames quickly. In case user do not want to set async=false, they should be aware of queue elements full condition as per open source documentation

|

.. _faster-than-realtime-gstreamer-example:

************************
Faster than Real-Time
************************

Xilinx devices and the |SDK| are optimized for low latency "real-time" applications. That is to say, they provide deterministic low latency transcoding, while operating at the FPS the human eye would normally process/watch it. This is ideal for ingesting a live video stream where there is minimal buffering.

When processing file-based video clips, it is possible to run faster than real time (FTRT) by using a map-reduce approach. With this method, the file-based video clip is split into multiple smaller segments, and each of these segments is individually transcoded. The more devices are available, the more segments can be processed in parallel and the faster the process is. While there is some overhead in "splitting" the clip into segments, and "stitching" the results of each segment into a single output file, these costs are almost always outweighed by the improvement in FPS.

The ``13_gst_transcode_only_split_stitch.py`` python script starts by automatically detecting the number of devices available in the system and then determines how many jobs can be run on each device based on the resolution of the input file. The input file is then split in as many segments aligning on GOP boundaries. Parallel GStreamer jobs are submited to transcode all the segments simultaneously. The :option:`dev-idx` option is used to dispatch each job on a specific device. Once all the segments have been processed, GStreamer is used to concatenate the results and form the final output stream.

This example script is provided for demonstration purposes. It is not intended to work for all input clips and all use cases.

- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/13_gst_transcode_only_split_stitch.py`

**Usage**::

   python3 13_gst_transcode_only_split_stitch.py -s <INPUT_FILE> -d <OUTPUT_FILE> -c <OUTPUT_CODEC> -b <BITRATE>

**Command Line**::

    e.g. python3 13_gst_transcode_only_split_stitch.py  -s ~/videos/bbb_sunflower_1080p_60fps_normal.mp4 -d ./output.mp4 -c h264 -b 5

Explanation of the flags:

- ``-s <INPUT_FILE>``

  + This is the name of the pre-encoded input file in MP4 format with h264 or h265 elementary stream.

- ``-d <OUTPUT_FILE>``

  + This is the name of the output file. The default output file name is "out.mp4"

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

  + Additional options for the encoder, specified as a string. For example, use ``-x "b-frames=1"`` to set the number of B frames to 1 in the output video. Bitrate values set with this options take precedence over values set with -b.

|

******************
Streaming Examples
******************

Streaming Examples operate largely on the same principles (and command line strings) as file-based operations. However, the main difference is how streams are received and transmitted.


These examples is will leverage example #6, which is a full transcode pipeline (decode, scale, encode), however, instead of saving the scaled outputs into monolithic MP4 files, will create a "manifest" file ``.m3u8`` for streaming along with several ``.ts`` files with the actual playback data. These manifest files, when inspected, will contain a "playlist" of clips with ``.ts`` extensions, which are of duration ``hls_time``. Creating separate clips enables the remote playback players to "drop quality" instantaneously without any buffering to the viewer, or trying to figure out and seek to "where we are in the clip". This is how most live streaming is done, however there are other, similar protocols (e.g. DASH) which operate on similar principles.


Replay Saved Files with Downscaling
===================================

- Example script: :url_to_examples:`examples/u30/gstreamer/tutorials/12_gst_streaming_transcode_from_file.sh`

**Usage**::

    ./12_gst_streaming_transcode_from_file.sh <device index> <Input 1080p60 H.264 file> <[Number of buffers]>

Ensure we have created and given write access to the ``/var/www/html`` directory before running this script.

The command included in the script doesn't handle the audio channel of the input video. For an example of how to include audio in the output streams, refer to the example commented out at the bottom of the script and to the section of the documentation about :ref:`Mapping Audio Streams <gst-mapping-audio-streams>`.

**Command Line**::

    gst-launch-1.0 filesrc num-buffers=2000 location=bbb_sunflower_1080p_60fps_normal.mp4 \
    ! qtdemux ! queue ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! queue \
    ! vvas_xabrscaler dev-idx=0 ppc=4 scale-mode=2 name=sc_00 avoid-output-copy=true \
    sc_00.src_0 ! queue ! video/x-raw, width=1280, height=720, format=NV12 \
      ! tee name=tee_00 \
      tee_00. \
      ! queue ! videorate ! video/x-raw, framerate=60/1 \
      ! vvas_xvcuenc name=enc_720p60_dev0_0 dev-idx=0 target-bitrate=4000 \
      ! h264parse ! video/x-h264 \
      ! hlssink2 target-duration=4 playlist-length=5 max-files=5 location=/var/www/html/segment1_%05d.ts playlist-location=/var/www/html/xil_xcode_stream_scale_720p60.m3u8 \
      tee_00. \
      ! queue ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_720p30_dev0_0 dev-idx=0 target-bitrate=3000 \
      ! h264parse ! video/x-h264 \
      ! hlssink2 target-duration=4 playlist-length=5 max-files=5 location=/var/www/html/segment2_%05d.ts playlist-location=/var/www/html/xil_xcode_stream_scale_720p30.m3u8 \
    sc_00.src_1 ! queue ! video/x-raw, width=848, height=480, format=NV12 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_480p30_dev0_0 dev-idx=0 target-bitrate=2500 \
      ! h264parse ! video/x-h264 \
      ! hlssink2 target-duration=4 playlist-length=5 max-files=5 location=/var/www/html/segment3_%05d.ts playlist-location=/var/www/html/xil_xcode_stream_scale_480p30.m3u8 \
    sc_00.src_2 ! queue ! video/x-raw, width=640, height=360, format=NV12 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_360p30_dev0_0 dev-idx=0 target-bitrate=1250 \
      ! h264parse ! video/x-h264 \
      ! hlssink2 target-duration=4 playlist-length=5 max-files=5 location=/var/www/html/segment4_%05d.ts playlist-location=/var/www/html/xil_xcode_stream_scale_360p30.m3u8 \
    sc_00.src_3 ! queue ! video/x-raw, width=288, height=160, format=NV12 \
      ! videorate ! video/x-raw, framerate=30/1 \
      ! vvas_xvcuenc name=enc_160p30_dev0_0 dev-idx=0 target-bitrate=625 \
      ! h264parse ! video/x-h264 \
      ! hlssink2 target-duration=4 playlist-length=5 max-files=5 location=/var/www/html/segment5_%05d.ts playlist-location=/var/www/html/xil_xcode_stream_scale_160p30.m3u8 -v

Explanation of the pipeline elements and their properties:

- ``hlssink2``

  + HTTP Live Streaming sink/server. Unlike the old hlssink which took a muxed MPEG-TS stream as input, this element takes elementary audio and video streams as input and handles the muxing internally. This element only writes fragments and a playlist file into a specified directory, it does not contain an actual HTTP server to serve these files. Just point an external webserver to the directory with the playlist and fragment files.


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
