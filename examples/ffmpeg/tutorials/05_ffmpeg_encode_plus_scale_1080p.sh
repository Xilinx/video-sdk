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

# This script accepts an 8-bit, YUV420, 1080p60 RAW file and will send it to the scaler, which outputs various renditions of various sizes,
# and sends them to the encoder targeting various bitrates (as defined by the -b:v flag). All outputs will be stored at /tmp/

# You may edit this to accept other sizes (-s), other framerates (-r), other output bitrates (-b:v).

# The 1080p60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):
# 720p60 4.0   Mbps
# 720p30 3.0   Mbps
# 848p30 2.5   Mbps
# 360p30 1.25  Mbps
# 288p30 0.625 Mbps

# You may edit this to enable other output bitrates (-b:v).
# You may also save the output file by replacing /dev/null with an output path.
# You may change the output framerate via the (-r) flags

# You may change the target codecs to HEVC by changing:
#     (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding into HEVC


if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <1080p60 YUV file>"
    exit 1
fi

ffmpeg -f rawvideo -s 1920x1080 -r 60 -pix_fmt yuv420p -i $1 \
-filter_complex "multiscale_xma=outputs=4: \
out_1_width=1280: out_1_height=720: out_1_rate=full:   \
out_2_width=848:  out_2_height=480: out_2_rate=half:   \
out_3_width=640:  out_3_height=360: out_3_rate=half:   \
out_4_width=288:  out_4_height=160: out_4_rate=half    \
[a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]"  \
-map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_720p60.mp4 \
-map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_720p30.mp4 \
-map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_480p30.mp4 \
-map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_360p30.mp4 \
-map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_scale_enc_288p30.mp4
