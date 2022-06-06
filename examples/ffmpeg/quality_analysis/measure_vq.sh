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


# This scripts calculates and displays PSNR, SSIM, MS-SSIM and VMAF metrics for each frame in the input clip.

if [ $# -le 5 ]
  then
    echo "Incorrect number of arguments supplied"
    echo "Usage:   ./measure_vq.sh <Distorted Encoded Clip> <Resolution ('W'x'H')> <Framerate> <Master YUV> <Path to custom FFmpeg> <VMAF model>"
    echo "Example: ./measure_vq.sh u30_3mpbs_clip.mp4 1920x1080 60 original_clip.yuv /home/user/ffmpeg-static/ vmaf_v0.6.1.pkl"
    exit 1
fi


DISTORTED=$1
RESOLUTION=$2
FRAMERATE=$3
MASTER=$4
FFMPEG_PATH=$5
MODEL=$6


filename="${DISTORTED##*/}"

${FFMPEG_PATH}/ffmpeg -i $DISTORTED -framerate $FRAMERATE -s $RESOLUTION -pix_fmt yuv420p -i $MASTER -lavfi libvmaf="log_fmt=json:ms_ssim=1:ssim=1:psnr=1:log_path=/tmp/${filename}.vmaf.json:model_path=${FFMPEG_PATH}/model/${MODEL}" -f null -
