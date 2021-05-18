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

# This script accepts 4x 8-bit, YUV420, pre-encoded h264 files of equal dimensions, and will create an output 2x2 composite video which will be an encoded h.264 output saved to /tmp/xil_2x2_composite.mp4 at a rate of 8Mbps. The output resolution will be equal to the original input.


if [ $# -ne 4 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./04_multi_comp.sh <h264 clip> <h264 clip> <h264 clip> <h264 clip>"
    exit 1
fi

TOPLEFT=$1
TOPRIGHT=$2
BOTLEFT=$3
BOTRIGHT=$4

ffmpeg -c:v mpsoc_vcu_h264 -i $TOPLEFT -c:v mpsoc_vcu_h264 -i $TOPRIGHT -c:v mpsoc_vcu_h264 -i $BOTLEFT -c:v mpsoc_vcu_h264 -i $BOTRIGHT -filter_complex "\
	[0]xvbm_convert[TL],\
	[1]xvbm_convert[TR],\
	[2]xvbm_convert[BL],\
	[3]xvbm_convert[BR],\
	[TL]scale=iw/2:ih/2[TLdiv2],\
	[TR]scale=iw/2:ih/2[TRdiv2],\
	[BL]scale=iw/2:ih/2[BLdiv2],\
	[BR]scale=iw/2:ih/2[BRdiv2],\
	[TLdiv2][TRdiv2]hstack[top],\
	[BLdiv2][BRdiv2]hstack[bot],\
	[top][bot]vstack" \
	-c:v mpsoc_vcu_h264 -b:v 8000K -f mp4 -y -frames 1000 /tmp/xil_multi_comp.mp4
