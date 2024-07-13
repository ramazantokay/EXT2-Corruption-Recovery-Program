#!/bin/bash

MOUNT_POINT="/home/username/Desktop/fs-root"
TOTAL_FILES=2200000  # Adjust this number as needed to test indirect blocks

echo "Creating files to populate the root directory..."

for i in $(seq 800000 $TOTAL_FILES); do
    touch ${MOUNT_POINT}/f$i
    if (( $i % 10000 == 0 )); then
        echo "Created $i files"
        df -i ${MOUNT_POINT}  # Check inode usage
        df -h ${MOUNT_POINT}  # Check block usage
    fi
done

echo "Completed creating files."
