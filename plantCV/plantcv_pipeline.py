import argparse
import json
from pathlib import Path
import cv2
import numpy as np

from plantcv import plantcv as pvc

# Configuration

# Image is 1536 * 1536
# Should possibly be adjusted afterwards

CROP_X1 = 0
CROP_Y1 = 0
CROP_X2 = 1536
CROP_Y2 = 1050

# HSV values
# Again just initial values - need possible adjustment
GREEN_H_MIN = 35
GREEN_H_MAX = 85
GREEN_S_MIN = 70
GREEN_V_MIN = 40

# Ignore isolated regions
MIN_COMPONENT_AREA = 100

# MQTT publishing
MQTT_ENABLED = True
MQTT_HOST = "localhost"
MQTT_PORT = 1883
MQTT_TOPIC = "plants/drivhusServer/plant_analysis"

def analyze_image(image_path, output_dir):

    """ 
    Analyze one greenhouse image.

    Returns a dictionary of measurements".
    """
    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    # Read image using OpenCV.

    image = cv2.imread(str(image_path))

    if image is None:
        raise ValueError(f"Could not read image: {image_path}")

    # Correct the upside-down camera
    image = cv2.rotate(image, cv2.ROTATE_180)

    # Crop to the part that should be analyzed
    crop = image[CROP_Y1:CROP_Y2, CROP_X1:CROP_X2]

    if crop.size == 0:
        raise ValueError("Crop is empty.")

    # Convert BGR --> HSV
    hsv = cv2.cvtColor(crop, cv2.COLOR_BGR2HSV)

    # Create a mask for green pixels
    lower_green = np.array(
            [GREEN_H_MIN, GREEN_S_MIN, GREEN_V_MIN],
            dtype=np.uint8
    )

    upper_green = np.array(
            [GREEN_H_MAX, 255, 255],
            dtype=np.uint8
    )

    mask = cv2.inRange(hsv, lower_green, upper_green)

# Remove small isolated regions.
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(
        mask,
        connectivity=8
    )

    cleaned_mask = np.zeros_like(mask)

    for label in range(1, num_labels):
        area = stats[label, cv2.CC_STAT_AREA]

        if area >= MIN_COMPONENT_AREA:
            cleaned_mask[labels == label] = 255

    # Count green pixels.
    green_pixels = int(cv2.countNonZero(cleaned_mask))

    total_pixels = int(cleaned_mask.shape[0] * cleaned_mask.shape[1])

    green_percentage = (
        green_pixels / total_pixels * 100
        if total_pixels > 0
        else 0
    )

    # Create a diagnostic image.
    # Green pixels are highlighted in red so you can inspect the mask.
    diagnostic = crop.copy()
    diagnostic[cleaned_mask > 0] = [0, 0, 255]

    # Draw the crop rectangle on the original image.
    original_with_crop = image.copy()

    cv2.rectangle(
        original_with_crop,
        (CROP_X1, CROP_Y1),
        (CROP_X2, CROP_Y2),
        (255, 0, 0),
        3
    )

    # Save diagnostic images.
    cv2.imwrite(
        str(output_dir / "crop_preview.jpg"),
        original_with_crop
    )

    cv2.imwrite(
        str(output_dir / "mask.jpg"),
        cleaned_mask
    )

    cv2.imwrite(
        str(output_dir / "diagnostic.jpg"),
        diagnostic
    )

    measurements = {
        "plant_id": "plant_1",
        "plant_area": green_pixels,
        "green_percentage": round(green_percentage, 2),
        "image": str(image_path)
    }

    return measurements


# --------------------------------------------------
# MQTT
# --------------------------------------------------

def publish_mqtt(measurements):
    """
    Publish measurements to MQTT.
    """

    import paho.mqtt.client as mqtt

    client = mqtt.Client()

    client.connect(MQTT_HOST, MQTT_PORT, 60)

    payload = json.dumps(measurements)

    client.publish(MQTT_TOPIC, payload)

    client.disconnect()


# --------------------------------------------------
# Main program
# --------------------------------------------------

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "image",
        help="Path to the greenhouse image"
    )

    parser.add_argument(
        "--output-dir",
        default="output",
        help="Directory for diagnostic images"
    )

    parser.add_argument(
        "--mqtt",
        action="store_true",
        help="Publish measurements to MQTT"
    )

    args = parser.parse_args()

    measurements = analyze_image(
        args.image,
        args.output_dir
    )

    print(json.dumps(measurements, indent=2))

    if args.mqtt:
        publish_mqtt(measurements)


if __name__ == "__main__":
    main()
