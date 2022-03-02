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

CC = gcc 
CFLAGS = $(shell pkg-config --cflags libxma2api libxma2plugin xvbm libxrm)
INCLUDE_DIR = include/
CFLAGS += -Wall -O0 -g -std=gnu99
CFLAGS += -I$(INCLUDE_DIR)
LDFLAGS = $(shell pkg-config --libs libxma2api libxma2plugin xvbm libxrm)

TARGET = u30_xma_transcode

BUILD_DIR  := build
SRC_DIR    := src
OBJ_DIR    := obj
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS :=  $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/$(OBJ_DIR)/%.o)

ifeq ($(VERSION), V2)
CFLAGS += -DU30V2
endif

.PHONY: all
all: $(BUILD_DIR)/${TARGET}

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): $(BUILD_DIR)/$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c 
	@mkdir -p $(BUILD_DIR)/$(OBJ_DIR)
	$(CC) -c $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/
