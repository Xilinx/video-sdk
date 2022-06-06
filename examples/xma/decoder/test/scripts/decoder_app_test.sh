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

mkdir -p ./app_out/

# h264
u30_xma_decode  -i input_1080p.264 -c:v mpsoc_vcu_h264 -pix_fmt yuv420p -o ./app_out/dec_test_basic_1920x1080.yuv420p

u30_xma_decode -entropy_buf_cnt 5 -i input_1080p.264 -c:v mpsoc_vcu_h264 -pix_fmt nv12 -o ./app_out/dec_test_entropy_5_1920x1080.nv12

u30_xma_decode -splitbuff_mode 1 -i input_1080p.264 -c:v mpsoc_vcu_h264 -o ./app_out/dec_test_splitbuff_1_1920x1080.nv12

u30_xma_decode -i input_1080p_10bit.h264 -c:v mpsoc_vcu_h264 -pix_fmt yuv420p10le -o ./app_out/dec_test_basic_1920x1080.yuv420p10le

u30_xma_decode -i input_1080p_10bit.h264 -c:v mpsoc_vcu_h264 -pix_fmt xv15 -o ./app_out/dec_test_basic_1920x1080.xv15

# hevc
u30_xma_decode  -i input_1080p.265 -c:v mpsoc_vcu_hevc -pix_fmt yuv420p -o ./app_out/dec_test_basic_1920x1080.yuv420p

u30_xma_decode -entropy_buf_cnt 5 -i input_1080p.265 -c:v mpsoc_vcu_hevc -pix_fmt nv12 -o ./app_out/dec_test_entropy_5_1920x1080.nv12

u30_xma_decode -splitbuff_mode 1 -i input_1080p.265 -c:v mpsoc_vcu_hevc -o ./app_out/dec_test_splitbuff_1_1920x1080.nv12

u30_xma_decode -i input_1080p_10bit.h265 -c:v mpsoc_vcu_hevc -pix_fmt yuv420p10le -o ./app_out/dec_test_basic_1920x1080.yuv420p10le

u30_xma_decode -i input_1080p_10bit.h265 -c:v mpsoc_vcu_hevc -pix_fmt xv15 -o ./app_out/dec_test_basic_1920x1080.xv15
