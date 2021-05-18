# The FFmpeg U30 SDK v1.0.0 patch

This folder contains a git patch file which can be applied to a FFmpeg fork to enable the Xilinx U30 plugins.

This patch is designed to apply to FFmpeg n4.1. As such, applying this patch to earlier or later versions of FFmpeg may require edits to successfully merge these changes and represent untested configurations.

The patch adds new plugins to FFmpeg to enable access to Xilinx U30 Video accelerators.  In addition, the patch makes edits to FFmpeg to support new command line options to support U30-based FPGA use cases as well as ensure proper initialization of Xilinx accelerators.

For instructions on how to apply this patch file to a FFmpeg fork, refer to the U30 Video SDK online documentation [U30 Video SDK online documentation](https://xilinx.github.io/video-sdk/docs/using_ffmpeg.html#using-the-git-patch-file)

---------------------------------------------------

© Copyright 2020-2021 Xilinx, Inc.

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.