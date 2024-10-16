######################
Video Quality Examples
######################

.. highlight:: none

This page is dedicated to explaining some of the details behind Video Quality (VQ), how it is measured, and how you can optimize your FFmpeg commands with the |SDK| to maximize its performance. 

Further documentation on this topic can be found in the :ref:`Tuning Video Quality <tuning-video-quality>` section of the |SDK| user guide.

.. contents:: Table of Contents
    :local:
    :depth: 2
.. .. section-numbering::



*****************
Environment Setup
*****************

.. include:: /include/examples/setup_examples.rst


*****************************
Introduction to Video Quality
*****************************

.. include:: /include/examples/intro_to_vq.rst

********************************
Optimized Settings for the |SDK|
********************************
It is highly recommended to perform encoding on raw video clips; that is, clips that have not undergone a transform/compression/encoding in the past. This ensures that the clips are in a universally known state in order to fairly compare encoders. 

Alternatively, you can add the flags to decode before encoding, and the results will remain accurate as long as the same pre-encoded file is used as the source across all encoders under test. Information on this process can be found on the :doc:`FFmpeg tutorial page </examples/ffmpeg/tutorials>`.

Flags not illustrated in this page are covered in the :doc:`Using FFmpeg </using_ffmpeg>` chapter of the |SDK| user guide.


Subjective Quality
==================

- Example script for H264 streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_subjective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_subjective.sh`

These are the command you should use to get maximum video quality to the human eye in most situations. It accepts a clip that is already decoded or a RAW YUV.

**Usage**::

    ./h264_subjective.sh <1080p60 YUV file> <bitrate in Kbps>
    ./hevc_subjective.sh <1080p60 YUV file> <bitrate in Kbps>


**Command Line**::

    ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT \
    -vsync 0 \
    -b:v ${BITRATE}K \
    -c:v ${CODEC} \
    -f ${FORMAT} \ 
    -bf 1 \
    -g 120 \
    -periodicity-idr 120 \
    -qp-mode relative-load \
    -lookahead_depth 20 \
    -spatial-aq 1 \
    -temporal-aq 1 \
    -y ./${INPUT}_${BITRATE}_${TEST}.${EXTENSION}
    
Explanation of the flags:

- ``ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT``
  
  + This line takes a YUV420 1080p60 file as the input

- ``-vsync 0``
  
  + This line is an `advanced FFmpeg flag <https://ffmpeg.org/ffmpeg.html#Advanced-options>`__
  
  + Setting it to 0 passes through every frame to the decoder as it is delivered. Other settings are used to 'help' the encode process by modifying the content; for example, FFmpeg often duplicates the first frame.
  
  + A ``0`` setting ensures nothing is changed/added/dropped, which is perfect for calculating VQ

- ``-b:v ${BITRATE}K`` 
  
  + This flag takes the 2nd parameter from the script and sets the desired target bitrate of the output video clip.
  
- ``-c:v ${CODEC}``

  + This flag defines the encoder shall be the either ``mpsoc_vcu_h264`` or ``mpsoc_vcu_hevc``
  
- ``-f ${FORMAT}``

  + This flag defines what the output format shall be, for example an ``h264`` or ``hevc`` elementary container (as opposed, to say, mp4)
  
- ``-bf 1``

  + This flag define the number of B-frames which shall be inserted 

- ``-g 120``

  + This sets the I-Frame interval; which means a full picture of data (instead of difference-frames P & B) is sent every 120 frames. 
  
  + Livestreaming protocols prefer (some require) "2 seconds". Since our input to the script expects a 60fps file, its set to 120; a 30fps source would be set to 60.
  
- ``-periodicity-idr 120``

  + This sets the period of an IDR frame to every 120 frames. This means that the "reference buffer" is flushed, or in other words "don't let any future frame reference a frame before me". 
  
  + This greatly helps in allowing decoders/players to seek to different parts of the video
  
  + This is also used in online streaming for ad-insertion.

