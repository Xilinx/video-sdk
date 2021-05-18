#
# Copyright 2020-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#

#!/bin/bash

# This script assumes an 8-bit, YUV420, pre-encoded 1080p60 h.264 file. It will scale this input into multiple renditions of various sizes,
# and send them back to disk in /tmp/

# The 1080p60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):
# 720p60 
# 720p30 
# 480p30 
# 360p30 
# 288p30 

# You may change the target codecs to HEVC by changing:
#     the first      instance  of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for decoding from HEVC

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <1080p60 h264 clip>"
    exit 1
fi

ffmpeg -c:v mpsoc_vcu_h264 -i $1 \
-filter_complex "multiscale_xma=outputs=4: \
out_1_width=1280: out_1_height=720:  out_1_rate=full: \
out_2_width=848:  out_2_height=480:  out_2_rate=half: \
out_3_width=640:  out_3_height=360:  out_3_rate=half: \
out_4_width=288:  out_4_height=160:  out_4_rate=half  \
[a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]; \
[aa]xvbm_convert[aa1];[abb]xvbm_convert[abb1];[b]xvbm_convert[b1];[c]xvbm_convert[c1]; \
[d]xvbm_convert[d1]" \
-map "[aa1]"  -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_720p60.yuv \
-map "[abb1]" -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_720p30.yuv \
-map "[b1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_480p30.yuv \
-map "[c1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_360p30.yuv \
-map "[d1]"   -pix_fmt yuv420p -f rawvideo /tmp/xil_dec_scale_288p30.yuv
