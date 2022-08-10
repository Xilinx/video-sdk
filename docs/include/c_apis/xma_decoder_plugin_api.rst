*************************************
Decoder Plugin Reference
*************************************

Decoder Interface
======================

The external interface to the decoder plugin consists of the following XMA upper-edge functions:

- :c:func:`xma_dec_session_create()`
- :c:func:`xma_dec_session_send_data()`
- :c:func:`xma_dec_session_get_properties()`
- :c:func:`xma_dec_session_recv_frame()`
- :c:func:`xma_dec_session_destroy()`

The declaration of these functions can be found in the `xmadecoder.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmadecoder.h>`_ file. General reference information about these functions can be found in the `Decoder <https://xilinx.github.io/XRT/master/html/xmakernels.main.html#decoder>`_ section of the XMA upper-edge API Library documentation. Information specific to use with the Xilinx video codec units is provided below.

|

.. c:function:: XmaDecoderSession* xma_dec_session_create(XmaDecoderProperties *dec_props)


This function creates a decoder session and must be called prior to decoding data. The hardware resources required to run the session must be previously reserved using the :ref:`XRM APIs <xrm-api-reference>` and should not be released until after the session is destroyed. The number of sessions allowed depends on several factors that include: resolution, frame rate, bit depth, and the capabilities of the hardware accelerator.

|

.. c:function:: int32_t xma_dec_session_send_data(XmaDecoderSession *session, XmaDataBuffer *data, int32_t *data_used)

This function sends input frame data to the hardware decoder by way of the plugin. The application needs to parse the input encoded stream and send one frame of data at a time in a :c:struct:`XmaDataBuffer` data structure.

The :c:var:`data_used` value indicates the amount of input data consumed by the decoder. 

If the function returns :c:macro:`XMA_SUCCESS`, then the decoder was able to consume the entirety of the available data and :c:var:`data_used` will be set accordingly. In this case, the application can proceed with fetching decoded data using the :c:func:`xma_dec_session_recv_frame` API. 

If the function returns :c:macro:`XMA_TRY_AGAIN`, then the decoder was did not consume any of the input data and :c:var:`data_used` will be reported as 0. In this case, the application can proceed with fetching previously decoded data with the :c:func:`xma_dec_session_recv_frame` function but must send the same input again using using :c:func:`xma_dec_session_send_data` until the function returns :c:macro:`XMA_SUCCESS`.

Once the application has sent all the input frames to the decoder, it must notify the decoder by sending a null frame buffer with :c:var:`is_eos` set to 1 in :c:struct:`XmaDataBuffer` structure. The application should then continue sending null frame buffers with :c:var:`is_eos` set to 0 in order to flush out all the output YUV frames.

|

.. c:function:: int32_t xma_dec_session_get_properties(XmaDecoderSession *dec_session, XmaFrameProperties *fprops);

This function returns the decoder properties such as width, height, output format, and bits per pixel. 

|

.. c:function:: int32_t xma_dec_session_recv_frame(XmaDecoderSession *session, XmaFrame *frame)

This function tries to fetch a decoded YUV frame from the hardware accelerator. 

If the function returns :c:macro:`XMA_SUCCESS`, a valid YUV frame pointer is available in the buffer pointer of the :c:struct:`XmaFrame` argument. The decoder plugin only supports :c:struct:`XmaFrame` structures of the :c:macro:`XMA_DEVICE_BUFFER_TYPE` type which are implemented using :ref:`XVBM buffers <xvbm_reference>`. The function does not copy the output frame to the host buffer, but simply provides a pointer to the output frame containing a XVBM buffer. The application must use the XVBM APIs to read, forward or release the buffer as explained in the :ref:`XVBM library <xvbm_reference>` section. 

If the function returns :c:macro:`XMA_TRY_AGAIN`, then the decoder still needs some input data to produce a complete YUV output frame. 

If the function returns :c:macro:`XMA_EOS`, then the decoder has flushed out all the frames.

For an example of how to read and release a YUV output frame using the XVBM :c:func:`xvbm_buffer_get_host_ptr()`, :c:func:`xvbm_buffer_read()` and  :c:func:`xvbm_buffer_pool_entry_free()` APIs, refer to the :url_to_repo:`examples/xma/decoder/app/src/xlnx_decoder_app.c` file of the sample XMA decoder app.

For an example of how to receive a YUV output frame and forward it to the scaler and to the encoder plugins using the XVBM the :c:func:`xvbm_buffer_refcnt_inc()` API, refer to the :url_to_repo:`examples/xma/transcoder/lib/src/xlnx_transcoder.c` file of the sample XMA transcoder application.

