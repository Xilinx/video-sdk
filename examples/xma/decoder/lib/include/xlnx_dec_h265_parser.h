/*
 * Copyright (C) 2021, Xilinx Inc - All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
#ifndef _H265_PARSER_H_
#define _H265_PARSER_H_

#include "xlnx_dec_common_parser.h"

/**
 * find_next_hevc_au: searches for the end of the next access unit
 * @param file: file to read from if more data is needed
 * @param buffer: buffer containing data to parse
 * @param parse_data: structure containing latest sps
 * @param ret_offset: returns the offset of the end of the next access unit
 * @return XMA_APP_SUCCESS on success, XMA_APP_EOF if the end of the file is
 * reached, otherwise RET_ERROR
 */
int xlnx_dec_parse_next_h265_au(int file, XlnxDecBuffer* buffer,
                                XlnxDecFrameParseData* parsedata,
                                int*                   retoffset);

#endif
