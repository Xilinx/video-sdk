*************************************
Scaler Plugin Reference
*************************************

Scaler Interface
======================

The external interface to the scaler plugin consists of the following XMA application-level functions:

- :c:func:`xma_scaler_default_filter_coeff_set()`
- :c:func:`xma_scaler_session_create()`
- :c:func:`xma_scaler_session_send_frame()`
- :c:func:`xma_scaler_session_recv_frame_list()`
- :c:func:`xma_scaler_session_destroy()`

The declaration of these functions can be found in the `xmascaler.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmascaler.h>`_ file. General reference information about these functions can be found in the `Scaler <https://xilinx.github.io/XRT/master/html/xmakernels.main.html#scaler>`_ section of the XMA upper-edge API library documentation. Information specific to use with the Xilinx video codec units is provided below.

|

.. c:function:: XmaScalerSession* xma_scaler_session_create(XmaScalerProperties *props)

This function creates scaler session and must be called prior to sending input frames. The hardware resources required to run the session must be previously reserved using the :ref:`XRM APIs <xrm-api-reference>` and should not be released until after the session is destroyed. The number of sessions allowed depends on several factors that include: resolution, frame rate, bit depth, and the capabilities of the hardware accelerator.

|

.. c:function:: int32_t xma_scaler_session_send_frame(XmaScalerSession *session, XmaFrame *frame)

