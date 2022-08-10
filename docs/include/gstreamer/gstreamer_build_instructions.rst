.. _rebuild-gstreamer:

**********************************************************
Rebuilding GStreamer
**********************************************************

Source code and build scripts for the GStreamer plugins developed by Xilinx can be found the in the :file:`sources/video-sdk-gstreamer` folder of the |SDK| repository.

Follow these steps to rebuild GStreamer with the |SDK| plugins enabled:

#. Navigate the top of the |SDK| repository::

    cd /path/to/video-sdk

#. Make sure the sources have been downloaded from the repository::
    
    git submodule update --init --recursive

#. Navigate to the directory containing the GStreamer sources::

    cd sources/video-sdk-gstreamer

#. Download, build, patch and install the GStreamer packages::
  
   ./install_gst.sh

#. Clean the Xilinx GStreamer plugin libraries::

    ./clean_vvas.sh

#. Build and install the Xilinx GStreamer plugins::
  
    ./build_install_vvas.sh PCIe 1


**NOTE**: The script :file:`sources/video-sdk-gstreamer/install_gst.sh` script deletes the source files after the installation. In order to preserve the patched open source GStreamer sources, use the :file:`sources/video-sdk-gstreamer/retrieve_gst_sources.sh` script. This script clones the open source GStreamer sources and applies the Xilinx specific patches on top of it. The patched sources are available in the following folders:

- /tmp/gst-libav-1.16.2
- /tmp/gst-plugins-bad-1.16.2
- /tmp/gst-plugins-base-1.16.2
- /tmp/gst-plugins-good-1.16.2
- /tmp/gstreamer-1.16.2

..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
