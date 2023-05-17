#!/bin/bash
#
# Copyright 2020-2022 Xilinx, Inc.
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

DUR=500
HLS_DIR="/tmp/hls"

function clean_up {
    echo "Killing $HTTP_SERVER_PID"
    kill -9 $HTTP_SERVER_PID
    (cd ${HLS_DIR} && rm -f *)
}
trap clean_up EXIT INT TERM
mkdir -p ${HLS_DIR} || exit 1
cd ${HLS_DIR}
echo "Starting basic web-server..."
python3 -m http.server 8080 2>&1 > /dev/null &
HTTP_SERVER_PID=$!

echo "Starting HLS streamer..."
ffmpeg -y -f lavfi -i "testsrc=duration=${DUR}:size=1920x1080:rate=30" \
-f lavfi -i "sine=frequency=5000:duration=${DUR}" \
-f lavfi -i "sine=frequency=4000:duration=${DUR}" \
-f lavfi -i "sine=frequency=3000:duration=${DUR}" \
-f lavfi -i "sine=frequency=2000:duration=${DUR}" \
-f lavfi -i "sine=frequency=1000:duration=${DUR}" \
-filter_complex "multiscale_xma=outputs=5: \
 out_1_width=1920: out_1_height=1080: out_1_rate=full: \
 out_2_width=1280: out_2_height=720:  out_2_rate=full: \
 out_3_width=848:  out_3_height=480:  out_3_rate=full: \
 out_4_width=640:  out_4_height=360:  out_4_rate=full: \
 out_5_width=288:  out_5_height=160:  out_5_rate=full  \
 [vid1][vid2][vid3][vid4][vid5]; [1]volume=1[aud1]; [2]volume=1[aud2]; [3]volume=1[aud3]; [4]volume=1[aud4]; [5]volume=1[aud5]" \
-map "[vid1]" -b:v:0 2M   -minrate:v:0 2M   -maxrate:v:0 2M   -bufsize:v:0 4M   -c:v:0 mpsoc_vcu_h264 \
-map "[vid2]" -b:v:1 1M   -minrate:v:1 1M   -maxrate:v:1 1M   -bufsize:v:1 1M   -c:v:1 mpsoc_vcu_h264 \
-map "[vid3]" -b:v:2 750K -minrate:v:2 750K -maxrate:v:2 750K -bufsize:v:2 750K -c:v:2 mpsoc_vcu_h264 \
-map "[vid4]" -b:v:3 375K -minrate:v:2 375K -maxrate:v:2 375K -bufsize:v:3 375K -c:v:3 mpsoc_vcu_h264 \
-map "[vid5]" -b:v:4 250k -minrate:v:4 250k -maxrate:v:4 250k -bufsize:v:4 250k -c:v:4 mpsoc_vcu_h264 \
-map "[aud1]" -c:a:0 aac \
-map "[aud2]" -c:a:1 aac \
-map "[aud3]" -c:a:2 aac \
-map "[aud4]" -c:a:3 aac \
-map "[aud5]" -c:a:4 aac \
-var_stream_map "v:0,a:0 v:1,a:1 v:2,a:2 v:3,a:3 v:4,a:4" \
-f hls \
-hls_wrap 5 \
-hls_time 6 \
-master_pl_name "test.m3u8" -hls_segment_filename  "${HLS_DIR}/test_%v-%d.ts" "${HLS_DIR}/test_%v.m3u8"



