#######################################
|SDK|
#######################################

The |SDK| is a complete software stack allowing users to seamlessly leverage the hardware accelerated features of Xilinx video codec units and enable high-density real-time transcoding for live streaming video service providers, OEMs, and Content Delivery Network (CDNs). Included in the |SDK| is pre-compiled version of FFmpeg which integrates key video transcoding plug-ins, enabling simple hardware offloading of compute-intensive workloads using the popular FFmpeg command line interface. The |SDK| also provides a C-based application programming interface (API) which facilitates the integration of Xilinx video codec units transcoding capabilities in proprietary frameworks. The |SDK| can be leveraged on-premises with Alveo U30 cards and in the cloud with |VT1| instances.

.. raw:: html

   <iframe 
     width="640" height="340"
     src="https://www.youtube.com/embed/kv-OS_cFOvg" 
     title="YouTube video player" 
     frameborder="0">
   </iframe>

.. .. image:: https://www.xilinx.com/content/dam/xilinx/imgs/kits/alveo-u30-hero.jpg
..   :width: 250
..   :alt: The Alveo U30 card is targeted at video workloads using FFmpeg, a leading multimedia framework. Xilinx has integrated key video transcoding plug-ins into FFmpeg, enabling simple hardware offloading of compute-intensive workloads.

*************************
Quick Links
*************************

- :doc:`Specs and Features </specs_and_features>`
- :doc:`Release Notes </release_notes>`
- :doc:`Get Started on Premises with Alveo U30 </getting_started_on_prem>`
- :doc:`Get Started in the Cloud with Amazon EC2 VT1 </getting_started_on_vt1>`
- :doc:`Tutorials and Examples </examples>`


.. toctree::
   :maxdepth: 3
   :caption: Get Started
   :hidden:

   Release Notes </release_notes>
   On Premises </getting_started_on_prem>
   Amazon EC2 VT1 </getting_started_on_vt1>
   Container Setup </container_setup> 
   Tutorials and Examples </examples>


.. toctree::
   :maxdepth: 3
   :caption: Reference Guides
   :hidden:

   Specs and Features </specs_and_features>
   Using FFmpeg </using_ffmpeg>   
   Managing Compute Resources </managing_compute_resources>   
   Deploying with Kubernetes </deploying_with_kubernetes>
   C API Programming Guide </c_apis>   
   Card Management </card_management> 


.. toctree::
   :maxdepth: 3
   :caption: Support
   :hidden:

   File an issue <https://github.com/Xilinx/video-sdk/issues>
   Contributing </contributing> 
   Other versions <https://xilinx.github.io/video-sdk/browse.html>

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.