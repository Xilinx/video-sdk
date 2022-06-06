#
# Copyright 2021 Xilinx, Inc.
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

import re
import math
import subprocess
from optparse import OptionParser
import time
import os
import array

#statusPrint 0 == do nothing, 1 == regular print, 2 == logfile, 3 == regular print and logfile
statusPrint = 1
def logPrint(*args):
    message = ""
    for arg in args:
        message += arg.__str__() + " "
    if (statusPrint == 1) or (statusPrint == 3):       
        print(message)
    if (statusPrint == 2) or (statusPrint == 3):
        fLog = open("smoketest.log", "a")
        message = message + "\n"
        fLog.write(message)
        fLog.close()

def createClip(width, height, frames, filename):
    f = open(filename, "wb")
    nrframes = frames
    speedLineH = 0.5 * width / frames
    speedLineV = 0.5 * height / frames

    widthY   = width
    heightY  = height
    widthUV  = int(widthY/2)
    heightUV = int(heightY/2)
    widthUV2  = int(widthUV/2)  

    arrY  = bytearray(widthY * heightY)
    arrU = bytearray(2 * widthUV * heightUV)
    #arrV = bytearray(widthUV * heightUV)
    startSec = time.time()

    # create a zoneplate in a resolution 2 x h and 2 x v of the clip size
    # this way we can easily make it a moving zoneplate
    arrZP = bytearray(4 * widthY * heightY)
    for y in range(0, 2 * heightY):
        tmp1 = y * 0.0000003
        tmp1 = y * 0.00000278
        ytmp2 = y * 2 * widthY
        for x in range(0, 2 * widthY):           
            tmp = math.cos(tmp1 * x * x)
            Y = int(127 * (1.0 + tmp))
            arrZP[x + ytmp2] = Y  

    for fr in range(0, nrframes):

        for z in range(0, heightY):
            # make the zonpelate look like it is moving in h and v direction
            htmp = int((fr * widthY) / frames) 
            vtmp = int((fr * heightY) / frames) 
            arrY[z*widthY:z*widthY+widthY] = arrZP[htmp+vtmp*2*widthY+z*2*widthY:htmp+vtmp*2*widthY+z*2*widthY+widthY]
        ufrtmp = (128 + int((255 / frames) * fr)) % 256
        vfrtmp = (128 - int((255 / frames) * fr)) % 256

        for y in range(0,heightUV):
            if y < heightUV/2 + 60 and y > heightUV/2 - 60:
                uvtmp1 = True
            else:
                uvtmp1 = False
            uvtmp2 = 2 * y * widthUV
            if y == (int(speedLineV*fr)):
                uvtmp3 = True
            else:
                uvtmp3 = False
            uvtmp4 = 2 * y * widthY
            uvtmp5 = (2 * y + 1) * widthY
            uvtmp8 = int(speedLineH*fr)
            for x in range(0,widthUV):
                U = ufrtmp
                V = vfrtmp
                uvtmp6 = x + x
                uvtmp7 = x + x + 1
                if uvtmp3 or x == uvtmp8:
                    U = 84
                    V = 255
                    arrY[uvtmp6 + uvtmp4] = 76
                    arrY[uvtmp7 + uvtmp4] = 76
                    arrY[uvtmp6 + uvtmp5] = 76
                    arrY[uvtmp7 + uvtmp5] = 76

                if uvtmp1 and x < widthUV2 + 60 and x > widthUV2 - 60:
                    fr255 = fr & 0xFF
                    U = fr255
                    V = fr255
                    arrY[uvtmp6 + uvtmp4] = fr255
                    arrY[uvtmp7 + uvtmp4] = fr255
                    arrY[uvtmp6 + uvtmp5] = fr255
                    arrY[uvtmp7 + uvtmp5] = fr255
                arrU[2*x + uvtmp2] = U
                arrU[2*x + uvtmp2 + 1] = V
                #arrV[x + uvtmp2] = V
                 
        f.write(arrY)
        f.write(arrU)
        #f.write(arrV)
    f.close()
    endSec = time.time()
    totSec = int(endSec-startSec)
    print("Time to create clip : " + str(totSec) + " seconds")


