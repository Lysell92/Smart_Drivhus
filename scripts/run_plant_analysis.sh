#!/bin/bash

IMAGE="$1"

cd /home/drivhus-user2/plantCVCode || exit 1

source /home/drivhus-user2/plantCVCode/.venv/bin/activate

python3 plantcv_pipeline.py "$IMAGE" --output-dir output --mqtt
