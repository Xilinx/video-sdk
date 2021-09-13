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

sw='n'

parse_parameters()
{
  while [ $# -gt 0 ]
  do
    if [ $1 = "-sw" ]
    then
      shift
      sw='y'
    else
      echo "Unknown parameter '$1'"
      exit
    fi
  done
}

install_libsdl2-dev()
{
    SYSTEM_PRETTY_NAME=$(grep '^PRETTY' /etc/os-release | sed 's/pretty_name=//Ig')
    if [ `echo "$SYSTEM_PRETTY_NAME" | { grep -c "Ubuntu" || true; }` -le 0 ]; then
        echo "This installation script is for Ubuntu systems. Your system is $SYSTEM_PRETTY_NAME"
        echo "Aborting installation."
        exit 1
    fi
    if [ "`dpkg -l | grep -i -c libsdl2-dev`" -ge 1 ]; then
        return
    fi
    UBUNTU_REPOS=$(grep -ir archive.ubuntu.com /etc/apt/sources.list* | { grep -c archive.ubuntu.com || true; })
    CODENAME=$(grep -i "ubuntu_codename=" /etc/os-release | sed "s/ubuntu_codename=//I")
    if [ "$UBUNTU_REPOS" = 0 ]; then
        echo "Default Ubuntu repositories are not found."
        echo "Do you wish to continue the installation by adding them? (y/n)"
        read ans
        if [ "$ans" != "${ans#[Yy]}" ]; then
            echo "Adding repositories"
            echo "deb http://archive.ubuntu.com/ubuntu/ $CODENAME main restricted universe multiverse"           | sudo tee -a /etc/apt/sources.list
            echo "deb http://archive.ubuntu.com/ubuntu/ $CODENAME-updates main restricted universe multiverse"   | sudo tee -a /etc/apt/sources.list
            echo "deb http://archive.ubuntu.com/ubuntu/ $CODENAME-security main restricted universe multiverse"  | sudo tee -a /etc/apt/sources.list
            echo "deb http://archive.ubuntu.com/ubuntu/ $CODENAME-backports main restricted universe multiverse" | sudo tee -a /etc/apt/sources.list
        else
            echo "Required repositories not found in /etc/apt/sources.list."
            echo "Please consult the installation guide for required dependencies"
            echo "and ensure the system is prepared before attempting a future installation."
            echo "Aborting installation."
            exit 1
        fi
    fi
    sudo apt-get -y install libsdl2-dev && RC=$? || RC=$?
    if [ "$RC" != 0 ]; then
        echo "Unable to install necessary package for installation!"
        echo "Try running the following command then restarting installation."
        echo "echo 'deb http://archive.ubuntu.com/ubuntu/ $CODENAME main restricted universe multiverse'           | sudo tee -a /etc/apt/sources.list"
        echo "Aborting installation."
        exit 1
    fi
}

remove_package_if_installed()
{
    echo "Searching for older/existing $@"
    CNT=`dpkg -l | grep -i xilinx | grep -c $@`
    if [ $CNT -eq 0 ]
    then
        echo "$@ not found"
        return
    fi
    echo "Found instance of $@. Removing..."
    sudo apt-get purge -y $@
}

parse_parameters $@

set -e
install_libsdl2-dev
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
sudo apt-get install ./xrt_*-amd64-xrt.deb -y
echo "==================================================="
echo " --- Please ignore XRT related Python or OpenCL errors ---"
echo "---------------------------------------------------"
echo "================= Installed XRT ================="

sudo apt-get install ./raptor_packages/xilinx-sc-fw-u30*.deb -y
sudo apt-get install ./raptor_packages/xilinx-u30-gen3x4-validate*.deb -y
sudo apt-get install ./raptor_packages/xilinx-u30-gen3x4-base*.deb -y
echo "================= Installed Raptor Packages ================="
sudo apt-get install ./xrm*.deb -y
echo "================= Installed XRM ================="
sudo apt-get install ./xmultiscaler-*-Linux.deb -y
sudo apt-get install ./xmpsoccodecs-*-Linux.deb -y
sudo apt-get install ./xlookahead-*-Linux.deb -y
echo "================= Installed Plugins ================="
sudo apt-get install ./xffmpeg-*-Linux.deb -y
echo "================= Installed xFFmpeg  ================="
sudo apt-get install ./xrmU30Decoder-*-Linux.deb -y
sudo apt-get install ./xrmU30Scaler-*-Linux.deb -y
sudo apt-get install ./xrmU30Encoder-*-Linux.deb -y
echo "================= Installed XRM plugins =================="
sudo apt-get install ./xmaPropsTOjson-*-Linux.deb -y
echo "================= Installed XMA props to JSON =================="
sudo apt-get install ./launcher-*-Linux.deb -y
echo "================= Installed worker/launcher =================="
sudo apt-get install ./jobSlotReservation-*-Linux.deb -y
echo "================= Installed job Slot Reservation ================="
sudo apt-get install ./xcdr-*-Linux.deb -y
echo "================= Installed  XCDR ================="
sudo apt-get install ./xvbm-*-Linux.deb -y
echo "================= Installed  XVBM ================="

if [ $sw = "y" ]
then
  sudo /opt/xilinx/xrt/bin/xclbinutil --dump-section SOFT_KERNEL[kernel_vcu_decoder]-METADATA:JSON:/tmp/dec.json --dump-section SOFT_KERNEL[kernel_vcu_encoder]-METADATA:JSON:/tmp/enc.json --input /opt/xilinx/xcdr/xclbins/transcode.xclbin
  sudo /opt/xilinx/xrt/bin/xclbinutil --remove-section BITSTREAM --remove-section SOFT_KERNEL[kernel_vcu_decoder] --remove-section SOFT_KERNEL[kernel_vcu_encoder] --input /opt/xilinx/xcdr/xclbins/transcode.xclbin --output /tmp/transcode_lite.xclbin --skip-uuid-insertion
  sudo rm -f /opt/xilinx/xcdr/xclbins/transcode.xclbin
  touch /tmp/temp
  sudo /opt/xilinx/xrt/bin/xclbinutil --add-section SOFT_KERNEL[kernel_vcu_decoder]-OBJ:RAW:/tmp/temp --add-section SOFT_KERNEL[kernel_vcu_decoder]-METADATA:JSON:/tmp/dec.json --add-section SOFT_KERNEL[kernel_vcu_encoder]-OBJ:RAW:/tmp/temp --add-section SOFT_KERNEL[kernel_vcu_encoder]-METADATA:JSON:/tmp/enc.json --input /tmp/transcode_lite.xclbin --output /opt/xilinx/xcdr/xclbins/transcode.xclbin --skip-uuid-insertion
  sudo rm -f /tmp/transcode_lite.xclbin /tmp/temp /tmp/dec.json /tmp/enc.json

  sudo rm -f /opt/xilinx/firmware/u30/gen3x4/base/test/*.xclbin
  sudo cp /opt/xilinx/xcdr/xclbins/transcode.xclbin /opt/xilinx/firmware/u30/gen3x4/base/test/
fi

