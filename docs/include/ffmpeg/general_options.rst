
General FFmpeg Options
======================

.. table:: 
   :widths: 30, 70

   =============================  ===========================
   Options                        Descriptions
   =============================  ===========================
   .. option:: -i                 | The input file.
   .. option:: -c:v               | Specify the video codec. 
                                  | This option must be set for any video stream sent to a Xilinx device.
                                  | Valid values are ``mpsoc_vcu_hevc`` (for HEVC) or ``mpsoc_vcu_h264`` (for H.264)
   .. option:: -s                 | The frame size (WxH). For example 1920x1080 or 3840x2160.
   .. option:: -f                 | The container format.
   .. option:: -r                 | The frame rate in fps (Hz).
   .. option:: -filter_complex    | Used to specify ABR scaling options. Consult the section about :ref:`Using the Xilinx Multiscale Filter <using-the-multiscale-filter>` for more details on how to use this option.
   .. option:: -xlnx_hwdev        | Global option used to specify on which Xilinx device the FFmpeg job should run. Consult the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` section for more details on how to use this option.
                                  | Valid values are positive integers. Default is device 0. 
   .. option:: -lxlnx_hwdev       | Component-level option used to specify on which Xilinx device a specific component of the FFmpeg job should be run. When set, it overwrites the global :option:`-xlnx_hwdev` option for that component. Consult the :ref:`Using Explicit Device IDs <using-explicit-device-ids>` section for more details on how to use this option.
                                  | Valid values are positive integers. Default is device 0.
   .. option:: -latency_logging   | Log latency information to syslog. 
                                  | Valid values: 0 (disabled, default) and 1 (enabled)
   .. option:: -loglevel          | Configures the FFmpeg log level.
                                  | Setting this option to ``debug`` displays comprehensive debug information about the job 
   =============================  ===========================

..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.