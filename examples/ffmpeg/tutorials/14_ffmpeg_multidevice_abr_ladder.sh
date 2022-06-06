#!/bin/bash
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


# The U30 Video SDK solution supports multi device usecases. For 4k streams with bitrates
# significantly higher than the ones typically used for live streaming, it may not be possible
# to sustain real-time performance.

# This script accepts a pre-encoded 4K60 file and runs an ABR ladder across two devices.
# The first device is used to decode the input, encode it to 4K60 HEVC and scale it to 1080p60. 
# The scaled 1080p60 output is sent to the second device, where it goes through an ABR ladder 
# and is scaled and encoded into multiple resolutions. Scaling the 4K60 input  to 1080p60 on 
# device 0 reduces the size of the buffer which needs to be transferred from device 0 to device 1, 
# which is better for overall performance.

# The -lxlnx_hwdev option is used to specify on which device each specific operation must be executed.
# The xvbm_convert filter is used to transfer frame buffers from device 0 back to the host.

# The 4K60 input is scaled down to the following resolutions, framerates, and bitrates (respectively):
#  - Device 0:    4K60    16 Mbps
#  - Device 1: 1080p60     6 Mbps
#  - Device 1:  720p60     4 Mbps
#  - Device 1:  720p30     3 Mbps
#  - Device 1:  480p30  2500 Kbps
#  - Device 1:  360p30  1250 Kbps
#  - Device 1:  160p30   625 Kbps

# You may change the target codecs by switching -c:v mpsoc_vcu_hevc to -c:v mpsoc_vcu_h264.

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <4K 60 fps HEVC clip>"
    exit 1
fi

ffmpeg -re -c:v mpsoc_vcu_hevc -lxlnx_hwdev 0 -i $1 -max_muxing_queue_size 1024 \
-filter_complex "[0]split=2[dec1][dec2]; \
                 [dec2]multiscale_xma=outputs=1:lxlnx_hwdev=0:out_1_width=1920:out_1_height=1080:out_1_rate=full[scal]; \
                 [scal]xvbm_convert[host]; [host]split=2[scl1][scl2]; \
                 [scl2]multiscale_xma=outputs=4:lxlnx_hwdev=1:out_1_width=1280:out_1_height=720:out_1_rate=full:\
                                                              out_2_width=848:out_2_height=480:out_2_rate=half:\
                                                              out_3_width=640:out_3_height=360:out_3_rate=half:\
                                                              out_4_width=288:out_4_height=160:out_4_rate=half \
                 [a][b30][c30][d30]; [a]split[a60][aa];[aa]fps=30[a30]" \
-map '[dec1]' -c:v mpsoc_vcu_hevc -b:v 16M   -max-bitrate 16M   -lxlnx_hwdev 0 -slices 4 -cores 4 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_4k.mp4 \
-map '[scl1]' -c:v mpsoc_vcu_hevc -b:v 6M    -max-bitrate 6M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_1080p60.mp4               \
-map '[a60]'  -c:v mpsoc_vcu_hevc -b:v 4M    -max-bitrate 4M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_720p60.mp4                \
-map '[a30]'  -c:v mpsoc_vcu_hevc -b:v 3M    -max-bitrate 3M    -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_720p30.mp4                \
-map '[b30]'  -c:v mpsoc_vcu_hevc -b:v 2500K -max-bitrate 2500K -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_480p30.mp4                \
-map '[c30]'  -c:v mpsoc_vcu_hevc -b:v 1250K -max-bitrate 1250K -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_360p30.mp4                \
-map '[d30]'  -c:v mpsoc_vcu_hevc -b:v 625K  -max-bitrate 625K  -lxlnx_hwdev 1 -max_interleave_delta 0 -f mp4 -y /tmp/xil_multidevice_ladder_160p30.mp4