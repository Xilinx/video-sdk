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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and will send the rotated, encoded h.264 output to /tmp/xil_rotate_<N>.mp4 at a rate of 8Mbps.


# Direction flag enumeration:
# direction = 0 -> Rotate by 90 degrees counterclockwise and vertically flip (default)
# direction = 1 -> Rotate by 90 degrees clockwise
# direction = 2 -> Rotate by 90 degrees counterclockwise
# direction = 3 -> Rotate by 90 degrees clockwise and vertically flip


# Passthrough flag enumeration:
# none
#	Always apply transposition.
#
# portrait
#	Preserve portrait geometry (when height >= width).
#
# landscape 
#	Preserve landscape geometry (when width >= height).

if [ $# -ne 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./01_rotate_video.sh <1080p60 h264 clip> <direction {0..3}>"
    exit 1
fi

INPUT=$1
DIRECTION=$2

ffmpeg -c:v mpsoc_vcu_h264 -i $INPUT -vf "xvbm_convert,transpose=dir=${DIRECTION}:passthrough=none" -c:v mpsoc_vcu_h264 -b:v 8000K -r 60 -f mp4 -y /tmp/xil_rotate_${DIRECTION}.mp4

