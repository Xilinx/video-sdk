*************************************
Look-Ahead Plugin Reference
*************************************

Look-Ahead Interface
======================

The Look-Ahead plugin is based on the Filter XMA plugin type. The external interface to the lookahead plugin consists of the following XMA application-level functions:

- :c:func:`xma_filter_session_create()`
- :c:func:`xma_filter_session_send_frame()`
- :c:func:`xma_filter_session_recv_frame()`
- :c:func:`xma_filter_session_destroy()`

The declaration of these functions can be found in the `xmafilter.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmafilter.h>`_ file. General reference information about these functions can be found in the `Filter <https://xilinx.github.io/XRT/master/html/xmakernels.main.html#filter>`_ section of the XMA upper-edge API library documentation. Information specific to use with the Xilinx video codec units is provided below.

|

.. c:function:: XmaFilterSession* xma_filter_session_create(XmaFilterProperties *props)

This function creates filter session and must be called prior to sending yuv frame to lookahead filter. The hardware resources required to run the session must be previously reserved using the :ref:`XRM APIs <xrm-api-reference>` and should not be released until after the session is destroyed. The number of sessions allowed depends on several factors that include: resolution, frame rate, bit depth, and the capabilities of the hardware accelerator.

|

.. c:function:: int32_t xma_filter_session_send_frame(XmaFilterSession *session, XmaFrame *frame)

This function sends YUV frame to the underlying XMA plugin(lower-edge interface) and eventually to lookahead module in hardware. The application has to read one YUV frame data in semi-planar format at a time and update the details of the buffer in the :c:struct:`XmaFrame` argument.
The application can take further action depending upon the return value from this API. 

If this function returns :c:macro:`XMA_SUCCESS`, then the application can proceed to fetch lookahead side data along with the output frame. 

If the function returns :c:macro:`XMA_SEND_MORE_DATA`, then the application should proceed with sending next YUV frame. 

If this function returns :c:macro:`XMA_TRY_AGAIN`, it means the input frame has not been consumed and needs to resend the same input frame after calling receive frame.

Once the application sends all input frames to the lookahead module, it should continue sending null framea until all the frames have been flushed out from the lookahead.

|

.. c:function:: int32_t xma_filter_session_recv_frame(XmaFilterSession *session, XmaFrame *frame)

This function is called after calling the function :c:func:`xma_filter_session_send_frame`. If an output frame is not ready to be returned, this function returns :c:macro:`XMA_TRY_AGAIN`. This function returns :c:macro:`XMA_SUCCESS` if the output frame is available.

The lookahead plugin provides the output frame and the application needs to release the frame after successfully sending it to the encoder and before calling the next :c:func:`xma_filter_session_send_frame`. 

Once the lookahead flushes all the frames, it returns :c:macro:`XMA_EOS`.

|

.. c:function:: int32_t xma_filter_session_destroy(XmaFilterSession *session)

This function destroys the filter session that was previously created with the :c:func:`xma_filter_session_create` function.

|

Look-Ahead Properties
=====================

The Xilinx lookahead is configured using a combination of standard XMA filter properties, standard XMA filter input and output properties and custom lookahead parameters, all of which are specified using :c:struct:`XmaFilterProperties` and :c:struct:`XmaFilterPortProperties` data structures. 

To facilitate application development, Xilinx recommends working with a simplified data structure from which the required :c:struct:`XmaFilterProperties` and :c:struct:`XmaFilterPortProperties` can be populated using a specialized function. A reusable example of this can found in the :url_to_repo:`examples/xma/transcode/include/xlnx_transcoder_xma_props.h` and :url_to_repo:`examples/xma/transcode/src/xlnx_transcoder_xma_props.c` files of the XMA transcoder example application.

**IMPORTANT**: Xilinx recommends enabling custom rate-control when using the lookahead. This is done as follows:

* When creating the lookahead session, set the custom ``rate_control_mode`` parameter to 1 in the :c:struct:`XmaFilterProperties` 
* When creating the encoder session, set the standard ``rc_mode`` property to 1 in the :c:struct:`XmaEncoderProperties` 

|

.. c:struct:: XmaFilterProperties

