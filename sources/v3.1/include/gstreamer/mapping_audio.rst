
.. _gst-mapping-audio-streams:

**************************************
Mapping Audio Streams
**************************************

This section gives examples on how to decode or transcode the input streams with both video and audio

Use MP4 container file with H264 video and MP3/AAC audio. Test both audio and video playback.Decode video using VVAS plugins and audio using open source plugins. Make sure both video and audio playback are in sync. ::

  gst-launch-1.0  filesrc location=<Input MP4 file> \
    ! qtdemux name=demux demux. \
    ! queue \
    ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! autovideosink demux. \
    ! queue \
    ! mpegaudioparse \
    ! avdec_mp3  \
    ! autoaudiosink -v

Use MP4 container file with H265 video and MP3/AAC audio. Test both audio and video playback.Decode video using VVAS plugins and audio using open source plugins. Make sure both video and audio playback are in sync. ::

   gst-launch-1.0  filesrc location=<Input MP4 file> \
    ! qtdemux name=demux demux. \
    ! queue \
    ! h265parse \
    ! vvas_xvcudec dev-idx=0 \
    ! autovideosink demux. \
    ! queue \
    ! mpegaudioparse \
    ! avdec_mp3  \
    ! autoaudiosink -v

Use MP4 container file with H264 video and MP3/AAC audio. Test both audio and video playback. Decode and scale video using VVAS plugins. Make sure to run decoder in one device and scaler on an another device. Decode the audio using opensource plugins. Make sure both video and audio playback are in sync. ::
  
   gst-launch-1.0 filesrc location=<Input 1080p MP4 file> \
    ! qtdemux name=demux demux. \
    ! queue  \
    ! h264parse \
    ! vvas_xvcudec dev-idx=0 \
    ! vvas_xabrscaler dev-idx=1 ppc=4 scale-mode=2 \
    ! video/x-raw, width=1280, height=720 \
    ! autovideosink demux. \
    ! queue \
    ! mpegaudioparse \
    ! avdec_mp3 \
    ! autoaudiosink -v

Use MP4 container file with H264 Video and AAC/MP3 audio. Transcode only video from H264 to H265 using VVAS plugins and keep audio as it is and mux them back to MP4. ::
  
  gst-launch-1.0 filesrc location=<1080p60 mp4 with h264 stream> \
   ! qtdemux name=demux qtmux name=mux \
   ! filesink location=output.mp4 demux.video_0 \
   ! queue \
   ! h264parse \
   ! vvas_xvcudec dev-idx=0 \
   ! vvas_xvcuenc dev-idx=0 target-bitrate=2000 \
   ! h265parse \
   ! video/x-h265 \
   ! mux. demux.audio_0 \
   ! queue \
   ! mpegaudioparse \
   !  mux.

When the GStreamer job has multiple outputs, It must be explicitely told which audio stream to map to each of the output streams. The example below implements an transcoding pipeline with an ABR ladder. The input audio stream is split into 4 different channels using qtdemux, one for each video output. The generated transcoded output streams will have both audio and video data. ::

  gst-launch-1.0 filesrc location=~/Akhanda.mp4 \
  ! qtdemux name=demux demux.video_0 ! queue \
  ! h264parse \
  ! vvas_xvcudec  dev-idx=0 ! queue \
  ! vvas_xabrscaler  dev-idx=0 ppc=4 scale-mode=2 avoid-output-copy=true name=sc_name sc_name.src_0 \
  ! queue \
  ! video/x-raw, width=1280, height=720 \
  ! vvas_xvcuenc name=enc_720p60 dev-idx=0  target-bitrate=3000 \
  ! h264parse \
  ! qtmux name=mux1 ! filesink location=/tmp/output1.mp4 demux.audio_0 ! tee name=tee ! queue ! mux1.audio_0 sc_name.src_1 \
  ! queue \
  ! video/x-raw, width=848, height=480 \
  ! vvas_xvcuenc name=enc_480p30 dev-idx=0  target-bitrate=2500 \
  ! h264parse \
  ! qtmux name=mux2 ! filesink location=/tmp/output2.mp4 tee. ! queue !  mux2.audio_0 sc_name.src_2 \
  ! queue \
  ! video/x-raw, width=640, height=360 \
  ! vvas_xvcuenc name=enc_360p30 dev-idx=0  target-bitrate=1250 \
  ! h264parse \
  ! qtmux name=mux3 ! filesink location=/tmp/output3.mp4 tee. ! queue ! mux3.audio_0 sc_name.src_3 \
  ! queue \
  ! video/x-raw, width=288, height=160 \
  ! vvas_xvcuenc name=enc_160p30 dev-idx=0  target-bitrate=625 \
  ! h264parse \
  ! qtmux name=mux4 ! filesink location=/tmp/output4.mp4 tee. ! queue ! mux4.audio_0


.. note::
  
  Running the GStreamer pipelines with audio demux elements will give errors if run with input streams without audio
..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
