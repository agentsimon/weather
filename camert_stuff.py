#!/usr/bin/python3
# -*- coding:utf-8 -*-
import pygame.camera
import sys
import os
home_dir = os.path.expanduser("~")
sys.path.append(home_dir)
import time
from datetime import datetime
from time import sleep
import paho.mqtt.publish as publish
import psutil
import string
import random
import requests
import json


print ('\n'.join(sys.path))
# Captured image dimensions.

width = 320

height = 240
# Initializing PyGame and the camera.

pygame.init()

pygame.camera.init()

# Specifying the camera to be used for capturing images.
cam = pygame.camera.Camera("/dev/video0", (width, height))

# Preparing a resizable window
window = pygame.display.set_mode((width, height), pygame.RESIZABLE)

# Starting the camera for capturing images.
cam.start()



api_key = "389579cb0a02784fb378a926f0a31766"
lat = "10.075239"
lon = "108.224136"
location = "Turan"
string.alphanum = '1234567890avcdefghijklmnopqrstuvwxyzxABCDEFGHIJKLMNOPQRSTUVWXYZ'

# The ThingSpeak Channel ID.
# Replace <YOUR-CHANNEL-ID> with your channel ID.
channelID = "1260661"

# The write API key for the channel.
# Replace <YOUR-CHANNEL-WRITEAPIKEY> with your write API key.
writeAPIKey = "5QY4AJY5PMVZVVKJ"

# The hostname of the ThingSpeak MQTT broker.
mqttHost = "mqtt.thingspeak.com"

# You can use any username.
mqttUsername = "TSMQTTRpiDemo"

# Your MQTT API key from Account > My Profile.
mqttAPIKey = "JF5DL7X8LR749ZKG"

tTransport = "websockets"
tPort = 80
# Create the topic string.
topic = "channels/" + channelID + "/publish/" + writeAPIKey

while True:
  
    url = "https://api.openweathermap.org/data/2.5/weather?q={}&units=metric&appid={}".format(location, api_key)
    response = requests.get(url)
    data = json.loads(response.text)
    sunrise = data["sys"]["sunrise"]
    dusk = data["sys"]["sunset"]
    
    if time.time() > sunrise and time.time() < dusk:
        print("Daylight")
        # Capturing an image.
        image = cam.get_image()

        # Displaying the image on the window starting from the top-left corner.
        window.blit(image, (0, 0))

        # Refreshing the window.
        pygame.display.update()

        # Saving the captured image.
        created_at = datetime.now()
        pygame.image.save(window, '/home/pi/camera/capture/uploadimages/ image_' + str(created_at) + '.jpg')
    clientID = ''

# Create a random clientID.
    for x in range(1, 16):
        clientID += random.choice(string.alphanum)
    
    current_temp = data["main"]["temp"]
    current_humid = data["main"]["humidity"]
    current_press = data["main"]["pressure"]
    #visibility = data["main"]["visibility"]
    #clouds = data["main"]["clouds"]
    print("Temp is", current_temp)
    print("Humidity is", current_humid)
    print("Pressure is", current_press)
    #print("Visibility is", visibility)
    #print("Clouds are", clouds)
    print("Sunrise is ",data["sys"]["sunrise"])
    print("Sunset is ",data["sys"]['sunset'])
    print("Time is ",time.time())

    # build the payload string.
    payload = "field1=" + str(current_temp) + "&field2=" + str(current_humid) + "&field3=" + str(current_press)

    # attempt to publish this data to the topic.
    try:
        publish.single(topic, payload, hostname=mqttHost, transport=tTransport, port=tPort, auth={'username': mqttUsername, 'password': mqttAPIKey})
        print (" Published  ")
    except:
        print ("There was an error while publishing the data.")
        sleep(60*10)
    else:
        print("Sleep for 10 minutes")
        sleep(60*10)
        
# Stopping the camera.
cam.stop()
