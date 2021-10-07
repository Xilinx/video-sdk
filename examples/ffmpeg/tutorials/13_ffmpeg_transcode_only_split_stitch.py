#
# Copyright 2020-2021 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); 
# you may not use this file except in compliance with the License. 
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software 
# distributed under the License is distributed on an "AS IS" BASIS, 
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
# See the License for the specific language governing permissions and 
# limitations under the License.
#

# This script accepts an 8-bit, YUV420, pre-encoded file and will send the encoded output to a user-defined output path.
# The aspect ratio must 16:9 and the resolution 720p or more.

# The script starts by automatically detecting the number of devices available in the system and then determines how many 
# jobs can be run on each device based on the resolution of the input. The input file is then split in as many segments 
# of equal length. Parallel FFmpeg jobs are submited to transcode all the segments simultaneously. The '-xlnx_hwdev' option 
# is used to dispatch each job on a specific device. Once all the segments have been processed, FFmpeg is used to concatenate 
# the results and form the final output stream.

# Usage: python 13_ffmpeg_transcode_only_split_stitch.py [options]

# Options: 
#   -s INPUT_FILE,    --sourcefile=INPUT_FILE
#                     input file to convert
#   -d OUTPUT_FILE,   --destinationfile=OUTPUT_FILE 
#                     output file path
#   -i INPUT_FORMAT,  --icodec=INPUT_FORMAT 
#                     input file algorithm standard <h264, hevc, h265>
#                     default: h264
#   -o OUTPUT_FORMAT, --ocodec=OUTPUT_FORMAT
#                     output file algorithm standard <h264, hevc, h265>
#                     default: hevc
#   -b BITRATE,       --bitrate=BITRATE
#                     output bitrate in Mbit/s. Must be a float or integer value between 1.0 and 25.0
#                     default: 5.0
#                     (example: use -b 3 to specify an output bitrate of 3Mbits/sec)
ASPECT_RATIO = (16/9)

import subprocess
from optparse import OptionParser
import time
from datetime import datetime
import json
import re

def count_substrings(string, substring):
    string_size = len(string)
    substring_size = len(substring)
    count = 0
    for i in range(0,string_size-substring_size+1):
        if string[i:i+substring_size] == substring:
            count+=1
    return count

