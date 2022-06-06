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

#include "xlnx_enc_dyn_params.h"

/**
 * check_dyn_param_value: Check if the dynamic encoder parameter is valid
 *
 * @param value: dynamic param value
 * @param key: dynamic param key
 * @param min: minimum value
 * @param max: maximum value
 * @param frame_num: Frame number in config file
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t check_dyn_param_value(int value, char* key, int min, int max,
                                     uint32_t frame_num)
{
    if(value < min || value > max) {
        xma_logmsg(
            XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
            "Invalid value=%d for key=%s at frame num %d in dynamic config "
            "file\n",
            value, key, frame_num);
        return XMA_ERROR;
    }
    return XMA_SUCCESS;
}

/**
 * trim_space: Remove space or tab in the parsed string
 *
 * @param str: Pointer to parsed string
 * @return NONE
 */
static void trim_space(char* str)
{
    int count = 0;
    for(int i = 0; str[i]; i++) {
        if(!isspace(str[i])) {
            str[count++] = str[i];
        }
    }
    str[count] = '\0';
}

/**
 * get_int_val: Converts string to integer
 *
 * @param str: Input string
 * @return Integer value
 */
static inline int32_t get_int_val(char* str)
{
    char*    end;
    uint32_t i = strtol(str, &end, 10);
    return i;
}

/**
 * parse_dyn_param_value: Extracts key value pair and stores in dynamic
 * encoder parameter structure
 *
 * @param str: Pointer to a single parsed key-value pair string
 * @param dyn_param_frames: Pointer to XlnxDynParamFrames structure
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t parse_dyn_param_value(char*               str,
                                     XlnxDynParamFrames* dyn_param_frames)
{
    const char        delimiters[] = "=";
    char*             s;
    char*             key       = strtok_r(str, delimiters, &s);
    char*             s_value   = NULL;
    int               i_value   = 0;
    int               ret       = 0;
    uint32_t          frame_num = dyn_param_frames->frame_num;
    XlnxEncDynParams* enc_dyn_param =
        &dyn_param_frames->dyn_params.enc_dyn_param;
    XlnxLaDynParams* la_dyn_param = &dyn_param_frames->dyn_params.la_dyn_param;
    if(key != NULL) {
        s_value = strtok_r(NULL, delimiters, &s);
        if(s_value != NULL) {
            i_value = get_int_val(s_value);
            if(!strcmp(key, D_PARAM_B_FRAMES)) {
                if((ret = check_dyn_param_value(i_value, key, ENC_MIN_BFRAMES,
                                                ENC_MAX_BFRAMES, frame_num)) !=
                   XMA_ERROR) {
                    enc_dyn_param->is_bframes_changed =
                        ENC_RUNTIME_PARAMS_CHANGED;
                    enc_dyn_param->num_b_frames = i_value;
                }
            } else if(!strcmp(key, D_PARAM_BITRATE)) {
                if((ret = check_dyn_param_value(i_value, key, ENC_MIN_BITRATE,
                                                ENC_MAX_BITRATE, frame_num)) !=
                   XMA_ERROR) {
                    enc_dyn_param->is_bitrate_changed =
                        ENC_RUNTIME_PARAMS_CHANGED;
                    enc_dyn_param->bit_rate = i_value;
                }
            } else if(!strcmp(key, D_PARAM_T_AQ)) {
                if((ret = check_dyn_param_value(i_value, key, ENC_MIN_TAQ,
                                                ENC_MAX_TAQ, frame_num)) !=
                   XMA_ERROR) {
                    la_dyn_param->is_temporal_mode_changed =
                        ENC_RUNTIME_PARAMS_CHANGED;
                    la_dyn_param->temporal_aq_mode = i_value;
                }
            } else if(!strcmp(key, D_PARAM_S_AQ)) {
                if((ret = check_dyn_param_value(i_value, key, ENC_MIN_SAQ,
                                                ENC_MAX_SAQ, frame_num)) !=
                   XMA_ERROR) {
                    la_dyn_param->is_spatial_mode_changed =
                        ENC_RUNTIME_PARAMS_CHANGED;
                    la_dyn_param->spatial_aq_mode = i_value;
                }
            } else if(!strcmp(key, D_PARAM_S_AQ_GAIN)) {
                if((ret = check_dyn_param_value(
                        i_value, key, ENC_MIN_SPAT_AQ_GAIN,
                        ENC_MAX_SPAT_AQ_GAIN, frame_num)) != XMA_ERROR) {
                    la_dyn_param->is_spatial_gain_changed =
                        ENC_RUNTIME_PARAMS_CHANGED;
                    la_dyn_param->spatial_aq_gain = i_value;
                }
            } else {
                xma_logmsg(XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
                           "Invalid key=%s"
                           " for frame=%d in dynamic params config file \n",
                           key, frame_num);
                return XMA_ERROR;
            }
        } else {
            xma_logmsg(XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
                       "Missing value for"
                       " key=%s in dynamic params config file for frame=%d \n",
                       key, frame_num);
            return XMA_ERROR;
        }
        key = strtok(NULL, delimiters);
    }
    return ret;
}

/**
 * xlnx_enc_parse_line: Parses each line and stores dynamic encoder parameters
 *
 * @param fp: File pointer to Config file
 * @param dyn_param_frames: Pointer to XlnxDynParamFrames structure
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
static int32_t xlnx_enc_parse_dyn_params(FILE*               fp,
                                         XlnxDynParamFrames* dyn_param_frames)
{
    int        count = 0;
    char       buf[D_PARAM_MAX_LENGTH];
    char *     dp, *frame_num, *key_value_pairs, *pair, *s;
    const char delimiters_1[] = ":";
    const char delimiters_2[] = ",";
    while((dp = fgets(buf, D_PARAM_MAX_LENGTH, fp)) != NULL) {
        if(buf[0] == '\n' || buf[0] == '#' || buf[0] == '\r') {
            continue;
        }
        trim_space(&buf[0]);
        frame_num                             = strtok(buf, delimiters_1);
        (&dyn_param_frames[count])->frame_num = get_int_val(frame_num);
        key_value_pairs                       = strtok(NULL, delimiters_1);
        pair = strtok_r(key_value_pairs, delimiters_2, &s);
        while(pair != NULL) {
            if(parse_dyn_param_value(pair, &dyn_param_frames[count]) !=
               XMA_SUCCESS) {
                return XMA_ERROR;
            }
            pair = strtok_r(NULL, delimiters_2, &s);
        }
        count++;
    }
    return XMA_SUCCESS;
}

/**
 * xlnx_enc_count_lines: Count the number of lines in a config text file
 *
 * @param fp: Config file pointer
 * @return Number of lines in the file
 */
