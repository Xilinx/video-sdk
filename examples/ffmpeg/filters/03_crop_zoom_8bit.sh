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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and will crop a 300x200 sized section of the original video. The section's top left corner begins at 20 pixels to the right, and 10 pixels down from the top-left corner of the original video. The output video is encoded in 8Mbps, and is saved to /tmp/xil_crop_zoom.mp4

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./03_crop_zoom_8bit.sh <h264 clip>"
    exit 1
fi

INPUT=$1

bps=$( ffprobe -show_entries stream=bits_per_raw_sample ${INPUT} 2> /dev/null | grep bits_per_raw_sample )
if [ $bps != "bits_per_raw_sample=8" ]
  then
    echo "Error: Incorrect input format detected."
    echo "The 03_crop_zoom_8bit.sh script only supports 8bit streams."
    echo "Use 03_crop_zoom_10bit.sh for 10bit streams."
    exit 1
fi

ffmpeg -c:v mpsoc_vcu_h264 -i $INPUT -vf 'xvbm_convert, crop=640:480:(in_w-800):(in_h-out_h)/2' -c:v mpsoc_vcu_h264 -b:v 8000K -f mp4 -y /tmp/xil_crop_zoom.mp4

