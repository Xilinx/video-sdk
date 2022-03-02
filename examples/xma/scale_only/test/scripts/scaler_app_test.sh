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

# 1080p to 720
./u30_xma_scale -w 1920 -h 1080 -i $1 -w 1280 -h 720 -o /dev/null

# ABR Ladder
./u30_xma_scale -w 1920 -h 1080 -i $1 \
-w 1280 -h  720 -o /dev/null \
-w  848 -h  480 -o /dev/null \
-w  640 -h  360 -o /dev/null \
-w  288 -h  160 -o /dev/null 

# Mixrate 
./u30_xma_scale -w 1920 -h 1080 -i $1 \
-enable-mixrate \
-w 1280 -h  720 -o /dev/null \
-w  848 -h  480 -o /dev/null \
-w  640 -h  360 -o /dev/null \
-w  288 -h  160 -o /dev/null 
