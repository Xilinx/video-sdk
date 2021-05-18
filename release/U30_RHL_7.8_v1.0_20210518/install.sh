#!/bin/bash

# Copyright 2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

install_epel()
{
    EPELS_FOUND=$(find /etc/yum.repos.d/ -iname "*epel*" | wc -l)
    if [ $EPELS_FOUND -ne 0 ]; then
        return
    fi
    sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm
}

remove_package_if_installed()
{
    echo "Searching for older/existing $@"
    CNT=`rpm -qa --qf '%{NAME} %{VENDOR}\n' | grep -i xilinx | grep -c $@`
    if [ $CNT -eq 0 ]
    then
        echo "$@ not found"
        return
    fi
    echo "Found instance of $@. Removing..."
    sudo yum -y remove $@
}

set -e
install_epel
set +e

remove_package_if_installed xrm
remove_package_if_installed xvbm
remove_package_if_installed xrmu30decoder
remove_package_if_installed xrmu30scaler
remove_package_if_installed xrmu30encoder
remove_package_if_installed xmpsoccodecs
remove_package_if_installed xmultiscaler
remove_package_if_installed xlookahead
remove_package_if_installed xmapropstojson
remove_package_if_installed xffmpeg
remove_package_if_installed launcher
remove_package_if_installed jobslotreservation
remove_package_if_installed xcdr
remove_package_if_installed xilinx-sc-fw-u30
remove_package_if_installed xilinx-u30-gen3x4-base
remove_package_if_installed xilinx-u30-gen3x4-validate

sudo cp ./xocl.conf /etc/modprobe.d/xocl.conf
sudo yum -y install ./xrt_*-xrt.rpm 
sudo yum -y install ./raptor_packages/xilinx-sc-fw-u30*.rpm
sudo yum -y install ./raptor_packages/xilinx-u30-gen3x4-validate*.rpm
sudo yum -y install ./raptor_packages/xilinx-u30-gen3x4-base*.rpm
printf "\n\t====U30 Shell Packages Installed====\n"

sudo yum -y install ./xrm_*.rpm 
sudo yum -y install ./xrmU30Decoder-*-Linux.rpm 
sudo yum -y install ./xrmU30Scaler-*-Linux.rpm 
sudo yum -y install ./xrmU30Encoder-*-Linux.rpm 
sudo yum -y install ./xmpsoccodecs-*-Linux.rpm 
sudo yum -y install ./xmultiscaler-*-Linux.rpm 
sudo yum -y install ./xlookahead-*-Linux.rpm 
sudo yum -y install ./xmaPropsTOjson-*-Linux.rpm 
sudo yum -y install ./xffmpeg-*-Linux.rpm 
sudo yum -y install ./launcher-*-Linux.rpm
sudo yum -y install ./jobSlotReservation-*-Linux.rpm
sudo yum -y install ./xcdr-*-Linux.rpm
sudo yum -y install ./xvbm-*-Linux.rpm
