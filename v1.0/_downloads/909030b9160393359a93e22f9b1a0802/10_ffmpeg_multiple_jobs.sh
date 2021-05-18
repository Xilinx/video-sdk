#
# Copyright 2020 Xilinx, Inc.
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

# This script expects 3 H.264 files and will transcode them to HEVC, sending the outputs to /tmp/xil_xcode_{n}.mp4.
# The three transcodes are run in parallel in individual xterms. 
# The first job is run on device #0 and the two others jobs are run on device #1.
# After the jobs are launched, a JSON system load report is generated.

# You may edit this script to run the job on a different combination of devices (-xlnx_hwdev).
# You may skip saving the resulting videos by setting /dev/null as the output (-y).

if ! [ -x "$(command -v xterm)" ]
then
    echo "This example requires the 'xterm' program which could not be found on this system. Aborting."
    exit 1
fi

if [ $# -ne 3 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <input_h264_1_mp4> <input_h264_2_mp4> <input_h264_3_mp4>"
    exit 1
fi

# Launch the three jobs in parallel
xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 0 -c:v mpsoc_vcu_h264 -i $1 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_1.mp4" &
xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $2 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_2.mp4" &
xterm -fa mono:size=9 -hold -e "ffmpeg -xlnx_hwdev 1 -c:v mpsoc_vcu_h264 -i $3 -f mp4 -c:v mpsoc_vcu_hevc -y /tmp/xil_xcode_3.mp4" &

# Wait until the jobs are started to generate a system load report
sleep 2s
xrmadm /opt/xilinx/xrm/test/list_cmd.json &
 

