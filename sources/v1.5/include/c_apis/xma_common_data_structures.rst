*************************************
Common XMA Data Structures
*************************************

.. c:struct:: XmaParameter

Type-Length-Value data structure used for passing custom arguments to a plugin. The declaration of :c:struct:`XmaParameter` can be found in the `xmaparam.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmaparam.h>`_ file.


.. c:struct:: XmaFrameProperties

Data structure describing the frame dimensions for XmaFrame. The declaration of :c:struct:`XmaFrameProperties` can be found in the `xmabuffers.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmabuffers.h>`_ file.


.. c:struct:: XmaFrame

Data structure describing a raw video frame and its buffers. :c:struct:`XmaFrame` structures can be received from the decoder or sent to the encoder. They are also used as input and outputs for the scaler and the look-ahead. The declaration of :c:struct:`XmaFrame` can be found in the `xmabuffers.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmabuffers.h>`_ file.

The |SDK| plugins supports two types of frames:

- :c:macro:`XMA_HOST_BUFFER_TYPE` frames are explicitly allocated and managed by the host application. They are always copied from the host to the device and back after an operation.
- :c:macro:`XMA_DEVICE_BUFFER_TYPE` frames are automatically allocated by the plugins and are implemented using the :ref:`XVBM library <xvbm_reference>`. In multistage video pipeline, they allow for zero-copy operations where frames are passed from one hardware accelerator to the next without being copied back to the host. The frame data in the underlying XVBM buffers can be accessed by the host application using the XVBM APIs. 

The decoder plugin only supports :c:macro:`XMA_DEVICE_BUFFER_TYPE` frames and the scaler, encoder and lookahead plugins support both types of frames.


.. c:struct:: XmaDataBuffer

Data structure describing a buffer containing encoded video data. :c:struct:`XmaDataBuffer` structures can be sent to the decoder or received from the encoder. The declaration of :c:struct:`XmaDataBuffer` can be found in the `xmabuffers.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmabuffers.h>`_ file.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.