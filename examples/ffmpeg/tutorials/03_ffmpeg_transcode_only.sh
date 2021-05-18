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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and will send the encoded h.264 output to /tmp/xil_xcode.mp4 at a rate of 8Mbps.

# You may edit this to enable other output bitrates (-b:v).
# You may also save the output file by replacing /dev/null with an output path.

# You may change the target codecs to HEVC by changing:
#     the first  instance of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for decoding from HEVC
#     the second instance of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding into HEVC

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <h264 clip>"
    exit 1
fi
ffmpeg -c:v mpsoc_vcu_h264 -i  $1 \
-f mp4 -b:v 8M -c:v mpsoc_vcu_h264 -y /tmp/xil_xcode.mp4
