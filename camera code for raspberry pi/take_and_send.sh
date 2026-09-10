#!/bin/bash

SERVER="drivhus-user2@192.168.103.11"
SERVER_FOLDER="/home/drivhus-user2/camera-images"
KEY="/home/user/.ssh/camera_upload_key"

LOCAL_FOLDER="/home/user/camera-upload/images"

mkdir -p "$LOCAL_FOLDER"

TIMESTAMP=$(date +"%Y-%m-%d_%H-%M-%S")
FILENAME="plant_${TIMESTAMP}.jpg"
LOCAL_FILE="${LOCAL_FOLDER}/${FILENAME}"

echo "Taking picture: $FILENAME"

/usr/bin/rpicam-still \
    --nopreview \
    --width 1640 \
    --height 1232 \
    --output "$LOCAL_FILE"

if [ $? -ne 0 ]; then
    echo "ERROR: Camera capture failed"
    exit 1
fi

echo "Sending picture to Debian server..."

scp \
    -i "$KEY" \
    "$LOCAL_FILE" \
    "${SERVER}:${SERVER_FOLDER}/"

if [ $? -eq 0 ]; then
    echo "Upload successful"
    echo "Updating latest.jpg..."

    ssh \
        -i "$KEY" \
        "$SERVER" \
        "cd '$SERVER_FOLDER' && ln -sfn '$FILENAME' latest.jpg"

    if [ $? -eq 0 ]; then
        echo "latest.jpg updated to: $FILENAME"
    else
        echo "ERROR: Could not update latest.jpg"
        exit 1
    fi

    rm "$LOCAL_FILE"
    echo "Local picture removed"
else
    echo "ERROR: Upload failed"
    exit 1
fi
