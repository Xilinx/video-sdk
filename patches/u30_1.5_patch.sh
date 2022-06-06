#!/bin/sh
#
# Copyright 2022 Xilinx, Inc.
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

# Script will repair a shell check in your U30 v1.5 SDK XCDR setup.sh script
# which will, erroneously, report 'No U30 devices found' on systems
# with upraded U30 device shell packages

XSCRIPT=setup.sh
XPATH=/opt/xilinx/xcdr
TARGET=$XPATH/$XSCRIPT

echo
echo "****** Xilinx U30 SDK v1.5 Patch ******"
echo
echo "Checking current state of your ${TARGET} file..."
echo
grep_cnt=`grep -c xilinx_u30_gen3x4_base_1 $TARGET`
if [ $grep_cnt -eq 0 ]
then
    echo "Your system has either already been patched or you are running"
    echo "a version of the U30 Video SDK that is different than the one"
    echo "for which this patch is intended."
    echo
    exit 1
fi

echo "Patching ${TARGET}."
echo "Backup stored as ${TARGET}bkup"
echo

sudo sed -ibkup s/xilinx_u30_gen3x4_base_1/xilinx_u30_gen3x4_base\*/ ${TARGET}

sed_ret=$?
grep_cnt=`grep -c xilinx_u30_gen3x4_base_1 $TARGET`

if [ $sed_ret -eq 0 ] && [ $grep_cnt -eq 0 ]
then
    echo "${TARGET} successfully patched"
    exit 0
else
    echo "Error patching ${TARGET}."
    echo "Please ensure you have root privileges"
fi
exit 1
echo
