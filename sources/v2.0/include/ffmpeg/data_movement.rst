.. _ffmpeg-data-movement:

**************************************
Moving Data through the Video Pipeline
**************************************

Automatic Data Movement
=======================

The |SDK| takes care of moving data efficiently through the FFmpeg pipeline in these situations:

Individual operations:

- Decoder input: encoded video is automatically sent from the host to the device
- Scaler input: raw video is automatically sent from the host to the device
- Encoder input: raw video is automatically sent from the host to the device
- Encoder output: encoded video is automatically sent from the device to the host

Multistage pipelines

- Pipelines with hardware accelerators only (such as transcoding with ABR ladder): the video frames remain on the device and are passed from one accelerator to the next, thereby avoiding unnecessary data movement between the host and the device
- Pipelines with software filters: when using the ouput of the decoder or the scaler with a FFmpeg software filter, the video frames are automatically copied back to the host, as long as the software filter performs frame cloning. Examples of such filters include ``fps`` and ``split``.


.. _ffmpeg-explicit-data-movement:

Explicit Data Movement
======================

It is necessary to explicitly copy video frames from the device to the host in these situations:

- Writing the output of the decoder or the scaler to file.
- Using the output of the decoder or the scaler with a FFmpeg software filter which does not perform frame cloning. 
- Performing different operations on different devices, in which case the video frames must be copied from the first device to the host and then from the host to the second device.

This is done using the :option:`xvbm_convert` filter.

**IMPORTANT**: Failing to use the :option:`xvbm_convert` filter will result in garbage data being propagated throught the processing pipeline.

.. option:: xvbm_convert

  FFmpeg filter which converts and copies a XVBM frame on the device to an AV frame on the host. The pixel format of the frame on the host depends on the pixel format of the frame on the device. If the frame on the device is 8-bit, the frame will be stored as nv12 on the host. If the frame on the device is 10-bit, the frame will be stored as xv15 on the host. The :option:`xvbm_convert` filter does not support other formats and does not support converting 8-bit frames to 10-bit frames.


Examples using the :option:`xvbm_convert` filter can be found here:

- FFmpeg tutorials :ref:`Decode Only <decode-only>` and :ref:`Decode Only Into Multiple-Resolution Outputs <decode-and-scale-only>`
- FFmpeg examples with :doc:`Software Filters </examples/ffmpeg/filters>`


..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.