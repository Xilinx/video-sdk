This is a standalone xma decoder app. It ingests an h264 or h265
encoded file and utilizes hardware acceleration to get the decoded
output.

Usage:
	./Debug/u30_xma_decode [options] -i <input-file> -c:v <codec-type>
	[codec_options] -o <output-file>

Arguments:
	--help                     Print this message and exit
	-log <level>               Specify the log level
	-d <device-id>             Specify a device on which to run.
	                           Default: 0

Input Arguments:

	-stream_loop <loop-count>  Number of times to loop the input
	                           file
	-i <input-file>            Input file to be used

Codec Arguments:

	-c:v <codec>               Specify H264 or H265 decoding.
	                           (mpsoc_vcu_h264, mpsoc_vcu_hevc)
	-low_latency               Should low latency decoding be used
	-entropy_buf_cnt <count>   Specify number of internal entropy
	                           buffers. [2-10], default: 2
	-latency_logging           Log latency information to syslog
	-splitbuff_mode            Configure decoder in split/unsplit
	                           input buffer mode
	-frames <frame-count>      Number of frames to be processed.
	-pix_fmt fmt               The output format (nv12, yuv420p,
	                           yuv420p10le, xv15) Default: nv12 8 bit,
	                           yuv420p10le 10 bit
	-o <file>                  File to which output is written.

