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

source /opt/xilinx/xcdr/setup.sh
INSTALL_XMA_APPS_DIR=/opt/xilinx/xma_apps
DEBUG_XMA_APPS_DIR="$(dirname $(realpath "${BASH_SOURCE[0]}"))/"
if [ -f "$DEBUG_XMA_APPS_DIR/CMakeLists.txt" ]; then
    export LD_LIBRARY_PATH=$DEBUG_XMA_APPS_DIR/Debug/:$LD_LIBRARY_PATH
    export PATH=$DEBUG_XMA_APPS_DIR/Debug/:$PATH
elif [ -d "$INSTALL_XMA_APPS_DIR" ]; then
    export LD_LIBRARY_PATH=$INSTALL_XMA_APPS_DIR:$LD_LIBRARY_PATH
    export PATH=$INSTALL_XMA_APPS_DIR/examples:$PATH
fi
