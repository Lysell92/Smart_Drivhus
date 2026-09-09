#!/bin/bash

WATCH_DIR="/home/drivhus-user2/camera-images"
RUN_SCRIPT="/home/drivhus-user2/run_plant_analysis.sh"

inotifywait -m -e close_write --format '%w%f' "$WATCH_DIR" |
	while read IMAGE
	do
		case "$IMAGE" in
			*.jpg|*.jpeg|*.JPG|*.JPEG)
		echo "New image: $IMAGE"
		"$RUN_SCRIPT" "$IMAGE"
		;;
	esac
done
