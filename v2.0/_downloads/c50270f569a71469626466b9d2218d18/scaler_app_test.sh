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
# 1080p nv12 to 720 yuv420p
u30_xma_scale -w 1920 -h 1080 -pix_fmt nv12 -i input_1920x1080.nv12 -w 1280 -h 720 -pix_fmt yuv420p -o ./app_out/scale_test_1280x720.yuv420p

# ABR Ladder
u30_xma_scale -w 1920 -h 1080 -i input_1920x1080.nv12 \
-w 1280 -h  720 -o ./app_out/scale_test_ABR_1280x720.nv12 \
-w  848 -h  480 -o ./app_out/scale_test_ABR_848x480.nv12 \
-w  640 -h  360 -o ./app_out/scale_test_ABR_640x360.nv12 \
-w  288 -h  160 -o ./app_out/scale_test_ABR_288x160.nv12

# Mixrate
u30_xma_scale -w 1920 -h 1080 -i input_1920x1080.nv12 \
-w 1280 -h  720 -rate full -o ./app_out/scale_test_fullrate_1280x720.nv12 \
-w  848 -h  480 -rate full -o ./app_out/scale_test_fullrate_848x480.nv12 \
-w  640 -h  360 -rate half -o ./app_out/scale_test_halfrate_640x360.nv12 \
-w  288 -h  160 -rate half -o ./app_out/scale_test_halfrate_288x160.nv12

# Upscale
u30_xma_scale -w 1920 -h 1080 -i input_1920x1080.nv12 \
-w 3840 -h 2160 -o ./app_out/scale_test_upscale_3840x2160.nv12 \
-w 2560 -h 1440 -o ./app_out/scale_test_upscale_2560x1440.nv12 \
-w 1280 -h 720  -o ./app_out/scale_test_upscale_1280x720.nv12

# 10 bit ABR
u30_xma_scale -w 1920 -h 1080 -pix_fmt yuv420p10le -i input_1920x1080.yuv420p10le \
-w 1280 -h  720 -pix_fmt yuv420p10le -o ./app_out/scale_test_ABR_1280x720.yuv420p10le \
-w  848 -h  480 -pix_fmt yuv420p10le -o ./app_out/scale_test_ABR_848x480.yuv420p10le \
-w  640 -h  360 -pix_fmt yuv420p10le -o ./app_out/scale_test_ABR_640x360.yuv420p10le \
-w  288 -h  160 -pix_fmt yuv420p10le -o ./app_out/scale_test_ABR_288x160.yuv420p10le
