*************************************
Encoder Plugin Reference
*************************************

Encoder Interface
======================

The external interface to the encoder plugin consists of the following XMA application-level functions:

- :c:func:`xma_enc_session_create()`
- :c:func:`xma_enc_session_send_frame()`
- :c:func:`xma_enc_session_recv_data()`
- :c:func:`xma_enc_session_destroy()`

The declaration of these functions can be found in the `xmaencoder.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmaencoder.h>`_ file. The API reference for these functions can be found in the `Encoder <https://xilinx.github.io/XRT/master/html/xmakernels.main.html#encoder>`_ section of the XMA upper-edge API Library documentation. Information specific to use with the Xilinx video codec units is provided below.

|

.. c:function:: XmaEncoderSession* xma_enc_session_create(XmaEncoderProperties *enc_props)

This function creates an encoder session and must be called prior to encoding input YUV. The hardware resources required to run the session must be previously reserved using the :ref:`XRM APIs <xrm-api-reference>` and should not be released until after the session is destroyed. The number of sessions allowed depends on several factors that include: resolution, frame rate, bit depth, and the capabilities of the hardware accelerator.

|

.. c:function:: int32_t xma_enc_session_send_frame(XmaEncoderSession *session, XmaFrame *frame)

This function sends a YUV frame to the hardware encoder by way of the plugin. 

Each time the application calls this function, it must provide valid pointer to a :c:struct:`XmaFrame` structure containing a YUV frame in semi-planar format (:c:struct:`XmaBufferRef`) and information about this frame (:c:struct:`XmaFrameProperties`).

If the function returns :c:macro:`XMA_SUCCESS`, then the application can proceed to fetch the encoded data using the :c:func:`xma_enc_session_recv_data` API. 

If the function returns :c:macro:`XMA_SEND_MORE_DATA`, then the application must send the next YUV frame before calling :c:func:`xma_enc_session_recv_data`.

Once the application has sent all the input frames to the encoder, it should notify the hardware by sending a null frame buffer and set :c:var:`is_last_frame` to 1 in the :c:struct:`XmaFrame` structure. If the API returns :c:macro:`XMA_FLUSH_AGAIN` after a null frame is sent, then the application can call :c:func:`xma_enc_session_recv_data` but must send a null frame again. Once the null frame is sent successfully, does not need to send frames anymore and can simply call :c:func:`xma_enc_session_recv_data` to flush out all the remaining output frames.

This function is not thread-safe. The :c:func:`xma_enc_session_send_frame` and :c:func:`xma_enc_session_recv_data` functions must be called in a serial manner by the application layer.

|

.. c:function:: int32_t xma_enc_session_recv_data(XmaEncoderSession *session, XmaDataBuffer *data, int32_t *data_size)

This function is called after calling the function :c:func:`xma_enc_session_send_frame`. The application must provide valid pointer to :c:struct:`XmaDataBuffer` and is the owner of it. The application is responsible for releasing this memory when done. 

If the function returns :c:macro:`XMA_SUCCESS` and if :c:var:`data_size` is greater than 0, then a valid output frame is available. The returned data (:c:var:`XmaDataBuffer.data`) is valid until the next call to the :c:func:`xma_enc_session_send_frame`, so the application must use or copy it before calling :c:func:`xma_enc_session_send_frame` again. The XMA encoder plugin is responsible for setting the fields of the :c:struct:`XmaDataBuffer` struct. That is, :c:var:`XmaDataBuffer.data` is set by the XMA plugin and does not transfer the ownership of this buffer to the application. The application must not attempt to free :c:var:`XmaDataBuffer.data`. The encoder plugin will recycle the data buffers in the next call to the :c:func:`xma_enc_session_send_frame` function.

If the function returns :c:macro:`XMA_TRY_AGAIN`, a data buffer is not ready to be returned and the length of the data buffer is set to 0. 

If the function returns :c:macro:`XMA_EOS`, the encoder has flushed all the output frames.

This function is not thread-safe. The :c:func:`xma_enc_session_send_frame` and :c:func:`xma_enc_session_recv_data` functions must be called in a serial manner by the application layer.

|

.. c:function:: int32_t xma_enc_session_destroy(XmaEncoderSession *session)

This function destroys an encoder session that was previously created with the :c:func:`xma_enc_session_create` function.

|

Encoder Properties
======================

The Xilinx video encoder is configured using a combination of standard XMA encoder properties and custom encoder parameters, both of which are specified using a :c:struct:`XmaEncoderProperties` data structure. 

To facilitate application development, Xilinx recommends working with a simplified data structure from which the required :c:struct:`XmaEncoderProperties` can be populated using a specialized function. A reusable example of this can found in the :url_to_repo:`examples/xma/transcode/include/xlnx_transcoder_xma_props.h` and :url_to_repo:`examples/xma/transcode/src/xlnx_transcoder_xma_props.c` files of the sample XMA transcoder application.

|

.. c:struct:: XmaEncoderProperties

This data structure is used to configure the Xilinx video encoder. The declaration of :c:struct:`XmaEncoderProperties` can be found in the `xmaencoder.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmaencoder.h>`_ file.

|

.. rubric:: Standard XMA Encoder Properties

When using the encoder plugin, the following members of the :c:struct:`XmaEncoderProperties` data structure must be set by the application:

hwencoder_type
    Vendor value used to identify the encoder type.
    Must be set to :c:macro:`XMA_MULTI_ENCODER_TYPE`

hwvendor_string[MAX_VENDOR_NAME]
    Vendor string used to identify hardware type.
    Must be set to "MPSoC"

format
    Input video format.
    Must be set to :c:macro:`XMA_VCU_NV12_FMT_TYPE`

bits_per_pixel
    Bits per pixel for primary plane of input video. 
    Must be set to 8 bits per pixel.

width
    Width in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 3840.
    Portrait mode is supported.

height
    Height in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 2160.

framerate
    Framerate data structure specifying frame rate per second

lookahead_depth
    The lookahead module depth to give start giving lookahead data.
    Supported values are 0 to 20.

rc_mode
    Rate control mode for custom rate control
    Supported values are 0 (custom rate control disabled) and 1 (enabled)

params
    Array of custom initialization parameters.
    See the next section for the list of custom parameters supported by the encoder plugin.

param_cnt
    Count of custom parameters.

plugin_lib
    The plugin library name to which the application wants to communicate.
    The value of this property is obtained as part of XRM resource allocation.

dev_index
    The device index number on which the encoder resource has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

cu_index
    The encoder coding unit(cu) index that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

channel_id
    The channel number of the encoder that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.
  
ddr_bank_index
    Must be set to -1 to let the hardware determine which DDR bank should be used for this channel.


Other members of :c:struct:`XmaEncoderProperties` are not applicable to the encoder plugin and should not be used.


.. rubric:: Custom Encoder Parameters

In addition to the standard properties, the following :c:struct:`XmaParameter` custom parameters are supported by the encoder plugin:

"enc_options"
    For the encoder, most of the parameters are specified using a stringified INI file which is then passed to the "enc_options" :c:struct:`XmaParameter`. Refer to the :c:func:`enc_update_props` function in the :url_to_repo:`examples/xma/transcode/src/xlnx_encoder.c` file for the parameters which are sent as a string.

"latency_logging"
    When enabled, it logs latency information to syslog.

"enable_hw_in_buf"
    This parameter notifies whether the input buffer needs to copy from host or is already present on device.
    If the yuv frame is already on device memory, set it to 1.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.