- ``-qp-mode relative-mode``
  
  + This is a setting wherein data from the lookahead buffer is used to assign the best quantization for each macroblock. Use this mode only when lookahead is enabled.
  
- ``-lookahead_depth 20``

  + The lookahead is the buffer depth of, in this case, 20 frames.
  
  + the LA Buffer is the number of frames the encoder looks at in a single pass to determine how best to compress the data temporarily. 
  
  + Increasing the buffer increases both the latency and quality of your compression

  + When lookahead is enabled, spatial-aq and temporal-aq are enabled by default and qp-mode is set to relative-load automatically.

- ``-spatial-aq 1``

  + Turns on Spatial Adaptive Quantization

- ``-temporal-aq 1``

  + Turns on Temporal Adaptive Quantization

- ``-y ./${INPUT}_${BITRATE}_${TEST}.${EXTENSION}``

  + Overwrite any existing file via ``-y`` and save it to the working directory: e.g. ``./myclip_4000K_objective.264`` or ``./myclip_2000K_subjective.hevc``.


Objective Quality
=================

- Example script for H264 streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_objective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_objective.sh`

**Usage**::

    ./h264_objective.sh <1080p60 YUV file> <bitrate in Kbps>
    ./hevc_objective.sh <1080p60 YUV file> <bitrate in Kbps>


This is the command you should run to get maximum objective scoring (PSNR, SSIM, VMAF). It accepts a clip that is already decoded or a RAW YUV.

**Command Line**::

    ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT \
    -vsync 0 \
    -b:v ${BITRATE}K \ 
    -c:v ${CODEC} \
    -f ${FORMAT} \ 
    -bf 1 \
    -g 120 \
    -periodicity-idr 120 \
    -tune-metrics 1 \
    -lookahead_depth 20 \
    -y ./${INPUT}_${BITRATE}_${TEST}.${EXTENSION}
    
Explanation of the flags:

- ``ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT``
  
  + This line takes a YUV420 1080p60 file as the input

- ``-vsync 0``
  
  + This line is an `advanced FFmpeg flag <https://ffmpeg.org/ffmpeg.html#Advanced-options>`__
  
  + Setting it to 0 passes through every frame to the decoder as it is delivered. Other settings are used to 'help' the encode process by modifying the content; for example, FFmpeg often duplicates the first frame.
  
  + A ``0`` setting ensures nothing is changed/added/dropped, which is perfect for calculating VQ

- ``-b:v ${BITRATE}K`` 
  
  + This flag takes the 2nd parameter from the script and sets the desired target bitrate of the output video clip.
  
- ``-c:v ${CODEC}``

  + This flag defines the encoder shall be the either ``mpsoc_vcu_h264`` or ``mpsoc_vcu_hevc``
  
- ``-f ${FORMAT}``

  + This flag defines what the output format shall be, for example an ``h264`` or ``hevc`` elementary container (as opposed, to say, mp4)
  
- ``-bf 1``

  + This flag defines the number of B-frames which shall be inserted 

- ``-g 120``

  + This sets the I-Frame interval; which means a full picture of data (instead of difference-frames P & B) is sent every 120 frames. 
  
  + Livestreaming protocols prefer (some require) "2 seconds". Since our input to the script expects a 60fps file, its set to 120; a 30fps source would be set to 60.
  
- ``-periodicity-idr 120``

  + This sets the period of an IDR frame to every 120 frames. This means that the "reference buffer" is flushed, or in other words "don't let any future frame reference a frame before me". 
  
  + This greatly helps in allowing decoders/players to seek to different parts of the video
  
  + This is also used in online streaming for ad-insertion.

- ``-tune-metrics 1``

  + Tunes video quality for objective scores by setting qp-mode to uniform and scaling-list to flat. Also, it turns OFF spatial-aq and temporal-aq when lookahead is enabled, to improve objective scores.

