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

#This file will accept 2 files, a master clip, and an encoded clip. It will calculate PSNR, SSIM, MS-SSIM, VMAF and output the data to a logfile.

#The only variable in this script you should edit is the $MODEL, which is described on the GitHub

if [ $# -le 4 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: ./measure_vq.sh <Distorted Encoded Clip> <Resolution ('W'x'H')> <Framerate> <Master YUV> [Custom FFmpeg Path]"
    echo "Example: ./measure_vq.sh u30_3mpbs_clip.mp4 1920x1080 60 original_clip.yuv /home/user/ffmpeg-static/"
    exit 1
fi


DISTORTED=$1
RESOLUTION=$2
FRAMERATE=$3
MASTER=$4
FFMPEG_PATH=$5


####################
#TODO parse 4k vs 1080 for pkl model until then hardcode

#4k
#MODEL=

#1080p
MODEL=vmaf_v0.6.1.pkl
###################

filename="${DISTORTED##*/}"

${FFMPEG_PATH}/ffmpeg -i $DISTORTED -framerate $FRAMERATE -s $RESOLUTION -pix_fmt yuv420p -i $MASTER -lavfi libvmaf="log_fmt=json:ms_ssim=1:ssim=1:psnr=1:log_path=/tmp/${filename}.vmaf.json:model_path=${FFMPEG_PATH}/model/${MODEL}" -f null -
