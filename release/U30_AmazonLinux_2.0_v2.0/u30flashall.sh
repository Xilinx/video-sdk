#!/bin/bash
#
# Copyright (C) 2021, Xilinx Inc - All rights reserved
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

# Files
temp_json_file=$(mktemp -u --suffix=.u30flash)
temp_log_file=$(mktemp -u --suffix=.u30flashlog)

# Source the environment
source /opt/xilinx/xrt/setup.sh

# 1. Discover all of the devices and write the output to a JSON file
echo "Discovering installed devices...."

# Run xbmgmt examine, creating the JSON and log file
xbmgmt examine --format json --output "${temp_json_file}" --force 2>&1 > "${temp_log_file}"

# 2. Use python to read the JSON file to find the devices of interest
python3 <<END
import json
import os

# Read in the JSON file produced earlier
with open("${temp_json_file}") as f:
  data = json.load(f)

device_count = 1
devices = data["system"]["host"]["devices"]
for device in devices:
  # Look just for U30 devices
  shell_vbnv = device["vbnv"]
  if shell_vbnv.find("_u30") == -1:
     continue
  
  # Update the U30 shell
  print("\n=====================================================")
  print("[%d]: Updating flash image(s) for device: %s" %  (device_count, device["bdf"]))
  print("=====================================================")
  cmd = "xbmgmt program --device " + device["bdf"] + " --base"
  print("%s" % cmd)
  os.system(cmd)

  device_count += 1

END

# 3. Clean up after ourselves
rm "${temp_json_file}"
rm "${temp_log_file}"