This function sends a YUV frame to the underlying XMA plugin( and eventually to hardware to scale the input frame to one or multiple resolutions. The application has to read one YUV frame data in semi-planar format at a time and update the buffer details in XmaFrame argument.

The application can take further action depending upon the return value from this API. 

If the function returns :c:macro:`XMA_SUCCESS`, then the application can proceed to fetch scaled output frames. 

If the function returns :c:macro:`XMA_SEND_MORE_DATA`, then the application should proceed with sending next yuv frame.

If the function returns :c:macro:`XMA_FLUSH_AGAIN` it means that the application should keep flushing the scaler.

If the function returns :c:macro:`XMA_EOS` then the scaler has been flushed and the pipeline can be exited.

Once the application has sent all the input frames to the scaler, it must notify the scaler by sending a null frame buffer in the :c:struct:`XmaFrame` structure and flush the scaler. To flush the scaler, send a null frame buffer and call the :c:func:`xma_scaler_session_recv_frame_list()` function for as long as :c:func:`xma_scaler_session_send_frame()` returns :c:macro:`XMA_SUCCESS` or :c:macro:`XMA_FLUSH_AGAIN`.


|

.. c:function:: int32_t xma_scaler_session_recv_frame_list(XmaScalerSession *session, XmaFrame **frame_list)

This function is called after calling the :c:func:`xma_scaler_session_send_frame`. This function returns a list of output frames with every call until it reaches end of scaling. Return codes can only be :c:macro:`XMA_SUCCESS` and :c:macro:`XMA_ERROR`.

The scaler plugin supports both :c:macro:`XMA_HOST_BUFFER_TYPE` and :c:macro:`XMA_DEVICE_BUFFER_TYPE` output buffers. The application indicates the buffer type through the :c:struct:`XmaFrameProperties` of the :c:struct:`XmaFrame` specified in the frame list.

When using :c:macro:`XMA_HOST_BUFFER_TYPE` buffers, the application is responsible for allocating the host memory for each frame. An example of how to do this can be found in the :url_to_repo:`examples/xma/scale_only/src/xlnx_scal_utils.c` file of the sample XMA scaler app.

When using :c:macro:`XMA_DEVICE_BUFFER_TYPE` buffers, the scaler plugin takes care of allocating XVBM buffers. The application can then access the buffer, release it the plugin or transfer it to another plugin using the XVBM APIs, as explained in the :ref:`XVBM library <xvbm_reference>` section. An example of a scaler session using :c:macro:`XMA_DEVICE_BUFFER_TYPE` buffers can be found in the :url_to_repo:`examples/xma/transcode/src/xlnx_scaler.c` file of the sample XMA transcoder app.

|

.. c:function:: int32_t xma_scaler_session_destroy(XmaScalerSession *session)

This function destroys scaler session that was previously created with the :c:func:`xma_scaler_session_create` function.

|

Scaler Properties
======================

The Xilinx scaler is configured using a combination of standard XMA scaler properties, standard XMA scaler input and ouput properties and custom scaler parameters, all of which are specified using :c:struct:`XmaScalerFilterProperties` and :c:struct:`XmaScalerInOutProperties` data structures. 

To facilitate application development, Xilinx recommends working with a simplified data structure from which the required :c:struct:`XmaScalerFilterProperties` and :c:struct:`XmaScalerInOutProperties` can be populated using a specialized function. A reusable example of this can found in the :url_to_repo:`examples/xma/transcode/include/xlnx_transcoder_xma_props.h` and :url_to_repo:`examples/xma/transcode/src/xlnx_transcoder_xma_props.c` files of the XMA transcoder example application.

|

.. c:struct:: XmaScalerFilterProperties

This data structure is used to configure the Xilinx scaler. The declaration of :c:struct:`XmaScalerFilterProperties` can be found in the `xmascaler.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmascaler.h>`_ file.


.. c:struct:: XmaScalerInOutProperties

This data structure is used to configure the input and outputs of the video scaler. The :c:struct:`XmaScalerFilterProperties` data structure contains one :c:struct:`XmaScalerInOutProperties` for the scaler input and an array of 8 :c:struct:`XmaScalerInOutProperties` for the scaler outputs. The declaration of :c:struct:`XmaScalerInOutProperties` can be found in the `xmascaler.h <https://github.com/Xilinx/XRT/blob/master/src/xma/include/app/xmascaler.h>`_ file.

|

.. rubric:: Standard XMA Scaler Properties

When using the scaler plugin, the following members of the :c:struct:`XmaScalerFilterProperties` data structure must be set by the application:

hwencoder_type
    Vendor value used to identify the scaler type.
    Must be set to :c:macro:`XMA_POLYPHASE_SCALER_TYPE`.

hwvendor_string[MAX_VENDOR_NAME]
    Vendor string used to identify specific scaler requested.
    Must be set to "Xilinx"    

num_outputs
    Number of scaler outputs.

params
    Array of custom initialization parameters.
    See the next section for the list of custom parameters supported by the scaler plugin.

param_cnt
    Count of custom parameters.

plugin_lib
    The plugin library name to which the application wants to communicate.
    The value of this property is obtained as part of XRM resource allocation.

dev_index
    The device index number on which the scaler resource has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

cu_index
    The scaler coding unit(cu) index that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.

channel_id
    The channel number of the scaler that has been allocated.
    The value of this property is obtained as part of XRM resource allocation.
 
ddr_bank_index
    Must be set to -1 to let the hardware determine which DDR bank should be used for this channel.


Other members of :c:struct:`XmaScalerFilterProperties` are not applicable to the scaler plugin and should not be used.


.. rubric:: XMA Scaler Input and Output Properties

When configuring the scaler input and outputs, the following members of the :c:struct:`XmaScalerInOutProperties` data structure must be set by the application:

format
    Input video format.
    Must be set to :c:macro:`XMA_VCU_NV12_FMT_TYPE`

width
    Width in pixels of incoming video stream/data.
    Valid values are integers between 128 and 3840, in multiples of 4.
    Portrait mode is supported.

height
    Height in pixels of incoming video stream/data.
    Valid values are even integers between 128 and 2160, in multiples of 4.

stride
    Stride must be set as input width aligned by 256.

framerate
    Framerate data structure specifying frame rate per second of the input stream. To specify a lower output frame rate, refer to the :ref:`Mix-Rate Support <mix-rate-support>` section below. This value is also used by the plugin to calculate the scaler load which determines how many hardware resources to allocate. Leaving the framerate undefined could lead to undefined behavior.

Other members of :c:struct:`XmaScalerInOutProperties` are not applicable to the scaler plugin and should not be used.


.. rubric:: Custom Scaler Parameters

In addition to the standard properties, the following :c:struct:`XmaParameter` custom parameters are supported by the scaler plugin:

"enable_pipeline"
    Enable/Disable pipeline in scaler. Enabling pipeline increases the scaler speed.

"logLevel"
    Enables XMA logging in scaler module. Supported values are 0 to 3.

"MixRate"
    This parameter is used to configure mix-rate sessions where some scaler outputs are configured at the input frame rate and some other outputs will be configured at half the rate.
    For single-rate scaling, this parameter must be set to null.
    For mix-rate scaling, the application will need to create two different scaler sessions. The MixRate parameter of the first session must be set to null, and the MixRate parameter of the second session must be set to the address of the first session. See section below for more details on how to set-up mix-rate support in the scaler.

"latency_logging"
    Set to 1 to enable logging of latency information to syslog. Set to 0 to disable logging.

.. _mix-rate-support:

Mix-Rate Support with the Scaler Plugin
---------------------------------------
The application can configure the scaler to work at mixed rate, where some output channels will be produced at the full input frame rate and some output channels will be produced at half the input frame rate. 

Mix-rate is achieved by creating two different scaler sessions. One for full rate and the one for full and half rate (all rate) outputs.

Enabling mixed rate outputs requires that the following conditions be met:

#. The first output channel must be full rate
#. The full rate channels should be specified at the beginning followed by half rate channels, i.e., no full rate channel to be specified after half rate during session creation. This simplifies output buffer handling.

Steps to implement full rate and half rate in application:

#. Create two scaler sessions, one for full rate channels and the other for full rate and half rate (all rate) channels.
#. Set the full rate session fps to half, since the full rate outputs will be received from both the sessions.
#. When creating the second session, use the address of the first session as value of the "MixRate" custom parameter. Based on this, the scaler plugin allocates more output buffers.
#. Call scaler send and receive with full rate and all rate sessions alternatively.

For an example of how to implement mix-rate scaling, refer to the :url_to_repo:`examples/xma/transcode/src/xlnx_scaler.c` file in the sample XMA scaler application.

..
  ------------
  
  © Copyright 2020-2023, Advanced Micro Devices, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.