- ``-scaling-list 0``
  
  + Sets the scaling list to flat

- ``-lookahead_depth 20``

  + The lookahead is the buffer depth, in this case, 20 frames.
  
  + the LA Buffer is the number of frames the encoder looks at in a single pass to determine how best to compress the data temporarily. 
  
  + Increasing the buffer increases both the latency and quality of your compression

- ``-y ./${INPUT}_${BITRATE}_${TEST}.${EXTENSION}``

  + Overwrite any existing file via ``-y`` and save it to the working directory: e.g. ``./myclip_4000K_objective.264`` or ``./myclip_2000K_subjective.hevc``.


Setting Differences Between Objective and Subjective Quality
============================================================

- ``-qp-mode``

  + How an encoder quantizes its CU's (Macroblocks/Coding Tree Units/etc.) is what fundamentally defines a large amount of its quality.
   
  + ``uniform`` load **(objective)** equally quantizes all CU's within a slice.
  
  + ``relative-load`` **(subjective)** adds another layer of analysis before the encoder and provides "hints" to the encoder; improves video quality but again, mathematically 'breaks' the image, resulting in lower scores. 

- ``-scaling_list``

  + The scaling list is used to scale up low-frequency data in the stream such that when it is quantized down during the encoding process, detail is retained.
  
  + This process specifically modifies parts of the scene and not others, which in most objective metrics is purposely "breaking" it, reducing its score.

- ``spatial-aq`` and ``temporal-aq``

  + Adaptive Quantization (AQ) exploits the fact that the human eye is more sensitive to certain regions of a frame. This method drops information from high-frequency locations and keeps more information in low-frequency locations in a frame. The result appears more visually appealing. To enable spatial or temporal AQ, qp-mode should be set to relative-load and lookahead should be enabled.
  
  + Imagine a scene of a windy forest: the moving leaves (high frequency/texture data), and tree trunks on the ground (low-frequency/texture data). Artifacts and issues in the low-frequency data will catch your eye much more than the high-frequency data. AQ will drop data in the leaves (they are much harder to see changes from frame-to-frame) and make sure the trunks and ground keep more of their data. It is a zero-sum game when compressing data.
  
  + Spatial AQ is redistribution of bits/data within a frame, while temporal AQ is data over time (i.e. over many frames). With Temporal AQ, the same concepts apply: high-motion regions are less noticeable than low-motion regions; Temporal AQ looks ahead in the Lookahead buffer to determine which is which and will redistribute bits/data accordingly for a more visually appealing scene.
  
  + There is another flag which is enabled (but is set to default in these strings, so it is omitted in the command line) with ``spatial-aq``. The flag is ``-spatial-aq-gain`` and can be set 0-100; default is 50. This parameter is the strength of the redistribution of data within the frame. Setting too high a value may have a consequence of blurring edges. Experimentation across your clips is recommended if you wish to tune the parameter. We keep it to 50(%) to cover the widest set of use cases.

*****************************
Running PSNR/SSIM/VMAF scores
*****************************

Due to licensing reasons, the FFmpeg binary delivered in this package does not include a comprehensive set of codecs or plugins required for scoring video quality. You have many options:

#. (Easiest) Download a static FFmpeg build from `John Van Sickle <https://johnvansickle.com/ffmpeg/>`__  which has VMAF (among other plugins) precompiled + installed

#. `Recompile another version FFmpeg <https://trac.ffmpeg.org/wiki/CompilationGuide>`__ and include the `VMAF library <https://github.com/Netflix/vmaf/>`__

#. Recompile the FFmpeg starting from the source code included in this repository and include the `VMAF library <https://github.com/Netflix/vmaf/>`__. For instructions on how to customize and rebuild the FFmpeg provided with the |SDK|, see the :ref:`Rebuilding FFmpeg <rebuild-ffmpeg>` section.

