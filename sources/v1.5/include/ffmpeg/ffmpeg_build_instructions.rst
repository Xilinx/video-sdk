.. _rebuild-ffmpeg:

*********************
Rebuilding FFmpeg
*********************

There are two methods for rebuilding FFmpeg with the |SDK| plugins enabled:

- Using the complete source code
- Using the git patch file

Using the Source Code
============================

The `sources/app-ffmpeg4-xma <https://github.com/Xilinx/app-ffmpeg4-xma/tree/v4.1.10>`_ submodule contains the entire source code for the FFmpeg executable included with the Video SDK. This is a fork of the main FFmpeg GitHub (release 4.1, commid ID bb01cd3cc01c9982e4b57f8ce5cfd6ec4724f848) with a :url_to_repo_folder:`Xilinx patch applied <sources/app-ffmpeg4-xma-patch>` to enable the |SDK| plugins. Due to licensing restrictions, the FFmpeg executable included in the Video SDK is enabled with the |SDK| plugins only. 

You can rebuild the FFmpeg executable with optional plugins by following the instructions below. Additionally, comprehensive instructions for compiling FFmpeg can be found `on the FFmpeg wiki page <https://trac.ffmpeg.org/wiki/CompilationGuide>`_. 

#. Make sure ``nasm`` and ``yasm`` are installed on your machine. 

#. Navigate the top of the |SDK| repository::

    cd /path/to/video-sdk

#. Make sure the sources have been downloaded from the repository::
    
    git submodule update --init --recursive

#. Navigate to the directory containing the FFmpeg sources::

    cd sources/app-ffmpeg4-xma

#. Optionally install FFmpeg plugins you wish to enable (either from source or from your package manager like ``yum`` or ``apt``). For example: libx264, or libx265.

#. Configure FFmpeg with ``--enable`` flags to enable the desired plugins. The ``-enable-libxma2api`` flag enables the |SDK| plugins. The command below will configure the Makefile to install the custom FFmpeg in the :file:`/tmp/ffmpeg` directory. To install in another location, modify the ``--prefix`` and ``--datadir`` options::

    ./configure --prefix=/tmp/ffmpeg --datadir=/tmp/ffmpeg/etc  --enable-x86asm --enable-libxma2api --disable-doc --enable-libxvbm --enable-libxrm --extra-cflags=-I/opt/xilinx/xrt/include/xma2 --extra-ldflags=-L/opt/xilinx/xrt/lib --extra-libs=-lxma2api --extra-libs=-lxrt_core --extra-libs=-lxrt_coreutil --extra-libs=-lpthread --extra-libs=-ldl --disable-static --enable-shared

#. Build and install the FFmpeg executable::

    make -j && sudo make install

#. The :file:`/opt/xilinx/xcdr/setup.sh` script puts the Xilinx-provided FFmpeg in the :envvar:`PATH` environment variable. To use the newly built FFmpeg, update your :envvar:`PATH` or provide the full path to the custom-built executable. 

|

Using the Git Patch File
===============================

The :url_to_repo_folder:`Xilinx patch applied <sources/app-ffmpeg4-xma-patch>` folder contains a git patch file which can be applied to a FFmpeg fork to enable the |SDK| plugins.

This patch is designed to apply to FFmpeg n4.1. As such, applying this patch to earlier or later versions of FFmpeg may require edits to successfully merge these changes and represent untested configurations.

The patch makes edits to FFmpeg and adds new plugins to FFmpeg to initialize, configure and use Xilinx video accelerators.

Here is an example of how the patch can be applied to a FFmpeg fork:

#. Clone the n4.1 version of FFmpeg::

    git clone https://github.com/FFmpeg/FFmpeg.git -b n4.1

#. After the git clone, you will have a directory named FFmpeg. Enter this directory::

    cd FFmpeg

#. Copy the patch file into the FFmpeg directory::

    cp /path/to/sources/app-ffmpeg4-xma-patch/0001-Add-plugins-to-support-U30-based-Video-SDK-v1.5.0.patch .

#. Apply the patch::

    git am 0001-Add-plugins-to-support-U30-based-Video-SDK-v1.5.0.patch --ignore-whitespace --ignore-space-change

#. You can then install additional FFmpeg plugins and proceed with the FFmpeg build and installation process.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
