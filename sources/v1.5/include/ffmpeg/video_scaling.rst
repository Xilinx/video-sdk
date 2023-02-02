.. _using-ffmpeg-for-video-scaling:

*****************************************************
Video Scaling
*****************************************************

Xilinx devices provide hardware-accelerated video decoding, scaling, and encoding. Each device supports multiple input channels (raw or encoded) up to a total equivalent bandwidth of 4kp60. Using the Multiscale XMA FFmpeg plug-in included in the |SDK|, each input channel can be scaled in hardware to multiple lower resolution and/or lower frame rate outputs.

The |SDK| supports the following scaling features and capabilities:

- Up to 32 input streams of raw or encoded video can be scaled down per device
- Each input stream can be scaled down to a maximum of 8 outputs streams of lower resolution and/or lower frame rate
- Up to 32 scaled outputs streams are supported per device, up to a maximum total equivalent bandwidth of 4kp60
- The scaler supports spatial resolutions from 3840x2160 to 128x128, in multiples of 4 
- Scaled output streams can optionally be encoded to H.264 of HEVC, using the same codec for all streams
- The scaler passes scaled frames and meta data to the next scaling level (if one is defined) and to the encoder (if one is being used)
- Each level of scaling adds a little more latency to the pipeline

For additional details about the specification of the hardware scaler, refer to the :ref:`Adaptive Bitrate Scaler features <adaptive-bitrate-scaler>` section in the introductory chapter of this user guide. 

The figure below illustrates a scaling ladder with a 1920x1080 input and 4 outputs with resolutions of 1280x720, 852x480, 640x360, and 416x240, respectively. 

.. image:: ./images/abr-ladder.png
    :alt: output from one rung is passed to the next for further scaling
    :align: center


**IMPORTANT:** The frame rate and resolution of a given output should be smaller or equal than the rate of the previous output. Since the output of one scaling stage is passed as an input to the next, video quality will be negatively affected if frame rate is increased after it has been lowered.

|

Multiscale Filter Options
==========================================

An ABR ladder is created using the :option:`multiscale_xma` complex filter and the `FFmpeg filter graph syntax <https://ffmpeg.org/ffmpeg-filters.html#Filtergraph-syntax-1>`_. This section describes the options of the :option:`multiscale_xma` complex filter.

.. option:: multiscale_xma

  Filter implementing the Xilinx ABR multiscaler. Takes one input and up to 8 output streams. The complete list of options is described below.


.. list-table:: Multiscale Filter Options
   :widths: 25 75
   :header-rows: 1

   * - Options
     - Description
   * - .. option:: outputs    
     - | **Specify the number of scaler outputs** 
       | Valid values are integers between 1 and 8     
   * - .. option:: out_{N}_width
     - | **Specify the width of each of the scaler outputs**
       | The output number {N} must be an integer value between 1 and 8, and must not exceed the number of outputs 
       | specified with :option:`outputs`     
       | Valid values are integers between 3840 and 128, in multiples of 4  
       | The frame resolution of a given output should be smaller or equal than the resolution of the previous output
   * - .. option:: out_{N}_height
     - | **Specify the height of each of the scaler outputs**    
       | The output number {N} must be an integer value between 1 and 8, and must not exceed the number of outputs 
       | specified with :option:`outputs`     
       | Valid values are integers between 2160 and 128, in multiples of 4  
       | The frame resolution of a given output should be smaller or equal than the resolution of the previous output
   * - .. option:: out_{N}_rate
     - | **Specify the frame rate of each of the scaler outputs**
       | By default, the scaler uses the input stream frame rate for all outputs. While the encoder supports frame 
       | dropping with the -r option, there is also hardware support in the scaler for dropping frames. Dropping 
       | frames in the scaler is preferred since it saves scaler bandwidth, allowing the scaler and encoder to operate 
       | more efficiently. 
       | The output number {N} must be an integer value between 1 and 8, and must not exceed the number of outputs 
       | specified with :option:`outputs`
       | Valid values are ``full`` (default) and ``half``                 
       | The first output has to be full rate output (``out_1_rate=full``)
       | The frame rate of a given output should be smaller or equal than the resolution of the previous output

