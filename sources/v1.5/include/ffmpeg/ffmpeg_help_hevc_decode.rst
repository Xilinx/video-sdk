Decoder mpsoc_vcu_hevc [MPSOC HEVC Decoder]:
    General capabilities: delay avoidprobe 
    Threading capabilities: none
    Supported pixel formats: xlnx_xvbm
MPSOC HEVC decoder AVOptions:
  -low_latency       <int>        .D.V..... Should low latency decoding be used (from 0 to 1) (default 0)
  -entropy_buffers_count <int>        .D.V..... Specify number of internal entropy buffers (from 2 to 10) (default 2)
  -latency_logging   <int>        .D.V..... Log latency information to syslog (from 0 to 1) (default 0)
  -splitbuff_mode    <int>        .D.V..... configure decoder in split/unsplit input buffer mode (from 0 to 1) (default 0)

