#
# Copyright 2021 Xilinx, Inc.
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


LOADED_XCLBIN_UUID=`/opt/xilinx/xrt/bin/xbutil query | grep 'Xclbin.*' -A 1 | grep -E '([0-9]|-)'`
if [ "$LOADED_XCLBIN_UUID" = "00000000-0000-0000-0000-000000000000" ]
then
    source /opt/xilinx/xcdr/setup.sh
else
    source /opt/xilinx/xrt/setup.sh
    source /opt/xilinx/xrm/setup.sh
    export LD_LIBRARY_PATH=/opt/xilinx/ffmpeg/lib:$LD_LIBRARY_PATH
    export PATH=/opt/xilinx/ffmpeg/bin:/opt/xilinx/xcdr/bin:/opt/xilinx/launcher/bin:/opt/xilinx/jobSlotReservation/bin:$PATH
fi


echo Building decoder:
cd decode_only
make clean
make

echo Building encoder:
cd ../encode_only
make clean
make

echo Building scaler:
cd ../scale_only
make clean
make

echo Building transcoder:
cd ../transcode
make clean
make

cd ../
export PATH=$PATH:$(pwd)/decode_only/build/:$(pwd)/encode_only/build/:$(pwd)/scale_only/build/:$(pwd)/transcode/build/
python smokeTest_xmaApp.py -f 5 -t tmp -l log -p 255 -r yes
