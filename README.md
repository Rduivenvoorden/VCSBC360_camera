# VCSBC360_camera
UTAT UAV -- Program that runs the Vision Components VCSBC360 camera for UAV payload purposes

UTAT UAV Division -- 2015-2016 
Rikky Duivenvoorden -- April 2016

This assumes current directory is "~/VCSBC360_camera/" from terminal

1. Establish connection with camera, start camera server program, initialize parameters and start live video mode
	>> python run_camera.py

2. Start client on Odroid, which requests images from camera and saves them as grayscale Bitmap images, NOT Bayer demosaicked
	>> ./img_xfer

The camera will then take up to 5000 pictures and populate the images folder.

uav@utat.skule.ca
