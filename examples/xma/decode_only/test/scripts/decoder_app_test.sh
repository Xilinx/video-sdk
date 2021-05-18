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

./u30_xma_decode  -i $1 -c:v $2 -o /dev/null

./u30_xma_decode -entropy_buf_cnt 5 -i $1 -c:v $2 -o /dev/null

./u30_xma_decode -splitbuff_mode 1 -i $1 -c:v $2 -o /dev/null