Example script: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/measure_vq.sh`

**Usage**::
    
    ./measure_vq.sh <Distorted Encoded Clip> <Resolution ('W'x'H')> <Framerate> <Master YUV> <Custom FFmpeg path> <VMAF Model> 

**Command Line**::

    ${FFMPEG_PATH}/ffmpeg -i $DISTORTED -framerate $FRAMERATE -s $RESOLUTION -pix_fmt yuv420p -i $MASTER \
    -lavfi libvmaf="log_fmt=json:ms_ssim=1:ssim=1:psnr=1:log_path=/tmp/${filename}.vmaf.json:model_path=${FFMPEG_PATH}/model/${MODEL}" -f null -



Explanation of the command:

- ``${FFMPEG_PATH}/ffmpeg``

  + This launches an FFmpeg, which can be overloaded/changed with a variable ``FFMPEG_PATH``, otherwise it will use the default FFmpeg in the $PATH

- ``-i $DISTORTED``
  
  + This is the encoded file which is being scored

- ``-framerate $FRAMERATE``
    
  + This is the framerate of the original (called MASTER) clip in RAW form. 
  + If your original/master clip is in an encoded format, you may omit this flag
    
- ``-s $RESOLUTION``
    
  + This is the resolution of the original (called MASTER) clip in RAW form
  + The format is ``<W>x<H>``, for example ``1920x1080`` or ``1280x720`` 

- ``-pix_fmt $PIX_FMT`` 

  + This is the colorspace of the original (called MASTER) clip in RAW form
  + To see supported pixel formats use ``${FFMPEG_PATH}ffmpeg -pix_fmts`` 

- ``-i $MASTER``

  + This takes a second input which is the master/golden/original file to which you are comparing the encoded $DISTORTED file to.
    
- ``-lavfi libvmaf=``

  + This enables the filter titled ``libvmaf`` which has the following flags/parameters in the next bullets
    
- ``log_fmt=json``
    
  + This sets the log format to JSON, you can also use ``xml`` for XML output

- ``ms_ssim=1``

  + Enables the more robust SSIM testing, MultiScale SSIM and adds the results to the logfile

- ``ssim=1``

  + Enables the standard SSIM testing, and adds it to the logfile

- ``psnr=1``
    
  + Enables the standard PSNR testing, and adds it to the logfile

- ``log_path=/tmp/${DISTORTED}.vmaf.json``

  + Sets the output logfile path

- ``model_path=${FFMPEG_PATH}/vmaf/model/${MODEL}``
    
  + **This is important** This flag determines the model that VMAF is computed against at a resolution level
  + ``vmaf_4k_v0.6.1.pkl`` is used for 4k
  + ``vmaf_float_v0.6.1.pkl`` is used for smaller resolutions (depending on version of your library may just be called ``vmaf_v0.6.1.pkl``
  + Other models exist and are documented `on the VMAF GitHub <https://github.com/Netflix/vmaf/blob/master/resource/doc/models.md>`__
    

- ``-f null -``

  + The filter (``libvmaf``) has the infrastructure to write the output log. We do not need FFmpeg to output any files, so we set the output to ``null``.

.. _quality_vs_latency:

*******************
Quality vs. Latency
*******************

A given encoder's "quality" is often a function of many different algorithms, functions and features. It is quite possible (and often seen) that an encoder can produce an H.264/HEVC compliant stream but have drastically different quality from one to another. 

Some of these features add latency, either by adding "pitstops" on the way to an output stream, or by increasing the complexity of the encoding functions. Most things in the video realm are content-dependent, or use-case-dependent, so the designer needs to determine what is best for them... a gradient of:

- Absolute best quality with high latency
- Lower quality with lowest latency. 

