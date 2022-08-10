*********************************************************
Video Encoding
*********************************************************

For the complete list of features and capabilities of the Xilinx hardware encoder, refer to the :ref:`Video Codec Unit <video-codec-unit>` section of the :doc:`Specs and Features </specs_and_features>` chapter of the documentation.

The Xilinx video encoder is leveraged in FFmpeg by setting the :option:`-c:v` option to ``mpsoc_vcu_hevc`` for HEVC or to ``mpsoc_vcu_h264`` for H.264.

The table below describes all the options for the Xilinx video encoder.

.. table:: 
   :widths: 30, 70

   =============================  ===========================
   Options                        Descriptions
   =============================  ===========================
   .. option:: -b:v               | **Specify the video bitrate**
                                  | You can specify this in Mb or Kb. For example -b:v 1M or -b:v 1000K.
                                  | Can be specified in Mb or Kb. For example -b:v 1M or -b:v 1000K
                                  | The bitrate can also be adjusted dynamically. Consult the :ref:`Dynamic Encoder Parameters <dyn-parameters>` section for more details on how to change this option during runtime.
   .. option:: -max-bitrate       | **Maximum bitrate**
                                  | Valid values: 0 to 3.5e+10 (default 5e+06)
                                  | You may want to use this to limit encoding bitrate if you have not specified a :option:`-b:v` bitrate                               
   .. option:: -g                 | **GOP size** 
                                  | Set the GOP size to 2x frame rate for a 2 second GOP 
                                  | The maximum supported GOP size is 1000.    
   .. option:: -aspect-ratio      | **Aspect ratio**
                                  | Valid values: 0 to 3 (default 0)
                                  |  (0) auto - 4:3 for SD video, 16:9 for HD video, unspecified for unknown format
                                  |  (1) 4:3 
                                  |  (2) 16:9 
                                  |  (3) none - Aspect ratio information is not present in the stream
   .. option:: -cores             | **Number of encoder cores in the Xilinx device to utilize**
                                  | Valid values: 0 to 4 (default 0)
                                  |  (0) auto
                                  | The FFmpeg encoder plugin automatically determines how many encoder cores are needed to sustain real-time performance (e.g. 1 for 1080p60, 4 for 4K60). The :option:`-cores` option can be used to manually specify how many encoder cores are to be used for a given job. When encoding file-based clips with a resolution of 1080p60 or lower, leveraging additional cores may increase performance. This option will provide diminishing returns when multiple streams are processed on the same device. This option has no impact on live streaming use-cases as a video stream cannot be processed faster than it is received.
   .. option:: -slices            | **Number of slices to operate on at once within a core**
                                  | Valid values: 1 to 68 (default 1)
                                  | Slices are a fundamental part of the stream format. You can operate on slices in parallel to increase speed at which a stream is processed. However, operating on multiple "slices" of video at once will have a negative impact on video quality. This option must be used when encoding 4k streams to H.264 in order to sustain real-time performance. The maximum practical value for this option is 4 since there are 4 encoder cores in a device.
   .. option:: -level             | **Encoding level restriction** 
                                  | If the user does not set this value, the encoder will automatically assign appropriate level based on resolution, frame rate and bitrate
                                  | Valid values for H.264: 1, 1.1, 1.2, 1.3, 2, 2.1, 2.2, 3, 3.1, 3.2, 4, 4.1, 4.2, 5, 5.1, 5.2 
                                  | Valid values for HEVC: 1, 2, 2.1, 3, 3.1, 4, 4.1, 5, 5.1, 5.2 
   .. option:: -profile           | **Set the encoding profile** 
                                  | Valid values for H.264: baseline (66), main (77), high (100, default), high-10 (110), high-10-intra (2158)
                                  | Valid values for HEVC: main (0, default), main-intra (1), main-10 (2), main-10-intra (3)
   .. option:: -tier              | **Set the encoding tier** (HEVC only)
                                  | Valid values: 0 to 1 (default is 0)
                                  |  (0) main - Main tier
                                  |  (1) high - High tier 
   .. option:: -bf                | **Number of B frames**
                                  | Valid values: 0 to 4 (default is 2) 
                                  | For tuning use 1 or 2 to improve video quality at the cost of latency. Consult the :ref:`B Frames <tuning-b-frames>` section for more details on how to use this option.
   .. option:: -lookahead_depth   | **Number of frames to lookahead for qp maps**
                                  | Valid values: 0 (default) to 20 
                                  | For tuning set this to 20 to improve subjective video quality at the cost of latency. Consult the :ref:`Lookahead <tuning-lookahead>` section for more details on how to use this option.
   .. option:: -spatial-aq        | **Enable spatial AQ**
                                  | Valid values: 0 or 1 (default)
                                  |  (0) disable 
                                  |  (1) enable - Default
                                  | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
   .. option:: -spatial-aq-gain   | Percentage of spatial AQ gain.
                                  | Valid values: 0 to 100 (default 50)
                                  | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
   .. option:: -temporal-aq       | **Enable temporal AQ**
                                  | Valid values: 0 or 1 (default)
                                  |  (0) disable 
                                  |  (1) enable - Default
                                  | Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.                            
   .. option:: -scaling-list      | **Determine if the quantization values are auto scaled**
                                  | Valid values:  0, 1 (default)
                                  |  (0) flat - Flat scaling list mode, improves objective metrics
                                  |  (1) default - Default scaling list mode, improves video quality
                                  | Consult the :ref:`Scaling List <tuning-scaling-list>` section for more details on how to use this option.                               
   .. option:: -qp-mode           | **QP control mode**
                                  | Valid values: 0 to 2 (default 1)
                                  |  (0) uniform
                                  |  (1) auto - default
                                  |  (2) relative_load
                                  | For best objective scores use ``uniform``. For best subjective quality use ``auto`` or ``relative_load``. Consult the :ref:`Adaptive Quantization <tuning-aq>` section for more details on how to use this option.
   .. option:: -control-rate      | **Set the Rate Control mode**
                                  | Valid values: 0 to 3 (default is 1)
                                  |  (0) Constant QP
                                  |  (1) Constant Bitrate - default
                                  |  (2) Variable Bitrate
                                  |  (3) Low Latency
   .. option:: -min-qp            | **Minimum QP value allowed for rate control**
                                  | Valid values: 0 to 51 (default 0)
                                  | This option has no effect when :option:`-control-rate` is set to Constant QP (0).
   .. option:: -max-qp            | **Maximum QP value allowed for rate control**
                                  | Valid values: 0 to 51 (default 51)
                                  | This option has no effect when :option:`-control-rate` is set to Constant QP (0).
   .. option:: -slice-qp          | **Slice QP**
                                  | Valid values: -1 to 51 (default -1)
                                  |  (-1) auto 
                                  | This sets the QP values when :option:`-control-rate` is Constant QP (0). The specified QP value applies to all the slices. This parameter can also be used to provide QP for the first Intra frame when :option:`-lookahead_depth` = 0. When set to -1, the QP for first Intra frame is internally calculated.
   .. option:: -periodicity-idr   | **IDR frame frequency**
                                  | Valid values: -1 to INT_MAX32 (default -1)
                                  | If this option is not specified, a closed GOP is generated: the IDR periodicity is aligned with the GOP size and IDR frame is inserted at the start of each GOP. To insert IDR frames less frequently, use a value which is a multiple of the GOP size.
   .. option:: -force_key_frames  | **Force insertion of IDR frames**
                                  | Valid values: time[,time...] or expr:expr
                                  | Force an IDR frame to be inserted at the specified frame number(s). Consult the :ref:`Dynamic IDR Frame Insertion <dyn-idr-frame>` section for more details on how to use this option.
   .. option:: -expert-options    | **Set advanced encoding options**
                                  | Valid values: dynamic-params=<options file>
                                  | Consult the :ref:`Dynamic Encoder Parameters <dyn-parameters>` section for more details on how to use this option.
   .. option:: -tune-metrics      | **Enable tuning video quality for objective metrics**
                                  | Valid value: 0, 1 (default 0)
                                  |  (0) disable - Disable tune metrics 
                                  |  (1) enable - Enable tune metrics    
                                  | Enabling :option:`-tune-metrics` automatically forces :option:`-qp-mode` = ``uniform`` and :option:`-scaling-list` = ``flat``, overwritting any explicit user settings of two values. This option improves objective quality by providing equal importance to all the blocks in the frame: the same quantization parameters and transform coefficients are used for all of them. 
                                  | This option should be disabled when measuring subjective quality or visually checking the video.
                                  | This option should be enabled when measuring objective metrics such as PSNR/SSIM/VMAF.  
   .. option:: -vsync             | **Add a vsync frame**
                                  | Valid values: 0, 1
                                  | Set this to 0 to prevent extra frames being added.                               
   =============================  ===========================

..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
