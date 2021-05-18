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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and will send the encoded h.264 output in a set of HLS streams located at /var/www/html/xil_xcode_stream_scale_*.m3u8

# You may edit this to enable other output bitrates (-b:v).

# The 1080p60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):
# 720p60 4.0   Mbps
# 720p30 3.0   Mbps
# 848p30 2.5   Mbps
# 360p30 1.25  Mbps
# 288p30 0.625 Mbps

# Most webservers host their "accessible locations" from /var/www/html/. This will not exist without a webserver (e.g. Apache2) installed. Please ensure one is installed and the directory is writable; otherwise this script will fail.

# You may change the target codecs to HEVC by changing:
#     the first  instance of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for decoding from HEVC
#     the second instance of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding into HEVC

# Stream settings may be changed to accomodate different needs:
# "-hls_time 4"
#	Sets the segment length to 4 seconds

# "-hls_list_size 5"
#	Sets the number of 4-second segments to 5 to be saved in the playlist/manifest

# "-hls_flags delete_segments"
#	Deletes segments after reaching the specificed list size (in this case 5)

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <1080p60 h264 clip>"
    exit 1
fi
ffmpeg -c:v mpsoc_vcu_h264 -i  $1 \
-filter_complex "multiscale_xma=outputs=4: \  
out_1_width=1280: out_1_height=720:  out_1_rate=full: \
out_2_width=848:  out_2_height=480:  out_2_rate=half: \
out_3_width=640:  out_3_height=360:  out_3_rate=half: \
out_4_width=288:  out_4_height=160:  out_4_rate=half  \
[a][b][c][d]; [a]split[aa][ab]; [ab]fps=30[abb]" \
-map "[aa]"  -b:v 4M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p60.m3u8 \
-map "[abb]" -b:v 3M    -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_720p30.m3u8 \
-map "[b]"   -b:v 2500K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_480p30.m3u8 \
-map "[c]"   -b:v 1250K -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_360p30.m3u8 \
-map "[d]"   -b:v 625K  -c:v mpsoc_vcu_h264 -f hls -hls_time 4 -hls_list_size 5 -hls_flags delete_segments -y /var/www/html/xil_xcode_stream_scale_288p30.m3u8
