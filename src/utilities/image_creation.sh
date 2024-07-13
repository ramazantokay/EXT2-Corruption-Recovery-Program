#!/bin/sh
echo "Script Name: $0"
echo "First Argument: $1"

dd if=/dev/zero of=$1 bs=1M count=2048
mke2fs -t ext2 -b 1024 -N 1000000 $1
dumpe2fs $1
