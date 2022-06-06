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

# This script accepts an 8-bit, YUV420, pre-encoded h264 file and will send a decoded output to /tmp/xil_dec_out.yuv

# You may change the target codec to HEVC by changing:
#     the instance of (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for decoding from HEVC

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <h264 clip>"
    exit 1
fi
ffmpeg -c:v mpsoc_vcu_h264 -i  $1 \
-vf xvbm_convert -pix_fmt yuv420p -y /tmp/xil_dec_out.yuv
