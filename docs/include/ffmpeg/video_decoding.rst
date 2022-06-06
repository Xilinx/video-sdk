*********************************************************
Video Decoding
*********************************************************

For the complete list of features and capabilities of the Xilinx hardware decoder, refer to the :ref:`Video Codec Unit <video-codec-unit>` section of the :doc:`Specs and Features </specs_and_features>` chapter of the documentation.

The Xilinx video decoder is leveraged in FFmpeg by setting the :option:`-c:v` option to ``mpsoc_vcu_hevc`` for HEVC or to ``mpsoc_vcu_h264`` for H.264.

The table below describes all the options for the Xilinx video decoder.

====================================  ===========================
Options                               Descriptions
====================================  ===========================
.. option:: -low_latency              | Configure decoder to handle out-of-order frames in order to decrease the latency of the system.
                                      | IMPORTANT: This option should not be used when processing streams containing B frames. 
                                      | Valid values: 0 (default) and 1
.. option:: -entropy_buffers_count    | Specify number of internal entropy buffers.
                                      | Valid values: 2 (default) to 10
                                      | Can be used to improve the performance for input streams with a high bitrate (including 4k streams) or a high number of reference frames. 2 is enough for most cases. 5 is the practical limit.
.. option:: -splitbuff_mode           | Configure decoder in split/unsplit input buffer mode.
                                      | Valid values: 0 (default) and 1
====================================  ===========================



..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.