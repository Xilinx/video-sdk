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

# **************** H.264 Encoder Testcases *********************

mkdir -p ./app_out/

u30_xma_enc -w 1920 -h 1080 -pix_fmt yuv420p -i input_1920x1080.yuv420p -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000 -profile 100 -level 42 -g 120 -periodicity-idr 120 -qp-mode 2 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o ./app_out/enc_test_4.264

u30_xma_enc -w 3840 -h 2160  -i input_3840x2160.nv12 -c:v mpsoc_vcu_h264 -b:v 10000 -o ./app_out/enc_test_2160sp.264

u30_xma_enc -w 1920 -h 1080  -i input_1920x1080.nv12 -c:v mpsoc_vcu_h264 -b:v 5000 -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o ./app_out/enc_test_la19.264

u30_xma_enc -w 1920 -h 1080 -pix_fmt yuv420p10le -i input_1920x1080.yuv420p10le -c:v mpsoc_vcu_h264 -o ./app_out/enc_test_1080p_10bit.264

# ********************** HEVC Encoder Testcases *************************************
u30_xma_enc -w 1920 -h 1080 -pix_fmt yuv420p -i input_1920x1080.yuv420p -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000 -g 120 -periodicity-idr 120 -qp-mode 2 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o ./app_out/enc_test_4.265

u30_xma_enc -w 3840 -h 2160  -i input_3840x2160.nv12 -c:v mpsoc_vcu_hevc -b:v 10000 -o ./app_out/enc_test_2160sp.265

u30_xma_enc -w 1920 -h 1080  -i input_1920x1080.nv12 -c:v mpsoc_vcu_hevc -b:v 5000 -qp-mode 2 -lookahead-depth 8 -temporal-aq 0 -spatial-aq 1 -spatial-aq-gain 100 -o ./app_out/enc_test_la8.265

u30_xma_enc -w 1920 -h 1080  -i input_1920x1080.nv12 -c:v mpsoc_vcu_hevc -b:v 5000 -qp-mode 2 -lookahead-depth 8 -temporal-aq 0 -spatial-aq 1 -spatial-aq-gain 100 -o ./app_out/enc_test_la8.265

u30_xma_enc -w 1920 -h 1080 -pix_fmt yuv420p10le -i input_1920x1080.yuv420p10le -c:v mpsoc_vcu_hevc -o ./app_out/enc_test_1080p_10bit.265