static uint32_t xlnx_enc_count_lines(FILE* fp)
{
    uint32_t count = 0;
    char     buf[D_PARAM_MAX_LENGTH];
    char*    dp;
    while((dp = fgets(buf, sizeof(buf), fp)) != NULL) {
        if(buf[0] == '\n' || buf[0] == '#' || buf[0] == '\r') {
            continue;
        }
        count++;
    }
    return count;
}

/**
 * xlnx_enc_get_dyn_params: Gets dynamic encoder parameters
 *
 * @param dyn_params_config_file: Pointer to input dynamic params config file
 * @param dynamic_frames_count: Number of frames having dynamic parameters
 * @return DynparamsHandle: Dynamic params handle
 */
DynparamsHandle xlnx_enc_get_dyn_params(char*     dyn_params_config_file,
                                        uint32_t* dynamic_frames_count)
{

    FILE*               fp_config;
    uint32_t            count            = 0;
    XlnxDynParamFrames* dyn_param_frames = NULL;

    fp_config = fopen(dyn_params_config_file, "rb");
    if(!fp_config) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
                   "Unable to open"
                   " config file %s for reading\n",
                   dyn_params_config_file);
        return NULL;
    }

    count                 = xlnx_enc_count_lines(fp_config);
    *dynamic_frames_count = count;
    fseek(fp_config, 0, SEEK_SET);
    if(count) {
        dyn_param_frames =
            (XlnxDynParamFrames*)calloc(count, sizeof(XlnxDynParamFrames));
        if(NULL == dyn_param_frames) {
            xma_logmsg(
                XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
                "Failed to "
                "allocate memory while parsing dynamic encoder parameters\n");
            return NULL;
        }

        if(xlnx_enc_parse_dyn_params(fp_config, dyn_param_frames) !=
           XMA_SUCCESS) {
            return NULL;
        }
    }

    fclose(fp_config);

    return (DynparamsHandle)dyn_param_frames;
}

/**
 * xlnx_enc_add_dyn_params: Adds dynamic params as side data to lookahead input
 * frame
 *
 * @param dyn_param_frames: Pointer to dynamic parameters structure
 * @param la_input_xframe: Pointer to XmaFrame to add side data
 * @param index: Index of the dynamic params structure
 * @return XMA_APP_SUCCESS or XMA_APP_ERROR
 */
