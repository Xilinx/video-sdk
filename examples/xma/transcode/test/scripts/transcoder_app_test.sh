#
# Copyright (C) 2021, Xilinx Inc - All rights reserved
# Xilinx SDAccel Media Accelerator API
# 
# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.
#

./u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
-c:v mpsoc_vcu_h264 -b:v 4000K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o app_out/out_la_test1.264 -c:v mpsoc_vcu_h264 -b:v 3000K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o app_out/out_la_test2.264 \
-c:v mpsoc_vcu_h264 -b:v 2500K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o app_out/out_la_test3.264 -c:v mpsoc_vcu_h264 -b:v 1250K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o app_out/out_la_test4.264 \
-c:v mpsoc_vcu_h264 -b:v 625K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o app_out/out_la_test5.264

./u30_xma_transcode -c:v mpsoc_vcu_hevc -i input_1080p.265 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_2_width 848 -out_2_height 480 -out_3_width 640 -out_3_height 360 -out_4_width 288 -out_4_height 160 \
-c:v mpsoc_vcu_hevc -b:v 4000K -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o app_out/out_la_test1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o app_out/out_la_test2.265 \
-c:v mpsoc_vcu_hevc -b:v 2500K -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o app_out/out_la_test3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o app_out/out_la_test4.265 \
-c:v mpsoc_vcu_hevc -b:v 625K -qp-mode 2 -lookahead-depth 16 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 75 -o app_out/out_la_test5.265

./u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_1080p.264 -multiscale_xma -num-output 1 -out_1_width 1280 -out_1_height 720 -c:v mpsoc_vcu_h264 -b:v 3000K -o app_out/out_sc_only.264

./u30_xma_transcode -c:v mpsoc_vcu_hevc -i input_1080p.265 -multiscale_xma -num-output 4 -out_1_width 1280 -out_1_height 720 -out_1_rate full -out_2_width 1024 -out_2_height 576 -out_2_rate half -out_3_width 960 -out_3_height 540 -out_3_rate half -out_4_width 852 -out_4_height 480 -out_4_rate half \
-c:v mpsoc_vcu_hevc -b:v 4000K -o app_out/out_mr_sconly1.265 -c:v mpsoc_vcu_hevc -b:v 3000K -o app_out/out_mr_sconly2.265 -c:v mpsoc_vcu_hevc -b:v 2500K -o app_out/out_mr_sconly3.265 -c:v mpsoc_vcu_hevc -b:v 1250K -o app_out/out_mr_sconly4.265

./u30_xma_transcode -c:v mpsoc_vcu_h264 -i input_2160sp.264 -c:v mpsoc_vcu_h264 -b:v 8000K  -o app_out/out2160_enc_only.264