Low Latency Encoder Pipeline with Optimized Subjective Visual Quality
=====================================================================
- Example script for H264 streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_ll_subjective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_ll_subjective.sh`


**Usage**::

    ./h264_ll_subjective.sh <1080p60 YUV file> <bitrate in Kbps>
    ./hevc_ll_subjective.sh <1080p60 YUV file> <bitrate in Kbps>


**Command Line**::

    ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT \
    -vsync 0 \
    -b:v ${BITRATE}K \
    -c:v ${CODEC} \
    -f ${FORMAT} \
    -bf 0 \
    -g 120 \
    -periodicity-idr 120 \
    -qp-mode auto \
    -y ${INPUT}_${BITRATE}_${TEST}.${EXTENSION}

Explanation of the flags:

- ``ffmpeg``

  + The ffmpeg application, which is provided by Xilinx, and moved to the top of the PATH when you sourced the setup.sh script

- ``vsync 0``

  + Passthrough timestamp flag, each frame is passed with its timestamp from the demuxer to the muxer.

- ``-b:v <INT>K``

  + Output video bitrate in Kilo bps

- ``-c:v <CODEC>``

  + Either mpsoc_vcu_h264 or mpsoc_vcu_hevc hardware codec accelerator

- ``-f <FORMAT>``

  + Either h264, for ITU-T H.264 / MPEG-4 Part 10 AVC, or hevc, for ITU-T H.265 / MPEG-H Part 2 HEVC, video format

- ``-bf <INT>``

  + This is the number of B-Frames inserted into the GOP. B-frames reference both past and future frames, so to build them, it will be required to have a buffer.

- ``-g <INT>``

  + GOP size

- ``periodicity-idr <INT>``

  + IDR insertion period, in frame numbers

- ``-qp-mode auto``

  + Using the FPGA, we are preprocessing the stream and making intelligent decisions which we can provide to the encoder as "hints". This option gives a compromise between lower latency, via ``uniform`` and higher quality, via ``relative_load``. See :option:`-qp-mode` for more details.

Low Latency Encoder Pipeline with Optimized Objective Visual Quality
====================================================================

- Example script for H264 streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/h264_ll_objective.sh`
- Example script for HEVC streams: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/hevc_ll_objective.sh`

**Usage**::

    ./h264_ll_objective.sh <1080p60 YUV file> <bitrate in Kbps>
    ./hevc_ll_objective.sh <1080p60 YUV file> <bitrate in Kbps>


**Command Line**::

    ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT \
    -vsync 0 \
    -b:v ${BITRATE}K \
    -c:v ${CODEC} \
    -f ${FORMAT} \ 
    -bf 0 \
    -g 120 \
    -periodicity-idr 120 \
    -tune-metrics 1 \
    -y ${INPUT}_${BITRATE}_${TEST}.${EXTENSION}

Explanation of the flags:

- ``tune-metrics 1``

  + This flag optimizes video quality for objective metrics

Measuring Latency
=================

Each of the plugins from Xilinx (Decoder, Scaler, and Encoder) has the flag ``-latency_logging``. When this is enabled, tags are added to the syslog ``(/var/log/syslog)``. The script ``latency_test.sh`` will parse the syslog and report on each stage's latency, as measured in milliseconds.

Example script: :url_to_examples:`examples/u30/ffmpeg/quality_analysis/latency_test.sh`

Note: You will need to identify the PID for the ``ffmpeg`` context and have ``sudo`` to access the syslog to use this script. ``ps -aux | grep ffmpeg`` should return the PIDs of all instances of ``ffmpeg`` running on your system.

**Usage**::

    sudo ./latency_test.sh /var/log/syslog <PID>

    $ ./latency_test.sh /var/log/syslog 20796
    rm: cannot remove '*.log': No such file or directory
    Ladder contexts =  0x555b83c37c10   0x555b83d29550
    ================== Generating logs for ./syslog log file =========================
    =============== Done generating logs! Measuring now... =======================
    Frames decoded =  4757
    Average decoding latency =  125 ms
    ============== decoder done ===============
    Frames encoded =  4750
    Average encoding latency =  45 ms
    ============== encoder 1 done =============
    Total frames encoded =  4750
    Total average latency =  203 ms

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
