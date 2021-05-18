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

#This script accepts an 8-bit, YUV420, 1080p60 RAW file and will send the encoded h.264 output to /tmp/xil_enc_out.mp4 at a rate of 8Mbps.

#You may edit this to accept other sizes (-s), other framerates (-r), other output bitrates (-b:v).
#You may also save the output file by replacing /dev/null with an output path.

# You may change the target codecs to HEVC by changing:
#     (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding into HEVC


if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <1080p60 YUV file>"
    exit 1
fi
ffmpeg -f rawvideo -s 1920x1080 -r 60 -pix_fmt yuv420p -i $1 \
  -b:v 8M -c:v mpsoc_vcu_h264 -f mp4 -y /tmp/xil_enc_out.mp4
