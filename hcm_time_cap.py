#!/usr/bin/env python
# -*- coding:utf-8 -*-
import pygame.camera
from astral import LocationInfo
from astral.sun import sun
import astral
from datetime import datetime
from time import sleep
from datetime import datetime, timedelta
from pytz import timezone
import pytz

# Captured image dimensions. It should be less than or equal to the maximum dimensions acceptable by the camera.

width = 320

height = 240
# Initializing PyGame and the camera.

pygame.init()

pygame.camera.init()

# Specifying the camera to be used for capturing images. If there is a single camera, then it has the index 0.
cam = pygame.camera.Camera("/dev/video0", (width, height))

# Preparing a resizable window of the specified size for displaying the captured images.
window = pygame.display.set_mode((width, height), pygame.RESIZABLE)

# Starting the camera for capturing images.
cam.start()

utc =pytz.UTC
#asiatim=timezone('Asia/Ho_Chi_Minh')
#fmt = '%Y-%m-%d %H:%M:%S %Z%z'
city = LocationInfo(10.8333, 106.6667)
while True:
    s = sun(city.observer)
    loc_dt = asiatim.localize(datetime.now())
    time_hcm = loc_dt.strftime(fmt)
    #time_now = utc.localize(datetime.now())
    sunrise =s["sunrise"]
    dusk = s["dusk"]
    print("Sunrise is",sunrise)
    print("Sunset is",dusk())
    print("Time now is",time_hcm)
    
    #if time_hcm > sunrise and time_now < dusk :
    if time_hcm > sunrise and time_hcm < dusk :
        print("Daylight")
        # Capturing an image.
        image = cam.get_image()

        # Displaying the image on the window starting from the top-left corner.
        window.blit(image, (0, 0))

        # Refreshing the window.
        pygame.display.update()

        # Saving the captured image.
        created_at = datetime.now()
        pygame.image.save(window, './capture/image_' + str(created_at) + '.jpg')
        print("Sleep for 10 minutes")
        sleep(60*10)
    else:
        print("No image taken sleep for 10 minutes")
        sleep(60*10)
	        
   
# Stopping the camera.

cam.stop()
