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

# Best objective score, low latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000K -profile 100 -level 42 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 0 -o out_1.264

# Best objective score, normal latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000K -profile 100 -level 42 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 2 -lookahead-depth 20 -o out_2.264

# Best visual score, low latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000K -profile 100 -level 42 -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out_3.264

# Best visual score, normal latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -fps 60 -b:v 5000K -profile 100 -level 42 -g 120 -periodicity-idr 120 -qp-mode 2 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o out_4.264

# Resolution, Bit-rate test cases
./u30_xma_enc -w 64 -h 64  -i input_64x64.yuv -c:v mpsoc_vcu_h264 -b:v 200K -o out_64x64.264
./u30_xma_enc -w 128 -h 96  -i input_sqcif.yuv -c:v mpsoc_vcu_h264 -b:v 300K -o out_sqcif.264
./u30_xma_enc -w 176 -h 144  -i input_qcif.yuv -c:v mpsoc_vcu_h264 -b:v 400K -o out_qcif.264
./u30_xma_enc -w 352 -h 288  -i input_cif.yuv -c:v mpsoc_vcu_h264 -b:v 500K -o out_cif.264
./u30_xma_enc -w 424 -h 240  -i input_240sp.yuv -c:v mpsoc_vcu_h264 -b:v 600K -o out_240sp.264
./u30_xma_enc -w 640 -h 360  -i input_360sp.yuv -c:v mpsoc_vcu_h264 -b:v 800K -o out_360sp.264
./u30_xma_enc -w 852 -h 480  -i input_480sp.yuv -c:v mpsoc_vcu_h264 -b:v 1000K -o out_480sp.264
./u30_xma_enc -w 1280 -h 720  -i input_720sp.yuv -c:v mpsoc_vcu_h264 -b:v 2000K -o out_720sp.264
./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_h264 -b:v 5000K -o out_1080sp.264
./u30_xma_enc -w 2560 -h 1440  -i input_1440sp.yuv -c:v mpsoc_vcu_h264 -b:v 8000K -o out_1440sp.264
./u30_xma_enc -w 3840 -h 2160  -i input_2160sp.yuv -c:v mpsoc_vcu_h264 -b:v 10000K -o out_2160sp.264

# Profile, Level test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -profile 66 -level 10 -o out_p0l10.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -profile 77 -level 20 -o out_p1l20.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -profile 100 -level 30 -o out_p2l30.264

# QP test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 0 -o out_qp0.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 10 -o out_qp10.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 18 -o out_qp18.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 24 -o out_qp24.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 35 -o out_qp35.264
./u30_xma_enc -w 3840 -h 2160  -i input_2160sp.yuv -c:v mpsoc_vcu_h264 -slice-qp 42 -o out_qp42.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -slice-qp 51 -o out_qp51.264

# QP mode, aspect ratio, scaling-list test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -qp-mode 0 -o out_qpmode0.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 4000K -qp-mode 1 -o out_qpmode1.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 2000K -qp-mode 2 -o out_qpmode3.264

# RC mode test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -control-rate 0 -o out_rc0.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 4000K -control-rate 1 -o out_rc1.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -control-rate 2 -o out_rc2.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 2000K -control-rate 3 -bf 0 -o out_rc3.264

# LA test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -qp-mode 2 -lookahead-depth 5 -temporal-aq 1 -spatial-aq 0 -o out_la5.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -qp-mode 2 -lookahead-depth 8 -temporal-aq 0 -spatial-aq 1 -spatial-aq-gain 100 -o out_la8.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 5000K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o out_la19.264

# GOP, IDR, slices, num B frames, test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -g 0 -periodicity-idr 0 -slices 2 -bf 1 -o out_slices2.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -g 120 -periodicity-idr 0 -slices 8 -bf 4 -o out_slices8.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -g 0 -periodicity-idr 60 -slices 1 -bf 4 -o out_slices30.264
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_h264 -b:v 3000K -g 30 -periodicity-idr 120 -slices 10 -bf 4 -o out_slices10.264


# ********************** HEVC Encoder Testcases *************************************
# Best objective score, low latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000K -max-bitrate 4000 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 0 -o out_1.265

# Best objective score, normal latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000K -max-bitrate 4000 -g 120 -periodicity-idr 120 -qp-mode 0 -scaling-list 0 -bf 2 -lookahead-depth 20 -o out_2.265