def testTranscode(frames, nrfiles, dir, logdir):

    xstart = 1920
    fail = 0

    if (nrfiles < 1):
        print("aborting; nr files needs to be at least 1")
        raise SystemExit(1)
    
    if (nrfiles == 1):
        xstep = 0
    else:
        xstep = int((1920 - 320) / (nrfiles-1))

    fps = [i for i in range(nrfiles)]

    #decode with U30
    for step in range(0, nrfiles):
    
        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        startSec = time.time()        
        # check if file exists already
        inputfile = dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".265"
        fe = os.path.exists(inputfile)
        if (fe == False):
            logPrint("File " + inputfile + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)

        fps[step] = 0
        
        logPrint("Transcoding HEVC "+str(x).zfill(4)+"x"+str(y).zfill(4)+" to h.264 960x540")

        transcode_cmd = "u30_xma_transcode -c:v mpsoc_vcu_hevc -i " + inputfile + \
                     " -multiscale_xma -num-output 1 -out_1_width 960 -out_1_height 540 -c:v mpsoc_vcu_h264 -control-rate 0 -qp-mode 0 -slice-qp 20 -o " \
                      +dir+"/transcode"+str(x).zfill(4)+"x"+str(y).zfill(4)+".264" \
                     " > "+logdir+"/transcodestdout" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log 2>> "+logdir+"/transcodestderr"+str(x).zfill(4)+"x"+str(y).zfill(4)+".log"

        subprocess.Popen(transcode_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()

        endSec = time.time()

        logfile = open(logdir+"/transcodestderr" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log")
        allNumbers = re.findall(r"[-+]?\d*\.\d+|\d+", logfile.read())
        if len(allNumbers) == 0:
            logPrint("Transcoder Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED: No fps stats found!")
            fps[step] = -1
            fail = 1
        else:
            fps[step] = allNumbers[-1]


    output  = subprocess.Popen("rm "+dir+"/transcode*.yuv", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()

    for step in range(0, nrfiles):

        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        #decode the transcoded file and check for correctness
        file_name = dir+"/transcode"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv"
        decode_cmd = "ffmpeg -nostdin -i " + dir+"/transcode"+str(x).zfill(4)+"x"+str(y).zfill(4)+ \
                     ".264 -c:v rawvideo -pix_fmt nv12 "+file_name

        output  = subprocess.Popen(decode_cmd, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()

        xo = 960
        yo = 540
        
        fe = os.path.exists(file_name)
        if fe:
            file_stats = os.stat(file_name)
            frames_mod = int (file_stats.st_size / (xo * yo * 1.5))
            if file_stats.st_size != int(xo * yo * frames * 1.5):
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED: Number of frames is " + str(frames_mod) + " instead of "+ str(frames))
                fail = fail + 1
                #logPrint("Exiting ...")
                #raise SystemExit(1)
            f = open(file_name, "rb")
        else:
            logPrint("File " + file_name + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)
        

        if fps[step] != 0:
            testPassY = True
            testPassUV = True
            firstframe = 0
            for i in range(0, frames_mod):

                arrY = array.array('B')
                arrU = array.array('B')
                #arrV = array.array('B')

                arrY.fromfile(f, xo*yo)
                arrU.fromfile(f, int(xo*yo/2))
                #arrV.fromfile(f, int(xo*yo/4))
 
                xval = int((xo/2)+ (xo) * (yo/2))
                uval = int((xo/2)+(xo/2) * (yo/2))
                #vval = int((xo/4)+(xo/2) * (yo/4))    

                #if (i != arrY[xval]) or (i != arrU[uval]) or (i != arrV[vval]):
                # ignoring UV for now as we know it fails
                if (i != arrY[xval]) and testPassY == True:
                    #if testPassY == True:
                    #    logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #    logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrV[vval])
                    testPassY = False
                    firstframe = i
                if ((i != arrU[uval]) or (i != arrU[uval + 1])) and testPassUV == True:
                    #if testPassUV == True:
                    #    logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #    logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrV[vval])
                    testPassUV = False
                    firstframe = i

            if testPassY == True and testPassUV == True:
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" passed. Processed @ "+str(fps[step])+" fps" )
            elif testPassY == True:
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Luma passed. Processed @ "+str(fps[step])+" fps" )
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Chroma FAILED. Processed @ "+str(fps[step])+" fps" )
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" First Mismatch detected in frame " + str(firstframe))
                fail = fail + 1
            else:
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED. Processed @ "+str(fps[step])+" fps" )
                logPrint("Transcode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED. First Mismatch detected in frame " + str(firstframe))
                fail = fail + 1
        f.close()
    return fail
        

def testDecodeHEVC(frames, nrfiles, dir, logdir):

    xstart = 1920
    fail = 0

    if (nrfiles < 1):
        print("aborting; nr files needs to be at least 1")
        raise SystemExit(1)
    
    if (nrfiles == 1):
        xstep = 0
    else:
        xstep = int((1920 - 320) / (nrfiles-1))

    fps = [i for i in range(nrfiles)]

    #decode with U30
    for step in range(0, nrfiles):
    
        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        startSec = time.time()        
        # check if file exists already
        inputfile = dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".265"
        fe = os.path.exists(inputfile)
        if (fe == False):
            logPrint("File " + inputfile + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)

        fps[step] = 0
        
        logPrint("HEVC decoding "+str(x).zfill(4)+"x"+str(y).zfill(4))

        decode_cmd = "u30_xma_decode -i " + inputfile + " -c:v mpsoc_vcu_h265 -o " + \
                     dir+"/decodehevc" + str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv" \
                     " > "+logdir+"/decodestdout" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log 2>> "+logdir+"/decodestderr"+str(x).zfill(4)+"x"+str(y).zfill(4)+".log"

        subprocess.Popen(decode_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()

        endSec = time.time()

        logfile = open(logdir+"/decodestderr" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log")
        allNumbers = re.findall(r"[-+]?\d*\.\d+|\d+", logfile.read())
        if len(allNumbers) == 0:
            logPrint("Decoder Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED: No fps stats found!")
            fps[step] = -1
            fail = 1
        else:
            fps[step] = allNumbers[-1]


    for step in range(0, nrfiles):

        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        #cmp the U30 decoded mp4 file with the RAW YUV420 output of the encoded file
        #they should be the same
        decode_cmd = "cmp " +dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv " + dir+"/decodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv"
        output  = subprocess.Popen(decode_cmd, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        
        if output != b'':
            logPrint("Decode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED with " + str(output))
            fail = 1
        else:
            logPrint("Decode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" passed. Processed @ "+str(fps[step])+" fps" )
    return fail

def testEncodeHEVC(frames, nrfiles, dir, logdir):

    xstart = 1920
    fail = 0

    if (nrfiles < 1):
        print("aborting; nr files needs to be at least 1")
        raise SystemExit(1)
    
    if (nrfiles == 1):
        xstep = 0
    else:
        xstep = int((1920 - 320) / (nrfiles-1))

    fps = [i for i in range(nrfiles)]

    #encode with U30
    for step in range(0, nrfiles):
    
        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        startSec = time.time()        
        # check if file exists already
        fe = os.path.exists(dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".265")

        fps[step] = 0
        if (fe == False):
        
            logPrint("HEVC encoding "+str(x).zfill(4)+"x"+str(y).zfill(4))

            encode_cmd = "u30_xma_encode -w "+str(x).zfill(4)+" -h "+str(y).zfill(4)+ \
                         " -i "+dir+"/scale"+str(x).zfill(4)+"x"+str(y).zfill(4)+ \
                         ".yuv -c:v mpsoc_vcu_hevc -control-rate 0 -qp-mode 0 -slice-qp 20 -o "+dir+"/encodehevc" \
                         +str(x).zfill(4)+"x"+str(y).zfill(4)+".265" \
                         " > "+logdir+"/encodestdout" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log 2>> "+logdir+"/encodestderr"+str(x).zfill(4)+"x"+str(y).zfill(4)+".log"
            
            subprocess.Popen(encode_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
            endSec = time.time()

            logfile = open(logdir+"/encodestderr" +str(x).zfill(4)+"x"+str(y).zfill(4)+".log")
            allNumbers = re.findall(r"[-+]?\d*\.\d+|\d+", logfile.read())
            if len(allNumbers) == 0:
                logPrint("Encoder Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED: No fps stats found!")
                fps[step] = -1
                fail = 1
            else:
                fps[step] = allNumbers[-1]


    for step in range(0, nrfiles):

        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        #decode the encoded file for correctness checking
        decode_cmd = "ffmpeg -nostdin -loglevel info -i "+dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+ \
                        ".265 -pix_fmt nv12 -y "+dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+ \
                        ".yuv > /dev/null 2>> /dev/null"

        subprocess.Popen(decode_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
        
        file_name = dir+"/encodehevc"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv"
        fe = os.path.exists(file_name)

        frames_mod = frames
        if fe:
            file_stats = os.stat(file_name)
            if file_stats.st_size != int(x * y * frames * 1.5):
                frames_mod = int (file_stats.st_size / (x * y * 1.5))
                logPrint("Encode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Failure: Number of frames is " + str(frames_mod) + " instead of " + str(frames))
                fail = fail + 1
                #logPrint("Exiting ...")
                #raise SystemExit(1)
            f = open(file_name, "rb")
        else:
            logPrint("File " + file_name + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)
        '''
        if fe:
            file_stats = os.stat(file_name)
            frames_mod = int (file_stats.st_size / (x * y * 1.5))

            f = open(file_name, "rb")
        else:
            logPrint("File " + file_name + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)
        '''
        
        if fps[step] != 0:
            testPassY = True
            testPassUV = True
            for i in range(0, frames_mod):

                arrY = array.array('B')
                arrU = array.array('B')
                #arrV = array.array('B')

                arrY.fromfile(f, x*y)
                arrU.fromfile(f, int(x*y/2))
                #arrV.fromfile(f, int(x*y/4))
 
                xval = int((x/2)+ (x) * (y/2))
                uval = int((x/2)+(x/2) * (y/2))
                #vval = int((x/4)+(x/2) * (y/4))    

                #if (i != arrY[xval]) or (i != arrU[uval]) or (i != arrV[vval]):
                # ignoring UV for now as we know it fails
                if (i != arrY[xval]):
                    #if testPassY == True:
                    #    logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #    logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrV[vval])
                    testPassY = False
                if (i != arrU[uval]) or (i != arrU[uval + 1]):
                    #if testPassUV == True:
                    #    logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #    logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrV[vval])
                    testPassUV = False

            if testPassY == True and testPassUV == True:
                logPrint("Encode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" passed. Processed @ "+str(fps[step])+" fps" )
            elif testPassY == True:
                logPrint("Encode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Luma passed. Processed @ "+str(fps[step])+" fps" )
                logPrint("Encode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Chroma FAILED. Processed @ "+str(fps[step])+" fps" )
                fail = fail + 1
            else:                
                logPrint("Encode Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED. Processed @ "+str(fps[step])+" fps" )
                fail = fail + 1

        f.close()
    return fail

def testScaler(width, height, frames, nrfiles, filename, dir, logdir):

    xstart = 1920
    fail = 0
    if (nrfiles < 1):
        print("aborting; nr files needs to be at least 1")
        raise SystemExit(1)
    
    if (nrfiles == 1):
        xstep = 0
    else:
        xstep = int((1920 - 320) / (nrfiles-1))

    fps = [i for i in range(nrfiles)]

    #scale with U30
    for step in range(0, nrfiles):
    
        x = 4 * int((xstart - (step*xstep)) / 4)
        #y = 8 * int((ystart - (step*ystep)) / 8)
        y = 4 * int(((x * 1080) / 1920) / 4)

        startSec = time.time()        
        # check if file exists already
        fe = os.path.exists(dir+"scale"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv")

        fps[step] = 0
        if (fe == False):
        
            #scale with U30
            logPrint("scaling to "+str(x).zfill(4)+"x"+str(y).zfill(4))
            scale_cmd = "u30_xma_scale -w "+str(width)+" -h "+str(height)+" -i "+str(filename)+ \
                         " -w "+str(x)+" -h "+str(y)+" -o "+dir+"/scale"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv" \
                          " > " + logdir + "/scalestdout.log 2>> " + logdir + "/scalestderr.log"
       
            subprocess.Popen(scale_cmd, shell = True, stdout = subprocess.PIPE).stdout.read()
            endSec = time.time()

            logfile = open(logdir + "/scalestderr.log")
            allNumbers = re.findall(r"[-+]?\d*\.\d+|\d+", logfile.read())
            if len(allNumbers) == 0:
                logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED: No fps stats found!")
                fps[step] = -1
                fail = 1
            else:
                fps[step] = allNumbers[-1]

    for step in range(0, nrfiles):

        x = 4 * int((xstart - (step*xstep)) / 4)
        y = 4 * int(((x * 1080) / 1920) / 4)

        file_name = dir+"/scale"+str(x).zfill(4)+"x"+str(y).zfill(4)+".yuv"
        fe = os.path.exists(file_name)
        frames_mod = frames
        if fe:
            file_stats = os.stat(file_name)
            if file_stats.st_size != int(x * y * frames * 1.5):
                frames_mod = int (file_stats.st_size / (x * y * 1.5))
                logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Failure: Number of frames is " + str(frames_mod) + " instead of " + str(frames))
                fail = fail + 1
                #logPrint("Exiting ...")
                #raise SystemExit(1)
            f = open(file_name, "rb")
        else:
            logPrint("File " + file_name + " doesn't exist")
            logPrint("Exiting ...")
            raise SystemExit(1)
      
        if fps[step] != 0:
            testPassY = True
            testPassUV = True
            for i in range(0, frames_mod):

                arrY = array.array('B')
                arrU = array.array('B')
                #arrV = array.array('B')

                arrY.fromfile(f, x*y)
                arrU.fromfile(f, int(x*y/2))
                #arrV.fromfile(f, int(x*y/4))
                
                xval = int((x/2)+ (x) * (y/2))
                uval = int((x/2)+ (x/2) * (y/2))
                #vval = int((x/4)+(x/2) * (y/4))       

                #if (i != arrY[xval]) or (i != arrU[uval]) or (i != arrV[vval]):
                # ignoring UV for now as we know it fails
                if (i != arrY[xval]):
                    #if testPassY == True:
                    #    logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #    logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrV[vval])
                    testPassY = False
                if (i != arrU[uval]) or (i != arrU[uval + 1]):
                    #if testPassUV == True:
                    #   logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" FAILED" )
                    #   logPrint("Mismatch :",x,y,i, arrY[xval], arrU[uval],arrU[uval+1])
                    testPassUV = False

            if testPassY == True and testPassUV == True:
                logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" passed. Processed @ "+str(fps[step])+" fps" )
            elif testPassY == True:
                logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Luma passed. Processed @ "+str(fps[step])+" fps" )
                logPrint("Scale Test: "+str(x).zfill(4)+"x"+str(y).zfill(4)+" Chroma FAILED. Processed @ "+str(fps[step])+" fps" )
                fail = fail + 1

        f.close()
    return fail

def main():
    (nrfiles, tmpdir, logdir, removefiles, iterations, minutes, frames, quit) = parse_options()

    startTest = time.time()
    current_dir = os.getcwd()

    # defaults to keep 
    width = 960
    height = 540

    if (frames > 255):
        print("upper limit of nrframes is 255")
        raise SystemExit(1)
    if (frames < 10):
        print("lower limit of nrframes is 10")
        raise SystemExit(1)

    #let's check for the presence of U30 boards
    print("")
    my_env = os.environ.copy()
    my_env["PATH"] = "/usr/sbin:/sbin:/usr/bin:/bin" + my_env["PATH"]
    output = str(subprocess.Popen("lspci |grep Xilinx", shell = True, env=my_env, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read().strip().decode("utf-8"))
    substring = "Xilinx"
    count = str(output).count(substring)  
    if count == 0:
        print("No U30 boards detected. Exiting...")
        raise SystemExit
    else:
        print("Number of U30 boards detected: "+str(count/4.0))

    if (minutes != 0):
        iterations = 0
        print("Running time bound smoketest of: "+ str(minutes)+" minutes")
    elif (iterations == 1):
        print("Running one iteration of smoketest")
    else:
        print("Running " + str(iterations)+ " iterations of smoketest")
    
    print("Testing with " + str(frames) + " video frames per clip")

    time.sleep(1)
    
    
    # check if tmpdir exists already
    fe = os.path.exists(tmpdir)
    if (fe == False):
        output = subprocess.Popen("mkdir " + tmpdir, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        if output != b'':
            print("Error occured. Exiting ...")
            print("ERROR: "+str(output))
            raise SystemExit(1)   
    else:
        print(tmpdir + " directory already exists. Removing old files..")

        output = subprocess.Popen("rm "+ tmpdir + "/tmp*.yuv", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("touch "+ tmpdir + "/checkforaccess123", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        fe = os.path.exists(tmpdir + "/checkforaccess123")
        if fe == False:
            print("Can't create files in directory "+tmpdir)
            print("Exiting ...")
            raise SystemExit(1) 
        else:
            output = subprocess.Popen("rm "+ tmpdir + "/checkforaccess123", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read() 

    # check if log exists already
    fe = os.path.exists(logdir)
    if (fe == False):
        output = subprocess.Popen("mkdir " + logdir, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        if output != b'':
            print("Error occured. Exiting ...")
            print("ERROR: "+str(output))
            raise SystemExit(1)   
    else:
        print(logdir + " directory already exists. Removing old files..")

        output = subprocess.Popen("rm "+ logdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("touch "+ logdir + "/checkforaccess123", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        fe = os.path.exists(logdir + "/checkforaccess123")
        if fe == False:
            print("Can't create files in directory "+logdir)
            print("Exiting ...")
            raise SystemExit(1) 
        else:
            output = subprocess.Popen("rm "+ logdir + "/checkforaccess123", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()   

    # check if test clip is already present
    # if so, and md5sum matches, dont recreate
    # otherwise, recreate the clip
    filename = tmpdir+"/clip"+str(width)+"x"+str(height)+"xmaApp.yuv"

    if os.path.exists(filename):
        output = subprocess.Popen("md5sum " + filename, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        checkSum255 = b'5f6b4013c40a227062574e30e0b2c784'
        checkSum10 = b'73938e7d6820efc965b2c1b54c85a5ec'
        if checkSum255 == output[:32] and frames == 255 and width == 960 and height == 540:
            print("Testclip is present; no need to generate again")
        elif checkSum10 == output[:32] and frames == 10 and width == 960 and height == 540:
            print("Testclip is present; no need to generate again")  
        else:
            print("Creating test clip with size "+str(width)+"x"+str(height))
            print("(this can take up to 30 seconds)")
            createClip(width, height, frames, filename)
    else:
        print("Testclip doesn't exist")
        print("Creating test clip with size "+str(width)+"x"+str(height))
        print("(this can take up to 30 seconds)")
        createClip(width, height, frames, filename)
    
    run = True
    runNumber = 1
    failSE = 0
    failS = 0
    failE = 0
    failD = 0
    failT = 0
    fail = 0
    while run:

        # remove intermediate temporary files
        output = subprocess.Popen("rm "+ tmpdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()

        print("Starting RUN: "+ str(runNumber))
        if (runNumber == iterations) and (iterations != 0):
            run = False
        runNumber = runNumber+1

        # test scaler
        # 1 x RAW YUV420 file --> scaler --> "nrfiles" RAW YUV420 video files 
        # check whether the scaled results are as expected

        startSec = time.time()
        logPrint(" ")
        logPrint("SCALER test: "+str(nrfiles)+" resolutions")
        failure = testScaler(width, height, frames, nrfiles, filename, tmpdir, logdir)
        endSec = time.time()
        totSec = int(endSec-startSec)
        print("Scale test time: " + str(totSec) + " seconds")
        print(" ")
        failS = failS + failure

        startSec = time.time()
        logPrint("HEVC ENCODER test: "+str(nrfiles)+" resolutions")
        failure = testEncodeHEVC(frames, nrfiles, tmpdir, logdir)
        endSec = time.time()
        totSec = int(endSec-startSec)
        print("Encode test time : " + str(totSec) + " seconds")
        print(" ") 
        failE = failE + failure

        startSec = time.time()
        logPrint("HEVC DECODER test: "+str(nrfiles)+" resolutions")
        failure = testDecodeHEVC(frames, nrfiles, tmpdir, logdir)
        endSec = time.time()
        totSec = int(endSec-startSec)
        print("Decode test time : " + str(totSec) + " seconds")
        print(" ") 
        failD = failD + failure

        startSec = time.time()
        logPrint("TRANSCODER test: "+str(nrfiles)+" resolutions")
        failure = testTranscode(frames, nrfiles, tmpdir, logdir)
        endSec = time.time()
        totSec = int(endSec-startSec)
        print("Transcode test time : " + str(totSec) + " seconds")
        print(" ") 
        failT = failT + failure

        endTest = time.time()
        totSec = int(endTest-startTest)
        print("Complete test time: " + str(totSec) + " seconds")

        fail = failS + failE + failD + failT + failSE
        if quit == "yes" and fail != 0:
            print("Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)
        if quit == "scale" and failS != 0:
            print("Scale Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)
        if quit == "encode" and failE != 0:
            print("Encode Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)
        if quit == "scaleencode" and failSE != 0:
            print("Scale+Encode Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)
        if quit == "decode" and failD != 0:
            print("Decode Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)
        if quit == "transcode" and failT != 0:
            print("Transcode Failure detected. Exiting as per commandline flag")
            raise SystemExit(1)

        
        if (minutes != 0) and totSec > (minutes * 60 ):
            run = False
    
    #if needed, remove temporary log & tmp files before exiting
    if (removefiles == "yes"):
        print("Removing log and tmp files ...")
        output = subprocess.Popen("rm "+ tmpdir + "/clip*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ tmpdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/scale*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/decode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/encode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rm "+ logdir + "/transcode*", shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rmdir "+ logdir, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
        output = subprocess.Popen("rmdir "+ tmpdir, shell = True, stderr=subprocess.STDOUT, stdout = subprocess.PIPE).stdout.read()
    
    
    pf = str(subprocess.Popen("lsb_release -i | cut -d: -f2 |sed s/'^\t'//", shell = True, stdout = subprocess.PIPE).stdout.read().strip().decode("utf-8"))
    ver = str(subprocess.Popen("lsb_release -r | cut -d: -f2 |sed s/'^\t'//", shell = True, stdout = subprocess.PIPE).stdout.read().strip().decode("utf-8"))
    testname = "xmaApp_smoke_test"
    f = open(current_dir + "/" + pf + ver + "_" + testname + ".log", "w")
    f.write("Number of Smoketests completed            : "+str(runNumber-1) +"\n")
    f.write("Number of failures in Scale tests         : "+str(failS) +"\n")
    f.write("Number of failures in Encode tests        : "+str(failE) +"\n")
    f.write("Number of failures in Scale + Encode tests: "+str(failSE) +"\n")
    f.write("Number of failures in Decode tests        : "+str(failD) +"\n")
    f.write("Number of failures in Transcode tests     : "+str(failT) +"\n")
    f.close()

    print("")
    print("Number of Smoketests completed            : "+str(runNumber-1))
    print("")
    print("Number of failures in Scale tests         : "+str(failS))
    print("Number of failures in Encode tests        : "+str(failE))
    print("Number of failures in Scale + Encode tests: "+str(failSE))
    print("Number of failures in Decode tests        : "+str(failD))
    print("Number of failures in Transcode tests     : "+str(failT))


def destroy():
	# Release resource
    print("Exiting ...")
    
def parse_options():
    parser = OptionParser()
    parser.add_option("-f", "--files",
                      dest = "nrfiles",
                      help = "#files to generate per test",
                      type = "int",
                      action = "store"
    )
    parser.add_option("-t", "--tmpdir",
                      dest = "tmpdir",
                      help = "directory for storing temporary YUV and mp4 files" \
                             "(best to keep the dir local on the machine for speed)",
                      type = "string",
                      action = "store"
    )
    parser.add_option("-l", "--logdir",
                      dest = "logdir",
                      help = "directory for log files",
                      type = "string",
                      action = "store"
    )
    parser.add_option("-r", "--removefiles",
                      dest = "removefiles",
                      help = "remove files after completion of all tests (yes/no): default = yes",
                      type = "string", 
                      action = "store", 
                      default = "yes"
    )
    parser.add_option("-i", "--iterations",
                      dest = "iterations",
                      help = "number of iterations to run (0 = continuous): default = 1",
                      type = "int", 
                      action = "store", 
                      default = "1"
    )
    parser.add_option("-m", "--minutes",
                      dest = "minutes",
                      help = "number of minutes to run (0 = ignore setting): default = 0",
                      type = "int", 
                      action = "store", 
                      default = "0"
    )
    parser.add_option("-p", "--pictures",
                      dest = "frames",
                      help = "number of video frames per test: default = 255",
                      type = "int", 
                      action = "store", 
                      default = 255                                            
    )
    parser.add_option("-q", "--quit",
                      dest = "quit",
                      help = "quit at failure: default = no; other options;yes, scale, encode, scaleencode, decode, transcode",
                      type = "string", 
                      action = "store", 
                      default = "no"                                            
    )
    (options, args) = parser.parse_args()
    if options.nrfiles and options.tmpdir and options.logdir:
        return (options.nrfiles, options.tmpdir, options.logdir, options.removefiles, options.iterations, options.minutes, options.frames, options.quit)
    else:
        parser.print_help()
        raise SystemExit(1)

if __name__ == '__main__':
	try:
		main()
	# When 'Ctrl+C' is pressed, the child program 
	# destroy() will be executed.
	except KeyboardInterrupt:
		destroy()