|

.. c:function:: int32_t xma_dec_session_destroy(XmaDecoderSession *session)

This function destroys a decoder session that was previously created with the :c:func:`xma_dec_session_create` function.

|

Decoder Properties
======================

The Xilinx video decoder is configured using a combination of standard XMA decoder properties and custom decoder parameters, both of which are specified using a :c:struct:`XmaDecoderProperties` data structure. 

To facilitate application development, Xilinx recommends working with a simplified data structure from which the required :c:struct:`XmaDecoderProperties` can be populated using a specialized function. A reusable example of this can found in the :url_to_repo:`examples/xma/transcoder/lib/include/xlnx_transcoder_xma_props.h` and :url_to_repo:`examples/xma/transcoder/lib/src/xlnx_transcoder_xma_props.c` files of the XMA transcoder example application.

|

.. c:struct:: XmaDecoderProperties

This data structure is used to configure the Xilinx video decoder. The declaration of :c:struct:`XmaDecoderProperties` can be found in the `xmadecoder.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmadecoder.h>`_ file.

|

.. rubric:: Standard XMA Decoder Properties

When using the decoder plugin, the following members of the :c:struct:`XmaDecoderProperties` data structure must be set by the application:

hwdecoder_type
    Must be set to :c:macro:`XMA_MULTI_DECODER_TYPE`

hwvendor_string[MAX_VENDOR_NAME]
    Vendor string used to identify specific decoder requested.
    Must be set to "MPSoC"

params
    Array of custom initialization parameters.
    See the next section for the list of custom parameters supported by the decoder plugin.

param_cnt
    Count of custom parameters.

width
    Width in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 3840 for H264 and HEVC.
    Portrait mode is supported.

height
    Height in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 2160 for H264 and HEVC.

bits_per_pixel
    Bits per pixel for primary plane of output video.
    Must be set to 8 bits per pixel.

framerate
    Framerate data structure specifying frame rate per second.
    Valid values can range from 1 to integer max.
 
plugin_lib
    The plugin library name to which the application wants to communicate.
    The value of this property is obtained as part of XRM resource allocation.

dev_index
    The device index number on which the decoder resource has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

cu_index
    The decoder coding unit(cu) index that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

channel_id
    The channel number of the decoder that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.
  
ddr_bank_index
    Required property.
    Must be set to -1 to let the hardware determine which DDR bank should be used for this channel.


Other members of :c:struct:`XmaDecoderProperties` are not applicable to the decoder plugin and should not be used.

.. rubric:: Custom Decoder Parameters

In addition to the standard properties, the following :c:struct:`XmaParameter` custom parameters are supported by the decoder plugin:

"bitdepth"
    Bits per pixel for primary plane of output video. Valid value is 8.
    Should be set to the same value as the bits_per_pixel property.
    10-bit support will be added in the future.

"codec_type"
    Codec type.
    For H264, set "codec_type" to 0.
    For HEVC, set "codec_type" to 1.

"low_latency"
    Setting this flag to 1 reduces decoding latency when ``splitbuff_mode`` is also enabled. 
    **IMPORTANT:** This option should not be used with streams containing B frames. 
    Valid values are 0 (disabled, default) and 1 (enabled)

"splitbuff_mode"
    The split buffer mode hands-off buffers to next pipeline stage earlier. Setting both ``splitbuff_mode`` and ``low_latency`` to 1 reduces decoding latency.
    **IMPORTANT:** Enable this mode only if you can always send a complete Access Unit in one shot to the decoder.
    Valid values are 0 (disabled, default) and 1 (enabled)

"entropy_buffers_count"
    Number of internal buffers to be used.
    Valid values are 2 to 10 and default is 2 (recommended).

"zero_copy"
    When enabled, the decoder plugin returns a buffer to the outframe data instead of copying data back to host memory. 
    This is useful in transcoder use cases where the decoder output will be used by encoder/scaler that is running on same hardware.
    Currently the decoder supports only zero copy, therefore this parameter must always be set to 1.

"profile"
    Profile of the input stream.
    Supported values are Baseline, Main and High for H264. Main profile for HEVC.

"level"
    Level of the input stream.
    Supported values are from 1.0 to 5.1.

"chroma_mode"
    Chroma mode with which the input has been encoded.
    Supported mode is 420.

"scan_type"
    Scan type denotes field order.
    Currently decoder supports only progressive and should be set to 1.

"latency_logging"
    Set to 1 to enable logging of latency information to syslog.

..
  ------------
  
  © Copyright 2020-2022 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.w