# Best visual score, low latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000K -g 120 -periodicity-idr 120 -qp-mode 1 -bf 0 -o out_3.265

# Best visual score, normal latency
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -fps 60 -b:v 4000K -g 120 -periodicity-idr 120 -qp-mode 2 -bf 1 -lookahead-depth 20 -temporal-aq 1 -spatial-aq 1 -o out_4.265

# Resolution, Bit-rate test cases
./u30_xma_enc -w 64 -h 64  -i input_64x64.yuv -c:v mpsoc_vcu_hevc -b:v 200K -o out_64x64.265
./u30_xma_enc -w 128 -h 96  -i input_sqcif.yuv -c:v mpsoc_vcu_hevc -b:v 300K -o out_sqcif.265
./u30_xma_enc -w 176 -h 144  -i input_qcif.yuv -c:v mpsoc_vcu_hevc -b:v 400K -o out_qcif.265
./u30_xma_enc -w 352 -h 288  -i input_cif.yuv -c:v mpsoc_vcu_hevc -b:v 500K -o out_cif.265
./u30_xma_enc -w 424 -h 240  -i input_240sp.yuv -c:v mpsoc_vcu_hevc -b:v 600K -o out_240sp.265
./u30_xma_enc -w 640 -h 360  -i input_360sp.yuv -c:v mpsoc_vcu_hevc -b:v 800K -o out_360sp.265
./u30_xma_enc -w 852 -h 480  -i input_480sp.yuv -c:v mpsoc_vcu_hevc -b:v 1000K -o out_480sp.265
./u30_xma_enc -w 1280 -h 720  -i input_720sp.yuv -c:v mpsoc_vcu_hevc -b:v 2000K -o out_720sp.265
./u30_xma_enc -w 1920 -h 1080  -i input_1080sp.yuv -c:v mpsoc_vcu_hevc -b:v 5000K -o out_1080sp.265
./u30_xma_enc -w 2560 -h 1440  -i input_1440sp.yuv -c:v mpsoc_vcu_hevc -b:v 8000K -o out_1440sp.265
./u30_xma_enc -w 3840 -h 2160  -i input_2160sp.yuv -c:v mpsoc_vcu_hevc -b:v 10000K -o out_2160sp.265

# Profile, Level test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -profile 0 -level 10 -o out_p0l10.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -profile 1 -level 20 -o out_p1l20.265

# QP test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 0 -o out_qp0.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 10 -o out_qp10.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 18 -o out_qp18.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 24 -o out_qp24.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 35 -o out_qp35.265
./u30_xma_enc -w 3840 -h 2160  -i input_2160sp.yuv -c:v mpsoc_vcu_hevc -slice-qp 42 -o out_qp42.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -slice-qp 51 -o out_qp51.265

# QP mode, aspect ratio, scaling-list test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -qp-mode 0 -o out_qpmode0.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 4000K -qp-mode 1 -o out_qpmode1.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 2000K -qp-mode 2 -o out_qpmode3.265

# RC mode test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -control-rate 0 -o out_rc0.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 4000K -control-rate 1 -o out_rc1.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -control-rate 2 -o out_rc2.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 2000K -control-rate 3 -bf 0 -o out_rc3.265

# LA test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -qp-mode 2 -lookahead-depth 5 -temporal-aq 1 -spatial-aq 0 -o out_la5.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -qp-mode 2 -lookahead-depth 8 -temporal-aq 0 -spatial-aq 1 -spatial-aq-gain 100 -o out_la8.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 5000K -qp-mode 2 -lookahead-depth 19 -temporal-aq 1 -spatial-aq 1 -spatial-aq-gain 55 -o out_la19.265

# GOP, IDR, slices, num B frames, test cases
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -g 0 -periodicity-idr 0 -slices 2 -bf 1 -o out_slices2.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -g 120 -periodicity-idr 0 -slices 8 -bf 4 -o out_slices8.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -g 0 -periodicity-idr 60 -slices 1 -bf 4 -o out_slices30.265
./u30_xma_enc -w 1920 -h 1080  -i $1 -c:v mpsoc_vcu_hevc -b:v 3000K -g 30 -periodicity-idr 120 -slices 10 -bf 4 -o out_slices10.265