This data structure is used to configure the Xilinx lookahead function. The declaration of :c:struct:`XmaFilterProperties` can be found in the `xmafilter.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmafilter.h>`_ file.


.. c:struct:: XmaFilterPortProperties

This data structure is used to configure the input and output of the lookahead. The :c:struct:`XmaFilterProperties` data structure contains one :c:struct:`XmaFilterPortProperties` for the lookahead input and one :c:struct:`XmaFilterPortProperties` for the lookahead output. The declaration of :c:struct:`XmaFilterPortProperties` can be found in the `xmafilter.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmafilter.h>`_ file.

|

.. rubric:: Standard XMA Lookahead Filter Properties

When using the lookahead plugin, the following members of the :c:struct:`XmaFilterPortProperties` data structure must be set by the application:

hwfilter_type
    Vendor value used to identify the filter type.
    Must be set to :c:macro:`XMA_2D_FILTER_TYPE`.

hwvendor_string[MAX_VENDOR_NAME]
    Vendor string used to identify specific filter requested.
    Must be set to "Xilinx" 

params
    Array of custom initialization parameters.
    See the next section for the list of custom parameters supported by the lookahead plugin.

param_cnt
    Count of custom parameters.

plugin_lib
    The plugin library name to which the application wants to communicate.
    The value of this property is obtained as part of XRM resource allocation.

dev_index
    The device index number on which the lookahead resource has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

cu_index
    The lookahead coding unit(cu) index that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

channel_id
    The channel number of the lookahead that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.
  
ddr_bank_index
    Required property.
    Must be set to -1 to let the hardware determine which DDR bank should be used for this channel.

Other members of :c:struct:`XmaFilterProperties` are not applicable to the lookahead plugin and should not be used.


.. rubric:: Standard XMA Lookahead Input Filter Properties

When configuring the lookahead input, the following members of the :c:struct:`XmaFilterPortProperties` data structure must be set by the application:

format
    Input video format.
    Must be set to :c:macro:`XMA_VCU_NV12_FMT_TYPE`.

bits_per_pixel
    Bits per pixel for primary plane of input video. 
    Must be set to 8 bits per pixel.

width
    Width in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 1920
    Portrait mode is supported.

height
    Height in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 1080.

stride
    Stride value should be width with 256 alignment.

framerate
    Framerate data structure specifying frame rate per second.

Other members of :c:struct:`XmaFilterPortProperties` are not applicable to the lookahead input and should not be used.


.. rubric:: Standard XMA Lookahead Output Filter Properties

When configuring the lookahead output, the following members of the :c:struct:`XmaFilterPortProperties` data structure must be set by the application:

format
    Input video format.
    Must be set to :c:macro:`XMA_VCU_NV12_FMT_TYPE`.

bits_per_pixel
    Bits per pixel for primary plane of input video. 
    Supported is 8 bits per pixel.

width
    Output width in pixels for output video frame.
    The value should be input width aligned by 64 and shift right by 4

height
    Output height in pixels for output video frame.
    The value should be input height aligned by 64 and shift right by 4

framerate
    Framerate data structure specifying frame rate per second.

Other members of :c:struct:`XmaFilterPortProperties` are not applicable to the lookahead output and should not be used.



.. rubric:: Custom Lookahead Parameters

In addition to the standard properties, the following :c:struct:`XmaParameter` custom parameters are supported by the lookahead plugin:

"ip"
    Intra period for the video stream.

"lookahead_depth"
    Lookahead depth for the module.
    Value range from 0 to 20.

"enable_hw_in_buf"
    This param notifies whether the input buffer needs to copy from host or is already present on device.
    Set it to 1, if the yuv frame is already on device memory.

"spatial_aq_mode"
    Enable/Disable spatial aq mode.

"temporal_aq_mode"
    Enable/Disable temporal aq mode.

"rate_control_mode"
    Enable/Disable custom rate control mode.

"spatial_aq_gain"
    Spatial aq gain ranges between 0 to 100, default is 50 

"num_b_frames"
    Number of B frames in a sub GOP. 
    Value range from 0 to Integer max.

"codec_type"
    For H264 encoder, set codec type as 0 and for HEVC encoder, set it as 1.

"latency_logging"
    Set to 1 to enable logging of latency information to syslog. Set to 0 to disable logging.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.