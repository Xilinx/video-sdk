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


# This script accepts an 10-bit, YUV420, pre-encoded h264 file and an image file ("logo"). It will scale the logo to 500x100, place it 16 pixels right and 16 pixels down from the top-left corner of the output video file, which will be an encoded h.264 output saved to /tmp/xil_logo_overlay.mp4 at a rate of 8Mbps.

# The GitHub example will provide the Xilinx Logo in the main directory, "xilinx_logo.png".

if [ $# -ne 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./02_logo_overlay_10bit.sh <h264 clip> <Logo.png>"
    exit 1
fi

INPUT=$1
LOGO=$2

bps=$( ffprobe -show_entries stream=bits_per_raw_sample ${INPUT} 2> /dev/null | grep bits_per_raw_sample )
if [ $bps != "bits_per_raw_sample=10" ]
  then
    echo "Error: Incorrect input format detected."
    echo "The 02_logo_overlay_10bit.sh script only supports 10bit streams."
    echo "Use 02_logo_overlay_8bit.sh for 8bit streams."
    exit 1
fi

ffmpeg -c:v mpsoc_vcu_h264 -i $INPUT -i $LOGO -filter_complex '[0]xvbm_convert, format=pix_fmts=yuv420p10le[vid], [1]scale=500:100[logo], [vid][logo]overlay=16:16:format=yuv420p10' -c:v mpsoc_vcu_h264 -b:v 8000K -f mp4 -y /tmp/xil_logo_overlay.mp4
