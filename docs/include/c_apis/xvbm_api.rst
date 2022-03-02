
.. _xvbm_reference:

*************************************
XVBM API Reference
*************************************

The Xilinx Video Buffer Management (XVBM) library is used by the |SDK| plugins to manage pools of video buffers. The XVBM API must be used to interact with the XVBM buffers associated with c:struct:`XmaFrame` frames of the :c:macro:`XMA_DEVICE_BUFFER_TYPE` type.

The :c:macro:`XMA_DEVICE_BUFFER_TYPE` frames and their XVBM buffers can either be directly passed to other hardware accelerators without being copied back to the host (zero-copy operation in a multistage pipeline) or copied back to the host for further processing in software.

If the application needs to access the content of a XVBM buffer, it must do so using the XVBM :c:func:`xvbm_buffer_get_host_ptr` and :c:func:`xvbm_buffer_read` APIs.

If a XVBM buffer is transferred to more than one other XMA plugin session, the :c:func:`xvbm_buffer_refcnt_inc` API should be used to split the buffer instead of explicitly creating copies of that buffer. For an example of this, refer to the :c:func:`xlnx_tran_xvbm_buf_inc` function in :url_to_repo:`examples/xma/transcode/src/xlnx_transcoder.c`.

If a XVBM buffer is not transferred to another plugin, then the application must release the buffer with the :c:func:`xvbm_buffer_pool_entry_free` API. This releases the buffer back to the plugin, allowing the plugin to reuse the buffer for a subsequent frame. Typically, if all the buffers managed by a plugin are used (not freed), then the plugin won’t be able to accept new data.

XVBM buffers are directly managed by the |SDK| plugins. The user application can read and release XVBM buffers, but it should not create or destroy XVBM buffers. 

|

Buffer Pool Related Functions
=============================

.. c:function:: XvbmPoolHandle xvbm_buffer_pool_create(xclDeviceHandle d_handle, int32_t num_buffers, size_t size, uint32_t flags)

Create a memory pool and allocate device buffers. Returns XvbmPoolHandle used for all subsequent memory pool requests.

|

.. c:function:: XvbmPoolHandle xvbm_buffer_pool_create_by_device_id(int32_t device_id, int32_t num_buffers, size_t size, uint32_t flags)

Create a memory pool and allocate device buffers using a device ID. Returns XvbmPoolHandle used for all subsequent memory pool requests.

|

.. c:function:: void xvbm_buffer_pool_offsets_set(XvbmPoolHandle p_handle, uint32_t offsets, uint32_t num_offsets)

Set offsets for all buffers in the pool (useful for planes of a frame). 

|

.. c:function:: uint32_t xvbm_buffer_pool_offset_get(XvbmBufferHandle b_handle, uint32_t offset_idx)

Get offset for a buffer in a pool.

|

.. c:function:: int32_t xvbm_buffer_pool_extend(XvbmBufferHandle b_handle, int32_t num_buffers)

Extend an existing memory pool allocating additional device buffers. Returns Number of buffers allocated to the pool. If the current number of buffers allocated is greater than the number of buffers requested, the number of buffers allocated will remain unchanged.

|

.. c:function:: int32_t xvbm_buffer_pool_num_buffers_get(XvbmBufferHandle b_handle)

Get the number of buffers allocated to the pool associated with a buffer. Returns number of buffers allocated to the associated pool.

|

.. c:function:: XvbmBufferHandle xvbm_buffer_pool_entry_alloc(XvbmPoolHandle p_handle)

Allocate a free buffer from a memory pool. Returns XvbmBufferHandle A non-NULL handle used for buffer operations. 

|

.. c:function:: bool xvbm_buffer_pool_entry_free(XvbmBufferHandle b_handle)

Free a buffer and return it back to the memory pool free list. 

|

.. c:function:: void xvbm_buffer_pool_destroy(XvbmPoolHandle p_handle)

Destroy all resources associated with a buffer pool. Returns true if entry is freed, otherwise false.

|

Buffer related accessor functions
=================================

.. c:function:: uint32_t xvbm_buffer_get_bo_handle(XvbmBufferHandle b_handle)

Get the BO handle from a xvbmBufferHandle. Returns a valid BO handle or -1 if not valid.

|

.. c:function:: uint32_t xvbm_buffer_get_id(XvbmBufferHandle b_handle)

Get the buffer ID used by Host and MPSoC device. Returns an index of the buffer. 

|

.. c:function:: size_t xvbm_buffer_get_size(XvbmBufferHandle b_handle)

Get the buffer size. Returns size of the buffer.

|

.. c:function:: uint64_t xvbm_buffer_get_paddr(XvbmBufferHandle b_handle)

Get the buffer physical address. Returns the physical address of the buffer.

|

.. c:function:: XvbmBufferHandle xvbm_buffer_get_handle(XvbmPoolHandle p_handle, uint64_t paddr)

Get the buffer handle given a physical address. Returns the buffer handle. 

|

.. c:function:: void xvbm_buffer_refcnt_inc(XvbmBufferHandle b_handle)

Increment the reference count of the buffer. Must be used when a buffer is used by more than one other accelerator, for instance when the output of the decoder is used by both the scaler and the encoder.

|

.. c:function:: uint32_t xvbm_buffer_get_refcnt(XvbmBufferHandle b_handle)

Get the reference count of the buffer.

|

.. c:function:: void xvbm_buffer_get_host_ptr(XvbmBufferHandle b_handle)

Get the host buffer handle. Returns the virtual pointer of the allocated host buffer.

|

Functions for reading and writing buffers
=========================================

.. c:function:: int32_t xvbm_buffer_write(XvbmBufferHandle b_handle, const void src, size_t size, size_t offset)

Write a buffer to device memory. Returns 0 on success. 

|

.. c:function:: int32_t xvbm_buffer_read(XvbmBufferHandle b_handle, void dst, size_t size, size_t offset)

Read a buffer from device memory. Returns 0 on success.

..
  ------------
  
  © Copyright 2020-2021 Xilinx, Inc.
  
  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at
  
  http://www.apache.org/licenses/LICENSE-2.0
  
  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.