This program ingests an NV12 input file and utilizes hardware acceleration to scale to various resolutions. 

Usage:
	./u30_xma_scale [options] -w <input-width> -h <input-height> -i 
	<input-file> [scaler_options] -w <output-1-width> -h 
	<output-1-height> -o <output-1-file> -w ...

Arguments:
	--help                     Print this message and exit
	-log <level>               Specify the log level
	-d <device-id>             Specify a device on which to run.
	                           Default: 0

Input Arguments:

	-stream_loop <loop-count>  Number of times to loop the input
	                           file
	-w <width>                 Specify the input's width
	-h <height>                Specify the input's height
	-pix_fmt <fmt>             Specify the input's pixel format.
	                           nv12 is the only format supported.
	-fps <frame-rate>          Frame rate. Used for scaler load
	                           calculation.
	-i <input-file>            Input file to be used

Output Arguments:
	-coeff_load <load>         Specify the coefficient load. 0 Auto 
	                           (default), 1 static, 2 FilterCoef.txt.
	-enable-pipeline           Enable scaler pipeline
	-rate <half/full>          Set the rate to half. Half rate drops 
	                           frames to reduce resource usage. 
	                           Default: full.
	-enable-latency-logging    Enable latency logging
	-w <width>                 Specify the output's width
	-h <height>                Specify the output's height
	-frames <frame-count>      Number of frames to be processed.
	-o <file>                  File to which output is written.

