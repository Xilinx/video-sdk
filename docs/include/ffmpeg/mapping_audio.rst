
.. _mapping-audio-streams:

**************************************
Mapping Audio Streams
**************************************

When the FFmpeg job has a single input and a single output, the audio stream of the input is automatically mapped to the output video.

When the FFmpeg job has multiple outputs, FFmpeg must be explicitely told which audio stream to map to each of the output streams. The example below implements an transcoding pipeline with an ABR ladder. The input audio stream is split into 4 different channels using the ``asplit`` filter, one for each video output. Each audio channel is then uniquely mapped to one of the output video streams using the ``-map`` option. ::

	ffmpeg -c:v mpsoc_vcu_h264 -i input.mp4
	    -filter_complex "multiscale_xma=outputs=4:
	    out_1_width=1280: out_1_height=720:  out_1_rate=full:
	    out_2_width=848:  out_2_height=480:  out_2_rate=full:
	    out_3_width=640:  out_3_height=360:  out_3_rate=full:
	    out_4_width=288:  out_4_height=160:  out_4_rate=full 
	    [vid1][vid2][vid3][vid4]; [0:1]asplit=outputs=4[aud1][aud2][aud3][aud4]" \
	    -map "[vid1]" -b:v 3M    -c:v mpsoc_vcu_h264 -map "[aud1]" -c:a aac -f mp4 -y output1.mp4
	    -map "[vid2]" -b:v 2500K -c:v mpsoc_vcu_h264 -map "[aud2]" -c:a aac -f mp4 -y output2.mp4
	    -map "[vid3]" -b:v 1250K -c:v mpsoc_vcu_h264 -map "[aud3]" -c:a aac -f mp4 -y output3.mp4
	    -map "[vid4]" -b:v 625K  -c:v mpsoc_vcu_h264 -map "[aud4]" -c:a aac -f mp4 -y output4.mp4



..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.