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

#!/bin/bash

if [ ! $# -eq 2 ]
then
  echo "Usage:"
  echo "  run.sh <log file> <pid>"
  exit 1
fi

rm *.log 

dec=`cat $1 | grep dec_handle | grep $2 | awk {'print $8'}`
sc=`cat $1 | grep scaler_handle | grep $2 | awk {'print $8'}`
la1=`cat $1 | grep la_handle | grep $2 | awk {'print $8'} | awk 'NR==1{print $1}'`
enc1=`cat $1 | grep enc_handle | grep $2 | awk {'print $8'} | awk 'NR==1{print $1}'`
la2=`cat $1 | grep la_handle | grep $2 | awk {'print $8'} | awk 'NR==2{print $1}'`
enc2=`cat $1 | grep enc_handle | grep $2 | awk {'print $8'} | awk 'NR==2{print $1}'`
la3=`cat $1 | grep la_handle | grep $2 | awk {'print $8'} | awk 'NR==3{print $1}'`
enc3=`cat $1 | grep enc_handle | grep $2 | awk {'print $8'} | awk 'NR==3{print $1}'`
la4=`cat $1 | grep la_handle | grep $2 | awk {'print $8'} | awk 'NR==4{print $1}'`
enc4=`cat $1 | grep enc_handle | grep $2 | awk {'print $8'} | awk 'NR==4{print $1}'`
la5=`cat $1 | grep la_handle | grep $2 | awk {'print $8'} | awk 'NR==5{print $1}'`
enc5=`cat $1 | grep enc_handle | grep $2 | awk {'print $8'} | awk 'NR==5{print $1}'`

echo "Ladder contexts =  $dec $sc $la1 $enc1 $la2 $enc2 $la3 $enc3 $la4 $enc4 $la5 $enc5"

echo " ================== Generating logs for $1 log file ========================= "

cat $1 | grep $dec | grep $2 | grep dec_frame_sent | awk {'print $13'} | grep -v kB > dec_send_$dec.log
cat $1 | grep $dec | grep $2 | grep dec_frame_recv | awk {'print $13'} | grep -v kB | grep -v yet > dec_recv_$dec.log

if [ ! -z $sc ]
then
cat $1 | grep $sc | grep $2 | grep scaler_frame_sent | awk {'print $13'} | grep -v kB > scaler_send_$sc.log
cat $1 | grep $sc | grep $2 | grep scaler_frame_recv | awk {'print $13'} | grep -v kB > scaler_recv_$sc.log
fi

if [ ! -z $la1 ]
then
cat $1 | grep $la1 | grep $2 | grep la_frame_sent | awk {'print $13'} | grep -v kB > lookahead_send_$la1.log
cat $1 | grep $la1 | grep $2 | grep la_frame_recv | awk {'print $13'} | grep -v kB > lookahead_recv_$la1.log
fi

cat $1 | grep $enc1 | grep $2 | grep enc_frame_sent | awk {'print $13'} | grep -v kB | grep -v allocated | grep -v pool > enc_send_$enc1.log
cat $1 | grep $enc1 | grep $2 | grep enc_frame_recv | awk {'print $13'} | grep -v kB > enc_recv_$enc1.log

if [ ! -z $la2 ]
then
cat $1 | grep $la2 | grep $2 | grep la_frame_sent | awk {'print $13'} | grep -v kB > lookahead_send_$la2.log
cat $1 | grep $la2 | grep $2 | grep la_frame_recv | awk {'print $13'} | grep -v kB > lookahead_recv_$la2.log
fi

if [ ! -z $enc2 ]
then
cat $1 | grep $enc2 | grep $2 | grep enc_frame_sent | awk {'print $13'} | grep -v kB | grep -v allocated | grep -v pool > enc_send_$enc2.log
cat $1 | grep $enc2 | grep $2 | grep enc_frame_recv | awk {'print $13'} | grep -v kB > enc_recv_$enc2.log
fi

if [ ! -z $la3 ]
then
cat $1 | grep $la3 | grep $2 | grep la_frame_sent | awk {'print $13'} | grep -v kB > lookahead_send_$la3.log
cat $1 | grep $la3 | grep $2 | grep la_frame_recv | awk {'print $13'} | grep -v kB > lookahead_recv_$la3.log
fi

if [ ! -z $enc3 ]
then
cat $1 | grep $enc3 | grep $2 | grep enc_frame_sent | awk {'print $13'} | grep -v kB | grep -v allocated | grep -v pool > enc_send_$enc3.log
cat $1 | grep $enc3 | grep $2 | grep enc_frame_recv | awk {'print $13'} | grep -v kB > enc_recv_$enc3.log
fi

if [ ! -z $la4 ]
then
cat $1 | grep $la4 | grep $2 | grep la_frame_sent | awk {'print $13'} | grep -v kB > lookahead_send_$la4.log
cat $1 | grep $la4 | grep $2 | grep la_frame_recv | awk {'print $13'} | grep -v kB > lookahead_recv_$la4.log
fi

if [ ! -z $enc4 ]
then
cat $1 | grep $enc4 | grep $2 | grep enc_frame_sent | awk {'print $13'} | grep -v kB | grep -v allocated | grep -v pool > enc_send_$enc4.log
cat $1 | grep $enc4 | grep $2 | grep enc_frame_recv | awk {'print $13'} | grep -v kB > enc_recv_$enc4.log
fi

if [ ! -z $la5 ]
then
cat $1 | grep $la5 | grep $2 | grep la_frame_sent | awk {'print $13'} | grep -v kB > lookahead_send_$la5.log
cat $1 | grep $la5 | grep $2 | grep la_frame_recv | awk {'print $13'} | grep -v kB > lookahead_recv_$la5.log
fi

if [ ! -z $enc5 ]
then
cat $1 | grep $enc5 | grep $2 | grep enc_frame_sent | awk {'print $13'} | grep -v kB | grep -v allocated | grep -v pool > enc_send_$enc5.log
cat $1 | grep $enc5 | grep $2 | grep enc_frame_recv | awk {'print $13'} | grep -v kB > enc_recv_$enc5.log
fi

echo " =============== Done generating logs! Measuring now... ======================="


let loop=`cat dec_recv_$dec.log | wc -l`

let line=0

echo "Frames decoded = " $loop

rm -f dec_lat_$dec.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line dec_send_$dec.log | tail -1`
	num2=`head -$line dec_recv_$dec.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> dec_lat_$dec.log

	let line=line+1
done

let ave=sum/line
echo $ave >> dec_lat_$dec.log
echo "Average decoding latency = " $ave "ms"

echo "============== decoder done ==============="

if [ ! -z $sc ]
then
let loop=`cat scaler_recv_$sc.log | wc -l`

let line=0

echo "Frames scaled = " $loop

rm -f scaler_lat_$sc.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line scaler_send_$sc.log | tail -1`
	num2=`head -$line scaler_recv_$sc.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> scaler_lat_$sc.log

	let line=line+1
done
let ave=sum/line
echo $ave >> scaler_lat_$sc.log
echo "Average scaling latency = " $ave "ms"

echo "============== scaler done ==============="
fi

if [ ! -z $la1 ]
then
let loop=`cat lookahead_recv_$la1.log | wc -l`

let line=0

echo "Frames processed = " $loop

rm -f lookahead_lat_$la1.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line lookahead_send_$la1.log | tail -1`
        num2=`head -$line lookahead_recv_$la1.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> lookahead_lat_$la1.log

        let line=line+1
done
let ave=sum/line
echo $ave >> lookahead_lat_$la1.log
echo "Average lookahead latency = " $ave "ms"

echo "============== lookahead 1 done ==============="
fi

let loop=`cat enc_recv_$enc1.log | wc -l`

let line=0

echo "Frames encoded = " $loop

rm -f enc_lat_$enc1.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line enc_send_$enc1.log | tail -1`
	num2=`head -$line enc_recv_$enc1.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> enc_lat_$enc1.log

	let line=line+1
done
let ave=sum/line
echo $ave >> enc_lat_$enc1.log
echo "Average encoding latency = " $ave "ms"

echo "============== encoder 1 done ============="

if [ ! -z $la2 ]
then
let loop=`cat lookahead_recv_$la2.log | wc -l`

let line=0

echo "Frames processed = " $loop

rm -f lookahead_lat_$la2.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line lookahead_send_$la2.log | tail -1`
        num2=`head -$line lookahead_recv_$la2.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> lookahead_lat_$la2.log

        let line=line+1
done
let ave=sum/line
echo $ave >> lookahead_lat_$la2.log
echo "Average lookahead latency = " $ave "ms"

echo "============== lookahead 2 done ==============="
fi

if [ ! -z $enc2 ]
then
#disable below exit for ladders
#exit

let loop=`cat enc_recv_$enc2.log | wc -l`

let line=0

echo "Frames encoded = " $loop

rm -f enc_lat_$enc2.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line enc_send_$enc2.log | tail -1`
	num2=`head -$line enc_recv_$enc2.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> enc_lat_$enc2.log

	let line=line+1
done
let ave=sum/line
echo $ave >> enc_lat_$enc2.log
echo "Average encoding latency = " $ave "ms"

echo "============== encoder 2 done ==============="
fi

if [ ! -z $la3 ]
then
let loop=`cat lookahead_recv_$la3.log | wc -l`

let line=0

echo "Frames processed = " $loop

rm -f lookahead_lat_$la3.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line lookahead_send_$la3.log | tail -1`
        num2=`head -$line lookahead_recv_$la3.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> lookahead_lat_$la3.log

        let line=line+1
done
let ave=sum/line
echo $ave >> lookahead_lat_$la3.log
echo "Average lookahead latency = " $ave "ms"

echo "============== lookahead 3 done ==============="
fi

if [ ! -z $enc3 ]
then
let loop=`cat enc_recv_$enc3.log | wc -l`

let line=0

echo "Frames encoded = " $loop

rm -f enc_lat_$enc3.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line enc_send_$enc3.log | tail -1`
	num2=`head -$line enc_recv_$enc3.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> enc_lat_$enc3.log

	let line=line+1
done
let ave=sum/line
echo $ave >> enc_lat_$enc3.log
echo "Average encoding latency = " $ave "ms"

echo "============== encoder 3 done ==============="
fi

if [ ! -z $la4 ]
then
let loop=`cat lookahead_recv_$la4.log | wc -l`

let line=0

echo "Frames processed = " $loop

rm -f lookahead_lat_$la4.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line lookahead_send_$la4.log | tail -1`
        num2=`head -$line lookahead_recv_$la4.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> lookahead_lat_$la4.log

        let line=line+1
done
let ave=sum/line
echo $ave >> lookahead_lat_$la4.log
echo "Average lookahead latency = " $ave "ms"

echo "============== lookahead 4 done ==============="
fi

if [ ! -z $enc4 ]
then

let loop=`cat enc_recv_$enc4.log | wc -l`

let line=0

echo "Frames encoded = " $loop

rm -f enc_lat_$enc4.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line enc_send_$enc4.log | tail -1`
	num2=`head -$line enc_recv_$enc4.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> enc_lat_$enc4.log

	let line=line+1
done
let ave=sum/line
echo $ave >> enc_lat_$enc4.log
echo "Average encoding latency = " $ave "ms"

echo "============== encoder 4 done ==============="
fi

if [ ! -z $la5 ]
then
let loop=`cat lookahead_recv_$la5.log | wc -l`

let line=0

echo "Frames processed = " $loop

rm -f lookahead_lat_$la5.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line lookahead_send_$la5.log | tail -1`
        num2=`head -$line lookahead_recv_$la5.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> lookahead_lat_$la5.log

        let line=line+1
done
let ave=sum/line
echo $ave >> lookahead_lat_$la5.log
echo "Average lookahead latency = " $ave "ms"

echo "============== lookahead 5 done ==============="
fi

if [ ! -z $enc5 ]
then

let loop=`cat enc_recv_$enc5.log | wc -l`

let line=0

echo "Frames encoded = " $loop

rm -f enc_lat_$enc5.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
	num1=`head -$line enc_send_$enc5.log | tail -1`
	num2=`head -$line enc_recv_$enc5.log | tail -1`
	let num=num2-num1
	let sum=sum+num
	echo $num1 $num2 $num >> enc_lat_$enc5.log

	let line=line+1
done
let ave=sum/line
echo $ave >> enc_lat_$enc5.log
echo "Average encoding latency = " $ave "ms"

echo "============== encoder 5 done ==============="
fi

let loop1=`cat dec_send_$dec.log | wc -l`
let loop2=`cat enc_recv_$enc1.log | wc -l`
if [ $loop1 -lt $loop2 ]
then
  let loop=$loop1
else
  let loop=$loop2
fi

let line=0

echo "Total frames encoded = " $loop

rm -f total_lat.log
let line=1
let sum=0
for (( c=1; c<=$loop; c++ ))
do
        num1=`head -$line dec_send_$dec.log | tail -1`
        num2=`head -$line enc_recv_$enc1.log | tail -1`
        let num=num2-num1
        let sum=sum+num
        echo $num1 $num2 $num >> total_lat.log

        let line=line+1
done
let ave=sum/line
echo $ave >> total_lat.log
echo "Total average latency = " $ave "ms"

rm *.log
