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


# This script takes two arguments: 
#  - The full path to a pre-encoded 4K60 YUV420 HEVC file
#  - The ID of a job slot separately allocated using the job slot reservation tool and the '14_ffmpeg_transcode_2dev_4k.json' file associated to this example

# The script uses two devices to transcode the input stream to 4K H264 and 1080p HEVC.
# The first device is used to decode the 4K60 input, scale it to 1080p60 and encode the 4K H264 output.
# The second device is used to encode the 1080p60 HEVC output.
# Both outputs are sent to /dev/null


if [ $# -ne 1 ]
  then
    echo "Incorrect arguments supplied"
    echo "Usage: $(basename $0) <4K60 HEVC clip>"
    exit 1
fi

INPUT=$1

# Determine the number of U30 devices
numDevices=$( xbutil examine | grep -c xilinx_u30_gen3x4_base_2 )
echo "Detected $numDevices devices"

# Get the full path to the directory containing the scripts
SCRIPT_DIR="$(dirname $(realpath "${BASH_SOURCE[0]}"))"

# Start the job slot reservation tool
xterm -fa mono:size=9 -e "jobSlotReservation $SCRIPT_DIR/15_ffmpeg_transcode_2dev_4k.json" &
PID_JSR=$!

# Wait a little bit before starting the FFmpeg jobs
sleep 1s

# Determine how many job slots were reserved
numSlots=$( grep -c "export XRM_RESERVE_ID" /var/tmp/xilinx/xrm_jobReservation.sh )
echo "Reserved $numSlots job slots"

# If two or more job slots were reserved, start two jobs. Otherwise start only one job. 
if [ $numSlots -ge 2 ]
  then
    echo "Starting two parallel jobs on slots #1 and #2"

    # Start a FFmpeg job using reserved job slot #1
    xterm -fa mono:size=9 -e "$SCRIPT_DIR/15_ffmpeg_transcode_2dev_4k.sh $INPUT 1; sleep 2s" &
    PID_JOB1=$!

    # Start a FFmpeg job using reserved job slot #2
    xterm -fa mono:size=9 -e "$SCRIPT_DIR/15_ffmpeg_transcode_2dev_4k.sh $INPUT 2; sleep 2s" &
    PID_JOB2=$!

    # Wait for the two FFmpeg jobs to finish
    wait $PID_JOB1 $PID_JOB2
  else
    echo "Starting one job on slot #1"

    # Start a FFmpeg job using reserved job slot #1
    xterm -fa mono:size=9 -e "$SCRIPT_DIR/15_ffmpeg_transcode_2dev_4k.sh $INPUT 1; sleep 2s" &
    PID_JOB1=$!

    # Wait for the FFmpeg job to finish
    wait $PID_JOB1
fi

# End the job slot reservation process
sleep 1s
kill $PID_JSR

echo "Done"