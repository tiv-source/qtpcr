#!/bin/sh

##################################
# QtPCR monitor script           #
# It records sound in mp3 format #
#                                #
# usuage:                        #
# mp3.sh filename                #     
#                                #
# input:                         #
# raw sound format rate 16000    #
# to /tmp/filename.fifo          #
#                                #
# output:                        #
# /tmp/filename.mp3              #                 
##################################

# default variables
fname=rec

# set file name
if [ $# -ge 1 ] ; then
fname=$1
fi

# create fifo
rm -f /tmp/$fname.fifo
mkfifo /tmp/$fname.fifo

# spwan mp3 encoder
lame -r -s 16 -b 16 - /tmp/$fname.mp3 </tmp/$fname.fifo 3>/tmp/$fname.fifo &

# remember process id for killing
pid=$!

# trap exit signal
trap 'kill $pid ; rm -f /tmp/$fname.fifo ; exit'  TERM INT

# waiting to signal
while true; do
sleep 1
done 
