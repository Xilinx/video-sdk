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

# This script takes two arguments: 
#  - The full path to a pre-encoded 4K60 YUV420 HEVC file
#  - The ID of a job slot separately allocated using the job slot reservation tool and the '14_ffmpeg_transcode_2dev_4k.json' file associated to this example

# The script uses two devices to transcode the input stream to 4K H264 and 1080p HEVC.
# The first device is used to decode the 4K60 input, scale it to 1080p60 and encode the 4K H264 output.
# The second device is used to encode the 1080p60 HEVC output.
# Both outputs are sent to /dev/null


if [ $# -ne 2 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <4K60 HEVC clip> <job slot ID>"
    exit 1
fi

source /opt/xilinx/xcdr/setup.sh
source /var/tmp/xilinx/xrm_jobReservation.sh

HEVC_INPUT=$1

var_dev_0=var_dev_$2_0
var_dev_1=var_dev_$2_1
var_res_id=XRM_RESERVE_ID_$2

DEV0=${!var_dev_0}
DEV1=${!var_dev_1}

export XRM_RESERVE_ID=${!var_res_id}

ffmpeg -re -lxlnx_hwdev $DEV0 -c:v mpsoc_vcu_hevc -i $HEVC_INPUT -max_muxing_queue_size 1024 \
       -filter_complex "[0]split[a][b];[b]multiscale_xma=outputs=1:lxlnx_hwdev=$DEV0:out_1_width=1920:out_1_height=1080:out_1_rate=full [c]; [c]xvbm_convert[d]" \
       -map '[a]' -lxlnx_hwdev $DEV0 -b:v 16M -max-bitrate 16M -level 4.2 -slices 4 -cores 4 -c:v mpsoc_vcu_h264 -max_interleave_delta 0 -f mp4 -y /tmp/xil_xcode_slot$2_2160p_h264.mp4  \
       -map '[d]' -lxlnx_hwdev $DEV1 -b:v 4M  -max-bitrate 4M                                -c:v mpsoc_vcu_hevc -max_interleave_delta 0 -f mp4 -y /tmp/xil_xcode_slot$2_1080p_hevc.mp4
