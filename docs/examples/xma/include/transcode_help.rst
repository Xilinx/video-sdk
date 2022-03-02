 XMA Transcoder App Usage: 
	./program [generic options] -c:v <decoder codec> [decoder options]  -i input-file -multiscale_xma -outputs [num] [Scaler options]  -c:v <encoder codec> [encoder options] -o <output-file>  -c:v <encoder codec> [encoder options] -o <output-file>  -c:v <encoder codec> [encoder options] -o <output-file>..... 

Arguments:

	--help                     Print this message and exit.
	-d <device-id>             Specify a device on which the 
	                           transcoder to run. Default: 0
	-stream_loop <loop-count>  Number of times to loop the input file 
	-frames <frame-count>      Number of input frames to be processed 
	                           
Decoder options:

	-c:v <codec>               Decoder codec to be used. Supported 
	                           are mpsoc_vcu_hevc, mpsoc_vcu_h264 
	-low-latency <0/1>         Low latency for decoder. Default 
	                           disabled 
	-latency_logging <0/1>     Latency logging for decoder. Default 
	                           disabled 
	-i <input-file>            Name and path of input H.264/HEVC file 
	                           
Scaler options:

	-multiscale_xma            Name of the ABR scaler filter 
	-num-output <value>        Number of output files from scaler 
	-out_1_width <width>       Width of the scaler output channel 1 
	-out_1_height <height>     Height of the scaler output channel 1 
	-out_1_rate <full/half>    Full of Half rate for output channel 1 
	-out_2_width <width>       Width of the scaler output channel 2 
	-out_2_height <height>     Height of the scaler output channel 2 
	-out_2_rate <full/half>    Full of Half rate for output channel 2 
	-out_3_width <width>       Width of the scaler output channel 3 
	-out_3_height <height>     Height of the scaler output channel 3 
	-out_3_rate <full/half>    Full of Half rate for output channel 3 
	-out_4_width <width>       Width of the scaler output channel 4 
	-out_4_height <height>     Height of the scaler output channel 4 
	-out_4_rate <full/half>    Full of Half rate for output channel 4 
	-out_5_width <width>       Width of the scaler output channel 5 
	-out_5_height <height>     Height of the scaler output channel 5 
	-out_5_rate <full/half>    Full of Half rate for output channel 5 
	-out_6_width <width>       Width of the scaler output channel 6 
	-out_6_height <height>     Height of the scaler output channel 6 
	-out_6_rate <full/half>    Full of Half rate for output channel 6 
	-out_7_width <width>       Width of the scaler output channel 7 
	-out_7_height <height>     Height of the scaler output channel 7 
	-out_7_rate <full/half>    Full of Half rate for output channel 7 
	-out_8_width <width>       Width of the scaler output channel 8 
	-out_8_height <height>     Height of the scaler output channel 8 
	-out_8_rate <full/half>    Full of Half rate for output channel 8 
	-latency_logging <0/1>     Latency logging for scaler. Default 
	                           disabled 
Encoder options:

	-c:v <codec>               Encoder codec to be used. Supported 
	                           are mpsoc_vcu_hevc, mpsoc_vcu_h264 
	-b:v <bitrate>             Bitrate can be given in Kbps or Mbps 
	                           or bits i.e., 5000000, 5000K, 5M. 
	                           Default is 200kbps 
	-fps <fps>                 Input frame rate. Default is 25. 
	-g <intraperiod>           Intra period. Default is 12. 
	-control-rate <mode>       Rate control mode. Supported are 0 
	                           to 3, default is 1.
	-max-bitrate <bitrate>     Maximum bit rate. Supported are 0 to 
	                           350000000, default is 5000 
	-slice-qp <qp>             Slice QP. Supported are -1 to 51, 
	                           default is -1 
	-min-qp <qp>               Minimum QP. Supported are 0 to 51, 
	                           default is 0. 
	-max-qp <qp>               Maximum QP. Supported values are 0 
	                           to 51, default is 51. 
	-bf <frames>               Number of B frames. Supported are 0 
	                           to 7, default is 2. 
	-periodicity-idr <value>   IDR picture frequency. Supported are 
	                           0 to UINT32_MAX, default is 
	                           UINT32_MAX. 
	-profile <value>           Encoder profile. 
	           For HEVC, supported are 0 or main and 1 or main-intra. 
	                           Default is 0. 
	           For H264, supported are 66 or baseline, 77 or main 
	                           and 100 or high. Default is 100 
	-level <value>             Encoder level. 
	                           For HEVC, supported are 10 to 51, 
	                           default is 50. 
	                           For H264, supported are 10 to 52, 
	                           default is 50. 
	-slices <value>            Number of slices per frame. Supported 
	                           are 1 to 68, default is 1. 
	-qp-mode <mode>            QP mode. Supported are 0, 1, and 2, 
	                           default is 1. 
	-aspect-ratio <value>      Aspect ratio. Supported values are 0 
	                           to 3, default is 0. 
	-scaling-list <0/1>        Scaling list. Enable/Disable, 
	                           default enable. 
	-lookahead-depth <value>   Lookahead depth. Supported are 0 to 
	                           20, default is 0. 
	-temporal-aq <0/1>         Temporal AQ. Enable/Disable, 
	                           default disable. 
	-spatial-aq <0/1>          Spatial AQ. Enable/Disable, 
	                           default disable. 
	-spatial-aq-gain <value>   Spatial AQ gain. Supported are 0 to 
	                           100, default is 50. 
	-cores <value>             Number of cores to use, supported are 
	                           0 to 4, default is 0. 
	-tune-metrics <0/1>        Tunes MPSoC H.264/HEVC encoder's video 
	                           quality for objective metrics, default 
	                           disable. 
	-latency_logging <0/1>     Enable latency logging in syslog.
	-o <file>                  File to which output is written.

