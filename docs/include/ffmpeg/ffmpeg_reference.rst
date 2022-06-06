*********************************************
Xilinx FFmpeg Reference Guide
*********************************************

H.264 Codec Reference
====================================

H.264 Decoder Options
------------------------------------
The entire list options for the Xilinx H.264 decoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h decoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 decoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_h264_decode.rst
   :language: none


H.264 Encoder Options
------------------------------------
The entire list options for the Xilinx H.264 encoder (mpsoc_vcu_h264) can be displayed using the following command::

  ffmpeg -h encoder=mpsoc_vcu_h264

The mpsoc_vcu_h264 encoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_h264_encode.rst
   :language: none


HEVC Codec Reference
====================================

HEVC Decoder Options
------------------------------------
The entire list options for the Xilinx HEVC decoder (mpsoc_vcu_hevc) can be displayed using the following command::

  ffmpeg -h decoder=mpsoc_vcu_hevc

The mpsoc_vcu_hevc decoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_hevc_decode.rst
   :language: none


HEVC Encoder Options
------------------------------------
The entire list options for the Xilinx HEVC encoder (mpsoc_vcu_hevc) can be displayed using the following command::

  ffmpeg -h encoder=mpsoc_vcu_hevc

The mpoc_vcu_hevc encoder has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_hevc_encode.rst
   :language: none


Multiscaler Filter Reference
====================================
The entire list options for the Xilinx Multiscaler (multiscale_xma) can be displayed using the following command::

  ffmpeg -h filter=multiscale_xma

The multiple output hardware scaling filter has the following options:

.. literalinclude:: ./include/ffmpeg/ffmpeg_help_multiscaler.rst
   :language: none

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.