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

#This file is the most recommended set of flags to produce a low latency encoded HEVC output that will look best visually

#Please provide a RAW YUV for the script; you can change the input resolution and framerate with the -s:v and -r flags, respectively

if [ $# -ne 2 ]
  then
    echo "Incorrect arguments supplied"
	 echo "Usage: ./hevc_ll_subjective.sh <1080p60 RAW clip> <bitrate in Kbps>"
    exit 1
fi

INPUT=$1
BITRATE=$2

ffmpeg -pix_fmt yuv420p -s:v 1920x1080 -r 60 -i $INPUT \
-vsync 0 \
-b:v ${BITRATE}K \
-c:v mpsoc_vcu_hevc \
-f hevc \
-bf 0 \
-g 120 \
-periodicity-idr 120 \
-qp-mode auto \
-y ${INPUT}_${BITRATE}_ll_subjective.hevc
