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

# The U30 Video SDK solution supports real-time decoding and encoding of 4k streams with the following notes:
# - The U30 video pipeline is optimized for live-streaming use cases. For 4k streams with bitrates significantly higher than the ones typically used for live streaming, it may not be possible to sustain real-time performance.
# - When decoding 4k streams with a high bitrate, increasing the number of entropy buffers using the -entropy_buffers_count option can help improve performance
# - When encoding raw video to 4k, set the -s option to 3840x2160 to specify the desired resolution.
# - When encoding 4k streams to H.264, the -slices option is required to sustain real-time performance. A value of 4 is recommended. This option is not required when encoding to HEVC.
# - The lookahead feature is not supported for 4k. FFmpeg will give an error if -lookahead_depth is enabled when encoding to 4k.


# This script accepts an 8-bit, YUV420, 2160p60 RAW file and will send the encoded H.264 output to /tmp/xil_4k_enc_out.mp4 at a rate of 20Mbps.

# You may edit this to accept other sizes (-s), other framerates (-r), other output bitrates (-b:v).

# You may change the target codecs to HEVC by changing:
#     the output codec from (-c:v mpsoc_vcu_h264) to (-c:v mpsoc_vcu_hevc) for encoding to HEVC

if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <2160p60 YUV file>"
    exit 1
fi
ffmpeg -f rawvideo -s 3840x2160 -r 60 -pix_fmt yuv420p -i $1 \
  -b:v 20M -c:v mpsoc_vcu_h264 -slices 4 -f mp4 -y /tmp/xil_4k_enc_out.mp4