def main():
    (filename, ofilename, input_encoder, output_encoder, bitrate) = parse_options()
    
    output = subprocess.Popen("xbutil scan",
                          shell = True,
                          stdout = subprocess.PIPE).stdout.read()
    outputS = str(output)
    result = outputS.find('Found total ')
    if (result == -1):
        print ("Can't determine number of U30s in the system, exiting ...")
        raise SystemExit

    num_devices = int(re.search(r'\d+', outputS).group())
    print ("There are " + str(int(num_devices/2)) + " cards, " + str(num_devices) + " devices in the system")

    xres = int(re.search(r'\d+', outputS).group()) 
    if input_encoder == "h265":
        input_encoder = "hevc"
    if input_encoder != "hevc" and input_encoder != "h264":
        print ("Input encoder needs to be h264, h265 or hevc")
        raise SystemExit

    if output_encoder == "h265":
            output_encoder = "hevc"
    if output_encoder != "hevc" and output_encoder != "h264":
        print ("Output encoder needs to be h264, h265 or hevc")
        raise SystemExit

    if bitrate < 1.0 or bitrate > 25.0:
        print ("Bitrate should be between 1.0 ... 25.0 Mbit/s")
        raise SystemExit
    br =str(bitrate)

    if ofilename[-4:] != ".mp4":
        print ("Only mp4 output file format supported")
        raise SystemExit        
    if filename[-4:] != ".mp4" and filename[-4:] != ".mov" and filename[-4:] != ".mkv" and filename[-4:] != ".MOV":
        print ("Only mp4 & mov & mkv input file format supported")
        raise SystemExit        
    
    if filename == ofilename:
        print ("Source and destination filename cannot be the same")
        raise SystemExit 
    
    startSec = time.time()
    #ffprobe -v error -select_streams v:0 -show_entries stream=width,height,duration,r_frame_rate -of default=nw=1

    output = subprocess.Popen("ffprobe -v error -select_streams v:0 -show_entries stream=width -of default=nw=1 "+filename+" 2>&1",
                              shell = True,
                              stdout = subprocess.PIPE).stdout.read()
    outputS = str(output)    
    result = outputS.find('width=')
    if (result == -1):
        print ("Can't determine clip resolution, exiting ...")
        raise SystemExit    
    xres = int(re.search(r'\d+', outputS).group())

    output = subprocess.Popen("ffprobe -v error -select_streams v:0 -show_entries stream=height -of default=nw=1 "+filename+" 2>&1",
                              shell = True,
                              stdout = subprocess.PIPE).stdout.read()
    outputS = str(output)    
    result = outputS.find('height=')
    if (result == -1):
        print ("Can't determine clip resolution, exiting ...")
        raise SystemExit    
    yres = int(re.search(r'\d+', outputS).group())

    # find out length of the clip such that we can determine segments sizes
    output = subprocess.Popen("ffprobe "+filename+" 2>&1",
                              shell = True,
                              stdout = subprocess.PIPE).stdout.read()

    outputS = str(output)
   
    #extract the framerate from the string
    result = outputS.find('fps, ')
    if (result == -1):
        print ("Can't determine framerate, exiting ...")
        raise SystemExit
    tmpS = outputS[result+5:result+14]
    framerateS = tmpS.split()
    framerate = float (framerateS[0])
    
    print("")
    #extract the video duration from the string
    result = outputS.find('Duration: ') 
    if (result == -1):
        print ("Can't determine video length, exiting ...")
        raise SystemExit    
    video_lengthS = outputS[result+10:result+18]   
    try:
        pt = datetime.strptime(video_lengthS,'%H:%M:%S')
        video_length = pt.second + pt.minute*60 + pt.hour*3600
        print("Video clip parameters:")
        print ("      length in seconds : "+str(video_length))
        print ("      length in hh:mm:ss: "+video_lengthS)
    except ValueError:
        print ("Can't determine video length, exiting ...")
        raise SystemExit
    
    print("      resolution: "+ str(xres)+"x"+str(yres))
    print("      framerate: "+ str(framerate))

    totFrames = video_length * framerate
   
    if float((xres/yres)/(ASPECT_RATIO)) != 1.0 :
        print ("Example script only supports 16:9 aspect ratios (e.g. 4k, 1080p, 720p)")
        raise SystemExit
    elif xres == 3840:
        device_split_count = 1 * (int(60/framerate))
        maxFPS=num_devices * 60
    elif xres == 1920:
        device_split_count = 4 * (int(60/framerate))
        maxFPS=num_devices * 240
    elif xres == 1280:
        device_split_count = 9 * (int(60/framerate))
        maxFPS=num_devices * 540
    else:
        print ("Resolutions lower than 720p not implemented, exiting!")
        raise SystemExit
   
    split_count = device_split_count * num_devices

    framesinClip = framerate * video_length / split_count
    split_length = int(video_length / split_count) + 1

    print ("")
    print ("Start splitting clip in " + str(split_count)+ " segments")
    # creating cmd to be run for splitting into segments
    if split_count != 1:
        split_cmd = "ffmpeg -nostdin -loglevel info  -vsync 0 -i " + filename + " -c copy -f segment -segment_time " \
            + str(split_length) + " -y tmpfile" + "%2d." + filename[-3:] + " > stdout.log 2>&1 \n"
    else:
        split_cmd = "cp " + filename + " tmpfile00." + filename[-3:]
    
    # run the command in a blocking way
    output = subprocess.Popen(split_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
       
    # check if the number of segments written equals the desired split_count
    output = subprocess.Popen("ls tmpfile* | wc -l", shell = True, stdout = subprocess.PIPE).stdout.read()
    if int(output) < split_count:
        print ("Video file may not be splittable ...")
        print ("Only able to create " + str(int(output)) + " segments for parallel processing")
        raise SystemExit   
    if int(output) > split_count:
        print ("Too many tmpfiles; Please delete old tmpfiles ...")
        raise SystemExit       
    print ("")

    clipNum = 0

    for n in range(0, num_devices):        
        for m in range(0, device_split_count):
            transcode_cmd = "ffmpeg -loglevel info -xlnx_hwdev "+ str(n)+" -vsync 0 -c:v mpsoc_vcu_" + input_encoder + " -i tmpfile" + \
                            format(clipNum, '02d') + filename[-4:] + \
                            " -periodicity-idr 120 -b:v " + br + "M -max-bitrate " + \
                            br + "M -c:v mpsoc_vcu_" \
                            + output_encoder + " -y tmpfileout" + \
                            format(clipNum, '02d') + ofilename[-4:] + " > stdout" +str(n)+".log 2>&1 & \n"
            output = subprocess.Popen(transcode_cmd, shell = True)
            time.sleep(0.1)
            clipNum += 1
    
    print ("Start transcoding segments")
    # wait until all ffmpeg processes are done
    pidsExist = True
    
    tail_cmd = "tail -1 stdout0.log"
    ps_cmd = "ps -ef | grep ffmpeg"  
    percentDone = 10   
    print("")
    print("  0 percent of transcoding completed")
    while pidsExist:
        time.sleep(0.1)
               
        output = subprocess.Popen(ps_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
        nr = count_substrings(str(output), "ffmpeg -loglevel info -xlnx_hwdev")
        if nr == 0:
            pidsExist = False

        output = subprocess.Popen(tail_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
        outputS = str(output)
        outputpartS = outputS[-150:]
        result = outputpartS.find('frame=')
        if result != -1:
            frameS = outputpartS[result+6:result+20].split()
            frame = int(frameS[0])
            if int(100.0 * frame/framesinClip) > percentDone:
                if percentDone > 95:
                    percentDone = 150
                else:
                    print(" " + str(percentDone) + " percent of transcoding completed")
                if percentDone > 89:
                    percentDone = percentDone + 5
                else:
                    percentDone = percentDone + 10
                
    print("100 percent of transcoding completed")   
    #start concatenating the transcoded files
    
    print("")
    print ("Start concatenating segments into final clip")
    cmd = "printf \"file '%s'\\n\" tmpfileout* > mylist.txt"
    output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    cmd = "rm -f " + ofilename
    output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    cmd = "ffmpeg -f concat -safe 0 -i mylist.txt -c copy " + ofilename + " > stdout.log 2>&1"
    output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    cmd = "rm tmpfile*"
    output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    cmd = "rm mylist.txt"
    output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    cmd = "rm stdout*.log"
    # output = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
    endSec = time.time()
    totSec = int(endSec-startSec)

    print(" ")
    
    if totSec > 119:
        print("Time from start to completion : "+ str(totSec) + \
            " seconds (" + str(int(totSec/60)) + " minutes and " + \
            str(totSec - 60*(int(totSec/60))) + " seconds)")
    elif totSec > 59:
        print("Time from start to completion : "+ str(totSec) + \
            " seconds (1 minute and " + \
            str(totSec - 60) + " seconds)")
    else:
        print("Time from start to completion : "+ str(totSec) + \
            " seconds")
    print(" ")
    print("This clip was processed "+str(round(1.0*video_length/totSec,1))+" times faster than realtime")
    print(" ")
    print("This clip was effectively processed at " + str(round(totFrames/totSec,2)) + " FPS")
    print(" ")
    print("Efficiency=" + str(round((totFrames/totSec)/maxFPS,2)*100) + "%")
def destroy():
	# Release resource
    print("Exiting ...")
    
def parse_options():
    parser = OptionParser()
    parser.add_option("-s", "--sourcefile",
                      dest = "ifilename",
                      help = "input file to convert",
                      type = "string",
                      action = "store"
    )
    parser.add_option("-d", "--destinationfile",
                      dest = "ofilename",
                      help = "output file",
                      type = "string",
                      action = "store"
    )
    parser.add_option("-i", "--icodec",
                      dest = "input_encoder",
                      help = "input encode standard <h264, hevc, h265> \
                              default h264",
                      type = "string",
                      action = "store", default = "h264"
    )
    parser.add_option("-o", "--ocodec",
                      dest = "output_encoder",
                      help = "output encode standard <h264, hevc, h265> \
                              default hevc",
                      type = "string",
                      action = "store", default = "hevc"
    )
    parser.add_option("-b", "--bitrate",
                      dest = "bitrate",
                      help = "output bitrate in Mbit/s. Must be a float or integer value between 1.0 and 25.0 (example: use -b 3 to specify an output bitrate of 3Mbits/sec) \
                              default 5.0",
                      type = "float",
                      action = "store", default = 5.0
    )

    (options, args) = parser.parse_args()
    if options.ifilename and options.ofilename:
        return (options.ifilename, options.ofilename, \
                options.input_encoder, options.output_encoder,options.bitrate)
    else:
        parser.print_help()
        raise SystemExit

if __name__ == '__main__':
	try:
		main()
	# When 'Ctrl+C' is pressed, the child program 
	# destroy() will be executed.
	except KeyboardInterrupt:
		destroy()