|

.. _using-the-multiscale-filter:

Using the Multiscale Filter
==========================================

The filter graph specification for the :option:`multiscale_xma` filter should be constructed in the following way:

- Add the :option:`multiscale_xma` filter to the graph   
- Set the number of scaler outputs
- Set the width, height, and rate settings for each scaler output 
- Define the name each scaler output
- If the outputs are not to encoded on the device, add :option:`xvbm_convert` filters to the filter graph to copy the frames back to the host and convert them to AV frames.

The following example shows a complete command to decode, scale and encode to five different resolutions::

    ffmpeg -c:v mpsoc_vcu_h264 -i input.mp4 \
      -filter_complex " \
        multiscale_xma=outputs=4: \
        out_1_width=1280: out_1_height=720: out_1_rate=full: \
        out_2_width=848:  out_2_height=480: out_2_rate=half: \
        out_3_width=640:  out_3_height=360: out_3_rate=half: \
        out_4_width=288:  out_4_height=160: out_4_rate=half  \
        [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
      -map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f mp4 -y ./scaled_720p60.mp4 \
      -map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f mp4 -y ./scaled_720p30.mp4 \
      -map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f mp4 -y ./scaled_480p30.mp4 \
      -map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f mp4 -y ./scaled_360p30.mp4 \
      -map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f mp4 -y ./scaled_288p30.mp4

This example can also be found in the FFMpeg introductory tutorials: :ref:`Transcode With Multiple-Resolution Outputs <transcode-with-abr-ladder>`.


Encoding Scaler Outputs
-----------------------

The outputs of an ABR ladder can be encoded on the device using either the ``mpsoc_vcu_h264`` or the ``mpsoc_vcu_hevc`` codec. **IMPORTANT:** All outputs must be encoded using the same codec.


Using Raw Scaler Outputs
------------------------

To return raw video outputs from the ABR ladder, use the :option:`xvbm_convert` filter to copy the frames from the device to the host and convert them to AV frames. The converted AV frames can then be used in FFmpeg software filters or directly saved to file as shown in this command::

  ffmpeg -c:v mpsoc_vcu_h264 -i input.mp4 \
  -filter_complex " \
    multiscale_xma=outputs=4: \
    out_1_width=1280: out_1_height=720:  out_1_rate=full: \
    out_2_width=848:  out_2_height=480:  out_2_rate=half: \
    out_3_width=640:  out_3_height=360:  out_3_rate=half: \
    out_4_width=288:  out_4_height=160:  out_4_rate=half  \
    [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]; \
    [aa]xvbm_convert[aa1];[abb]xvbm_convert[abb1];[b]xvbm_convert[b1];[c]xvbm_convert[c1]; \
    [d]xvbm_convert[d1]" \
  -map "[aa1]"  -pix_fmt yuv420p -f rawvideo ./scaled_720p60.yuv \
  -map "[abb1]" -pix_fmt yuv420p -f rawvideo ./scaled_720p30.yuv \
  -map "[b1]"   -pix_fmt yuv420p -f rawvideo ./scaled_480p30.yuv \
  -map "[c1]"   -pix_fmt yuv420p -f rawvideo ./scaled_360p30.yuv \
  -map "[d1]"   -pix_fmt yuv420p -f rawvideo ./scaled_288p30.yuv

This example can also be found in the FFMpeg introductory tutorials: :ref:`Decode Only Into Multiple-Resolution Outputs <decode-and-scale-only>`.


Performance Considerations
==========================
Encoded input streams with a high bitrate or with a high number of reference frames can degrade the performance of an ABR ladder. The :option:`-entropy_buffers_count` decoder option can be used to help with this. A value of 2 is enough for most cases, 5 is the practical limit.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.