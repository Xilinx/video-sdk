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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and an image file ("logo"). It will scale the logo to 500x100, place it 16 pixels right and 16 pixels down from the top-left corner of the output video file, which will be an encoded h.264 output saved to /tmp/xil_logo_overlay.mp4 at a rate of 8Mbps.

# The GitHub example will provide the Xilinx Logo in the main directory, "xilinx_logo.png".

if [ $# -ne 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./02_logo_overlay.sh <h264 clip> <Logo.png>"
    exit 1
fi

INPUT=$1
LOGO=$2

ffmpeg -c:v mpsoc_vcu_h264 -i $INPUT -i $LOGO -filter_complex '[0]xvbm_convert[vid], [1]scale=500:100[logo], [vid][logo]overlay=16:16' -c:v mpsoc_vcu_h264 -b:v 8000K -f mp4 -y /tmp/xil_logo_overlay.mp4
