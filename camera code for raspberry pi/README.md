# Raspberry Pi Greenhouse Camera

This Raspberry Pi camera setup is used for the Smart Greenhouse project.

## What it does
- Takes a picture with the Raspberry Pi camera.
- Adds a timestamp to the filename.
- Uploads the image to the Debian server with SCP over SSH.
- Updates `latest.jpg` on the Debian server.
- Deletes the temporary local image after a successful upload.
- Runs automatically every 30 minutes using cron.

## Important paths
- Script: `/home/user/camera-upload/take_and_send.sh`
- Temporary images: `/home/user/camera-upload/images/`
- Log: `/home/user/camera-upload/camera.log`
- Upload key: `/home/user/.ssh/camera_upload_key`
- Debian destination: `/home/drivhus-user2/camera-images/`

## Setup

Make the script executable:

```bash
chmod +x /home/user/camera-upload/take_and_send.sh
```

Test it:

```bash
/home/user/camera-upload/take_and_send.sh
```

Check the camera:

```bash
rpicam-hello --list-cameras
```

Edit cron:

```bash
crontab -e
```

Add:

```cron
*/30 * * * * /home/user/camera-upload/take_and_send.sh >> /home/user/camera-upload/camera.log 2>&1
```

Check cron:

```bash
crontab -l
```

Watch logs:

```bash
tail -f /home/user/camera-upload/camera.log
```

## Security
Do not upload SSH private keys, generated camera images, logs, or `.env` files to GitHub.