int32_t xlnx_enc_add_dyn_params(DynparamsHandle dynamic_params_handle,
                                XmaFrame* la_input_xframe, uint32_t index)
{
    XlnxDynParamFrames* dynamic_params_frames =
        (XlnxDynParamFrames*)dynamic_params_handle;
    dynamic_params_frames += index;
    XmaSideDataHandle sd = NULL;

    XlnxDynParams* dyn_params = &dynamic_params_frames->dyn_params;
    sd = xma_side_data_alloc(dyn_params, XMA_FRAME_DYNAMIC_PARAMS,
                             sizeof(XlnxDynParams), 0);
    if(sd == NULL) {
        xma_logmsg(XMA_ERROR_LOG, XLNX_DYN_PARAMS_PARSER,
                   "Failed allocating "
                   "side data in encoder input frame\n");
        return XMA_ERROR;
    }

    xma_frame_add_side_data(la_input_xframe, sd);
    xma_side_data_dec_ref(sd);
    return XMA_SUCCESS;
}

/**
 * xlnx_enc_get_dyn_param_frame_num: Returns the frame number at which run-time
 *
 * @param dynamic_params_handle: Dynamic params handle
 * @param index: Index of the dynamic params structure
 * @return Frame number
 */
uint32_t xlnx_enc_get_dyn_param_frame_num(DynparamsHandle dynamic_params_handle,
                                          uint32_t        index)
{
    XlnxDynParamFrames* dynamic_params_frames =
        (XlnxDynParamFrames*)dynamic_params_handle;
    dynamic_params_frames += index;
    return dynamic_params_frames->frame_num;
}

/**
 * xlnx_enc_get_runtime_b_frames: Returns number of B frames to be set at
 * runtime
 *
 * @param dynamic_params_handle: Dynamic params handle
 * @param index: Index of the dynamic params structure
 * @return Number of B frames
 */
uint32_t xlnx_enc_get_runtime_b_frames(DynparamsHandle dynamic_params_handle,
                                       uint32_t        index)
{
    XlnxDynParamFrames* dynamic_params_frames =
        (XlnxDynParamFrames*)dynamic_params_handle;
    dynamic_params_frames += index;
    XlnxDynParams* dyn_params = &dynamic_params_frames->dyn_params;

    return dyn_params->enc_dyn_param.num_b_frames;
}

/**
 * xlnx_enc_reset_runtime_aq_params: Resets the runtime aq params changes when
 * tune-metrics is turned on
 *
 * @param dynamic_params_handle: Dynamic params handle
 * @param index: Index of the dynamic params structure
 * @return None
 */
void xlnx_enc_reset_runtime_aq_params(DynparamsHandle dynamic_params_handle,
                                      uint32_t        index)
{
    XlnxDynParamFrames* dynamic_params_frames =
        (XlnxDynParamFrames*)dynamic_params_handle;
    dynamic_params_frames += index;
    XlnxDynParams* dyn_params = &dynamic_params_frames->dyn_params;
    dyn_params->la_dyn_param.temporal_aq_mode = 0;
    dyn_params->la_dyn_param.spatial_aq_mode  = 0;
    dyn_params->la_dyn_param.spatial_aq_gain  = 0;

    return;
}

/**
 * xlnx_enc_init_dyn_params_obj: Gets all the dynamic param function pointers
 *
 * @param dyn_params_obj: Pointer to structure of dynamic params function
 * pointers
 * @return None
 */
void xlnx_enc_init_dyn_params_obj(XlnxDynParamsObj* dyn_params_obj)
{
    dyn_params_obj->xlnx_enc_get_dyn_params = xlnx_enc_get_dyn_params;
    dyn_params_obj->xlnx_enc_get_dyn_param_frame_num =
        xlnx_enc_get_dyn_param_frame_num;
    dyn_params_obj->xlnx_enc_get_runtime_b_frames =
        xlnx_enc_get_runtime_b_frames;
    dyn_params_obj->xlnx_enc_reset_runtime_aq_params =
        xlnx_enc_reset_runtime_aq_params;
    dyn_params_obj->xlnx_enc_add_dyn_params    = xlnx_enc_add_dyn_params;
    dyn_params_obj->xlnx_enc_deinit_dyn_params = xlnx_enc_deinit_dyn_params;

    return;
}

/**
 * xlnx_enc_deinit_dyn_params: Releases all the dynamic params library resources
 *
 * @param dynamic_params_handle: Dynamic params handle
 * @return None
 */
void xlnx_enc_deinit_dyn_params(DynparamsHandle dynamic_params_handle)
{
    XlnxDynParamFrames* dynamic_params_frames =
        (XlnxDynParamFrames*)dynamic_params_handle;
    if(dynamic_params_frames) {
        free(dynamic_params_frames);
    }

    return;
}
