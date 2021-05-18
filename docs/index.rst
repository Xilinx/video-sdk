#######################################
Xilinx Video SDK
#######################################

The Xilinx Video SDK is a complete software stack allowing users to seamlessly leverage the hardware accelerated features of Alveo U30 cards and enable high-density real-time transcoding for live streaming video service providers, OEMs, and Content Delivery Network (CDNs). Included in the Xilinx Video SDK is pre-compiled version of FFmpeg which integrates key video transcoding plug-ins, enabling simple hardware offloading of compute-intensive workloads using the popular FFmpeg command line interface. The Xilinx Video SDK also provides a C-based application programming interface (API) which facilitates the integration of Alveo U30 transcoding capabilities in proprietary frameworks. 

|

*************************
Quick Start Links
*************************

- :doc:`Specs and Features </introduction>`
- :doc:`Release Notes </release_notes>`
- :doc:`Installation Instructions </installation>`
- :doc:`Examples and Tutorials </examples/ffmpeg/tutorials>`

|

.. image:: https://www.xilinx.com/content/dam/xilinx/imgs/kits/alveo-u30-hero.jpg
  :width: 400
  :alt: The Alveo U30 card is targeted at video workloads using FFmpeg, a leading multimedia framework. Xilinx has integrated key video transcoding plug-ins into FFmpeg, enabling simple hardware offloading of compute-intensive workloads.




.. toctree::
   :maxdepth: 3
   :caption: Installation and Setup
   :hidden:

   Release Notes </release_notes>
   Installation Instructions </installation>
   Environment Setup </setup>


.. toctree::
   :maxdepth: 3
   :caption: Tutorials and Examples
   :hidden:

   FFmpeg </examples/ffmpeg/tutorials>
   Software Filters </examples/ffmpeg/filters>
   Visual Quality </examples/ffmpeg/quality_analysis>
   XMA-based Apps </examples/xma/xma_apps>


.. toctree::
   :maxdepth: 3
   :caption: User Guide
   :hidden:

   Specs and Features </introduction>
   Using FFmpeg </using_ffmpeg>   
   Running Multiple Jobs </running_multiple_jobs>   
   C API Programming Guide </c_apis>   
   Card Management and Recovery </card_management> 


.. toctree::
   :maxdepth: 3
   :caption: Support
   :hidden:

   File an issue <https://github.com/Xilinx/video-sdk/issues>
   Other versions <https://xilinx.github.io/video-sdk/browse.html>


..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.