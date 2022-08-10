.. rubric:: Experimental options

========================================== ===========================
Property Name                              Description
========================================== ===========================
.. option:: entropy-mode                   | **Entropy mode for encoding process (only in H264)**
                                           | Type: Enum
                                           | Range: N/A
                                           | Default: 1 (i.e. CABAC)
.. option:: filler-data                    | **Enable/Disable Filler Data NAL units for CBR rate control**
                                           | Type: Boolean
                                           | Range: N/A
                                           | Default: true
.. option:: gdr-mode                       | **Gradual Decoder Refresh scheme mode.**
                                           | Only used if gop-mode=low-delay-p
                                           | Type: Enum
                                           | Range: N/A
                                           | Default: 0                                           
.. option:: initial-delay                  | **Initial removal delay as specified in the HRD model in msec.**
                                           | Not used when control-rate=disable
                                           | Type: Unsigned Integer
                                           | Range: 0 - 4294967295
                                           | Default: 1000
.. option:: cpb-size                       | **Coded Picture Buffer as specified in the HRD model in msec.**
                                           | Not used when control-rate=disable
                                           | Type: Unsigned Integer
                                           | Range: 0 - 4294967295
                                           | Default: 2000
.. option:: constrained-intra-prediction   | **Intra-prediction control**
                                           | If enabled, prediction only uses residual data and decoded samples from neighboring coding blocks coded using intra prediction modes
                                           | Type: Boolean
                                           | Range: N/A
                                           | Default: false
.. option:: loop-filter-mode               | **Enable or disable the deblocking filter**
                                           | Type: Enum
                                           | Range: N/A
                                           | Default: 0 (i.e. enable)
.. option:: prefetch-buffer                | **Enable/Disable L2 Cache buffer in encoding process**
                                           | Type: Boolean
                                           | Range: N/A
                                           | Default: true
.. option:: num-slices                     | **Number of slices produced for each frame.**
                                           | Each slice contains one or more complete macroblock/CTU row(s). Slices are distributed over the frame as regularly as possible.
                                           | If slice-size is defined as well more slices may be produced to fit the slice-size requirement.
                                           | In low-latency mode H.264(AVC): 32, H.265 (HEVC): 22
                                           | In normal latency-mode H.264(AVC): picture_height/16, H.265(HEVC): minimum of picture_height/32
                                           | Type: Unsigned Integer
                                           | Range: 1 - 68
                                           | Default: 1                                           
.. option:: slice-size                     | **Target slice size (in bytes)**
                                           | The encoder uses this value to automatically split the bitstream into approximately equally-sized slices
                                           | Type: Unsigned Integer
                                           | Range: 0 - 65535
                                           | Default: 0
.. option:: b-frames                       | Number of B-frames between two consecutive P-frames.
                                           | This property can be changed in PAUSED/PLAYING state of the element when ``gop-mode`` is basic.
                                           | Type: Unsigned Integer
                                           | Range: 0 - 4294967295
                                           | Default: 2
.. option:: gop-mode                       | Group of Pictures mode
                                           | Type: Enum
                                           | Range: N/A
                                           | Default: 0 (i.e. basic)
========================================== ===========================

..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.