# Copyright (C) 2021, Xilinx Inc - All rights reserved
# Xilinx Worker-Launcher Application

# Licensed under the Apache License, Version 2.0 (the "License"). You may
# not use this file except in compliance with the License. A copy of the
# License is located at

#     http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations
# under the License.

cmake_minimum_required(VERSION 2.8.11)
include (CMakeDependentOption)
set(XMA_APPS_PROJ "xilinx-u30-xma-apps")
set(XMA_APPS_VER  "2.0.0")

find_program(UNAME_EXEC uname)
find_program(LSB_RELEASE_EXEC lsb_release)

# Grab Arch information like x86-64
execute_process(COMMAND ${UNAME_EXEC} -p
    OUTPUT_VARIABLE UNAME_ARCHTYPE
    OUTPUT_STRIP_TRAILING_WHITESPACE)
# Grab OS release short name
execute_process(COMMAND ${LSB_RELEASE_EXEC} -rs
	OUTPUT_VARIABLE LSB_RELEASE_REL_SHORT
	OUTPUT_STRIP_TRAILING_WHITESPACE)

set(XMA_APPS_PROJ_FULL_NAME "${XMA_APPS_PROJ}_${XMA_APPS_VER}_${LSB_RELEASE_REL_SHORT}-${UNAME_ARCHTYPE}")

project(${XMA_APPS_PROJ})

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/lib/pkgconfig")

# Linking variables
find_package(PkgConfig REQUIRED)
pkg_check_modules(XRT REQUIRED libxma2api)
pkg_check_modules(XVBM REQUIRED xvbm)
pkg_check_modules(LIBXRM REQUIRED libxrm)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -DDSA64 ${CMAKE_C_FLAGS}")

# Building encoder dynamic params library
add_library(u30_enc_dyn_param SHARED
	common/enc_utils/src/xlnx_enc_dyn_params.c
)

target_compile_options(u30_enc_dyn_param
	PUBLIC	${XRT_CFLAGS}
)

target_include_directories(u30_enc_dyn_param
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/enc_utils/include
)

target_link_libraries(u30_enc_dyn_param ${XRT_LDFLAGS})

# Building decoder xma app
file(GLOB DECODER_SRC "decoder/lib/src/*.c" "decoder/app/src/*.c" "common/src/*.c"
	 "common/xvbm_utils/src/*.c")

add_executable(u30_xma_decode ${DECODER_SRC})

target_include_directories(u30_xma_decode
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${LIBXRM_INCLUDE_DIRS}
	PUBLIC ${XVBM_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/decoder/lib/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/decoder/app/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/xvbm_utils/include
)

# Building scaler xma app
file(GLOB SCALER_SRC "scaler/lib/src/*.c" "scaler/app/src/*.c" "common/src/*.c")

add_executable(u30_xma_scale ${SCALER_SRC})

target_include_directories(u30_xma_scale
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${LIBXRM_INCLUDE_DIRS}
	PUBLIC ${XVBM_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/scaler/lib/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/scaler/app/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/include
)

# Building encoder xma app
file(GLOB ENCODER_SRC "encoder/lib/src/*.c" "encoder/app/src/*.c" "common/src/*.c")

add_executable(u30_xma_encode ${ENCODER_SRC})

target_include_directories(u30_xma_encode
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${LIBXRM_INCLUDE_DIRS}
	PUBLIC ${XVBM_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/encoder/lib/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/encoder/app/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/include
)

# Building transcoder xma app
file(GLOB TRANSCODER_SRC "transcoder/lib/src/*.c" "transcoder/app/src/*.c" "common/src/*.c"
	 "common/xvbm_utils/src/*.c")

add_executable(u30_xma_transcode ${TRANSCODER_SRC})

target_include_directories(u30_xma_transcode
	PUBLIC ${XRT_INCLUDE_DIRS}
	PUBLIC ${LIBXRM_INCLUDE_DIRS}
	PUBLIC ${XVBM_INCLUDE_DIRS}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/transcoder/lib/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/transcoder/app/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/include
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common/xvbm_utils/include
)

# Compile and link options for xma apps
set_property(TARGET u30_xma_decode u30_xma_scale u30_xma_encode u30_xma_transcode
    APPEND PROPERTY COMPILE_OPTIONS ${XRT_CFLAGS} ${LIBXRM_CFLAGS} ${XVBM_CFLAGS})

set_property(TARGET u30_xma_decode u30_xma_scale u30_xma_encode u30_xma_transcode
    APPEND PROPERTY LINK_LIBRARIES ${XRT_LDFLAGS} ${LIBXRM_LDFLAGS} ${XVBM_LDFLAGS} -lm)

# Set the location for xma apps and library installation
install(TARGETS u30_xma_decode DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps/examples)
install(TARGETS u30_xma_scale DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps/examples)
install(TARGETS u30_xma_encode DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps/examples)
install(TARGETS u30_xma_transcode DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps/examples)
install(TARGETS u30_enc_dyn_param DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/setup.bash DESTINATION ${CMAKE_INSTALL_PREFIX}/xma_apps)

# Packaging section
set(CPACK_PACKAGE_NAME ${XMA_APPS_PROJ})
set(CPACK_PACKAGE_VERSION ${XMA_APPS_VER})
set(CPACK_PACKAGE_FILE_NAME ${XMA_APPS_PROJ_FULL_NAME})
set(CPACK_PACKAGE_CONTACT "Suresh Avula <avula@xilinx.com>")
set(CPACK_PACKAGE_VENDOR "Xilinx")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Xilinx MPSoC XMA Apps")
set(CPACK_PACKAGE_DESCRIPTION "The Xilinx MPSoC XMA based encoder, decoder, scaler and transcoder apps.
                               A shared library that handles run time changing of encoder paramters.")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "xrt (>= 2.11), xilinx-u30-xmpsoccodecs (>= 1.1.0), xilinx-u30-xma-multiscaler (>= 1.1.0), xilinx-u30-xlookahead (>= 1.1.0), xilinx-u30-xmapropstojson (>= 1.0.1), xrm (>= 1.3), xilinx-u30-xrm-decoder (>= 1.1.0), xilinx-u30-xrm-multiscaler (>= 1.1.0), xilinx-u30-xrm-encoder (>= 1.1.0)" )
set(CPACK_RPM_PACKAGE_DEPENDS "xrt (>= 2.11), xilinx-u30-xmpsoccodecs (>= 1.1.0), xilinx-u30-xma-multiscaler (>= 1.1.0), xilinx-u30-xlookahead (>= 1.1.0), xilinx-u30-xmapropstojson (>= 1.0.1), xrm (>= 1.3), xilinx-u30-xrm-decoder (>= 1.1.0), xilinx-u30-xrm-multiscaler (>= 1.1.0), xilinx-u30-xrm-encoder (>= 1.1.0) " )
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION "/opt" "/opt/xilinx" "/usr/lib" "/usr/lib/pkgconfig")
include(CPack)

