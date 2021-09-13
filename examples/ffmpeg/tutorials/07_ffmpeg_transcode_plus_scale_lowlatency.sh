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

# This script assumes an 8-bit, YUV420, pre-encoded 60FPS h.264 file. It will scale this input into multiple renditions of various sizes,
# and send them to the encoder targeting various bitrates (as defined by the -b:v flag).

# This is a LOW-LATENCY version: b-frames are removed, as well as a reduced lookahead. This means 2 things:
# 1) The decoder does not accept B-Frames (out of order decoding). If you provide a clip with B-Frames, you will receive an invalid output. 
#    If your input has B-Frames, simply remove ``-low_latency 1``
# 2) The encoder will not produce B-Frames in its output


# The 1080p60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):
# 720p60 4.0   Mbps
# 720p30 3.0   Mbps
# 848p30 2.5   Mbps
# 360p30 1.25  Mbps
# 288p30 0.625 Mbps

# You may edit this to enable other output bitrates (-b:v).
# You may change the output framerate via the (-r) flags

# You may change the target codecs to HEVC by changing:
#     the first      instance  of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for decoding from HEVC
#     the subsequent instances of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding into HEVC

# The command below doesn't handle the audio channel of the input video. For an example of how to include
# audio in the output streams, refer to the example commented out at the bottom of this script.

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <1080p60 h264 clip>"
    exit 1
fi

ffmpeg -c:v mpsoc_vcu_h264 -low_latency 1 -i $1 \
-filter_complex "multiscale_xma=outputs=4: \
out_1_width=1280: out_1_height=720: out_1_rate=full:   \
out_2_width=848:  out_2_height=480: out_2_rate=half:   \ 
out_3_width=640:  out_3_height=360: out_3_rate=half:   \
out_4_width=288:  out_4_height=160: out_4_rate=half    \
[a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
-map "[aa]"  -b:v 4M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_720p60.mp4 \
-map "[abb]" -b:v 3M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_720p30.mp4 \
-map "[b]"   -b:v 2500K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_480p30.mp4 \
-map "[c]"   -b:v 1250K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_360p30.mp4 \
-map "[d]"   -b:v 625K  -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_ll_xcode_scale_288p30.mp4


# FFmpeg command to process audio as well as video. 
# Should only be used if the input video has an audio channel.

# ffmpeg -c:v mpsoc_vcu_h264 -low_latency 1 -i $1 \
# -filter_complex "multiscale_xma=outputs=4: \
# out_1_width=1280: out_1_height=720: out_1_rate=full:   \
# out_2_width=848:  out_2_height=480: out_2_rate=half:   \ 
# out_3_width=640:  out_3_height=360: out_3_rate=half:   \
# out_4_width=288:  out_4_height=160: out_4_rate=half    \
# [a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]; \
# [0:1]asplit=outputs=5[aud1][aud2][aud3][aud4][aud5]" \
# -map "[aa]"  -b:v 4M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -map "[aud1]" -f mp4 -y /tmp/xil_ll_xcode_scale_720p60.mp4 \
# -map "[abb]" -b:v 3M    -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -map "[aud2]" -f mp4 -y /tmp/xil_ll_xcode_scale_720p30.mp4 \
# -map "[b]"   -b:v 2500K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -map "[aud3]" -f mp4 -y /tmp/xil_ll_xcode_scale_480p30.mp4 \
# -map "[c]"   -b:v 1250K -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -map "[aud4]" -f mp4 -y /tmp/xil_ll_xcode_scale_360p30.mp4 \
# -map "[d]"   -b:v 625K  -bf 0 -scaling-list 0 -c:v mpsoc_vcu_h264 -map "[aud5]" -f mp4 -y /tmp/xil_ll_xcode_scale_288p30.mp4
