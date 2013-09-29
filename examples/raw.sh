#!/bin/sh

# default variables
fname=rec

# set file name
if [ $# -ge 1 ] ; then
fname=$1
fi

# create fifo
rm -f /tmp/$fname.fifo
mkfifo /tmp/$fname.fifo

# spwan raw 
sox  -t raw -r 22050 -s -w - -t raw /tmp/$fname.raw  </tmp/$fname.fifo 3>/tmp/$fname.fifo &

# remember process id for killing
pid=$!

# trap exit signal
trap 'kill $pid ; rm -f /tmp/$fname.fifo ; exit'  TERM INT

# waiting to signal
while true; do
sleep 1
done 
