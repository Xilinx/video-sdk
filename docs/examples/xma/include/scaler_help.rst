This program ingests an nv12, yuv420p, yuv420p10le, or xv15 input
file and utilizes hardware acceleration to scale to various resolutions.

Usage:
    u30_xma_scale [options] -w <input-width> -h <input-height> -i
    <input-file> [scaler_options] -w <output-1-width> -h
    <output-1-height> -o <output-1-file> -w ...

Arguments:
    --help                     Print this message and exit
    -log <level>               Specify the log level
    -d <device-id>             Specify a device on which to run.
                                Default: 0

Input Arguments:

    -stream_loop <loop-count>  Number of times to loop the input
                                file
    -w <width>                 Specify the input's width
    -h <height>                Specify the input's height
    -pix_fmt <pixel-format>    Pixel format of the input file (nv12,
                                xv15, yuv420p10le). Default : nv12.
    -fps <frame-rate>          Frame rate. Used for scaler load
                                calculation.
    -i <input-file>            Input file to be used

Output Arguments:
    -coeff_load <load>         Specify the coefficient load. 0 Auto
                                (default), 1 static, 2 FilterCoef.txt.
    -enable_pipeline           Enable scaler pipeline. Default disabled (0)
    -rate <half/full>          Set the rate to half. Half rate drops
                                frames to reduce resource usage.
                                Default: full.
    -enable-latency-logging    Enable latency logging
    -w <width>                 Specify the output's width
    -h <height>                Specify the output's height
    -pix_fmt <pixel-format>    Pixel format of the output file (nv12,
                                xv15, yuv420p10le). Default: input fmt
    -frames <frame-count>      Number of frames to be processed.
    -o <file>                  File to which output is written.
..
  ------------

  © Copyright 2020-2022 Xilinx, Inc.

  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
