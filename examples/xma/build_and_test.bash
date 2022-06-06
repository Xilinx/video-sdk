#
# Copyright (C) 2021, Xilinx Inc - All rights reserved
# Xilinx Transcoder (xcdr)
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

DEBUG_XMA_APPS_DIR="$(dirname $(realpath "${BASH_SOURCE[0]}"))/"
if [ -f "$DEBUG_XMA_APPS_DIR/CMakeLists.txt" ]; then
    # They are running setup.sh from scratch. Build and setup environment.
    cd $DEBUG_XMA_APPS_DIR
    source ./setup.bash
    make clean
    make -j
    if [ -f "$DEBUG_XMA_APPS_DIR/smokeTest_xmaApp.py" ]; then
        printf "\nBuilt XMA apps, running test\n"
        unset XRM_RESERVE_ID
        python3 smokeTest_xmaApp.py -f 5 -t tmp -l log -p 255 -r yes
    else
        printf "\nSmoke test not found in repo! Cannot test.\n"
    fi
    cd - &>/dev/null
else
    printf "\nUnable to build XMA Apps. Not found in directory ${DEBUG_XMA_APPS_DIR}\n"
fi
