#!/usr/bin/python3
# -*- coding:utf-8 -*-
import pygame.camera
from astral import LocationInfo
from astral.sun import sun
import astral
from datetime import datetime
from time import sleep
import pytz
import paho.mqtt.publish as publish
import psutil
import string
import random
import requests
import json

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

city = LocationInfo(10.8333, 106.6667)
api_key = "389579cb0a02784fb378a926f0a31766"
lat = "16.075239"
lon = "108.224136"
string.alphanum = '1234567890avcdefghijklmnopqrstuvwxyzxABCDEFGHIJKLMNOPQRSTUVWXYZ'

# The ThingSpeak Channel ID.
# Replace <YOUR-CHANNEL-ID> with your channel ID.
channelID = "<YOUR-CHANNEL-ID>"

# The write API key for the channel.
# Replace <YOUR-CHANNEL-WRITEAPIKEY> with your write API key.
writeAPIKey = "<YOUR-CHANNEL-WRITEAPIKEY> "

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
    s = sun(city.observer)
    time_now = utc.localize(datetime.now())    
    sunrise =s["sunrise"]
    print("Sunrise is",sunrise)
    dusk = s["dusk"]
    print("Sunset is ",dusk)
    print("Time is", time_now)
    if time_now > sunrise and time_now < dusk :
        print("Daylight")
        # Capturing an image.
        image = cam.get_image()

        # Displaying the image on the window starting from the top-left corner.
        window.blit(image, (0, 0))

        # Refreshing the window.
        pygame.display.update()

        # Saving the captured image.
        created_at = datetime.now()
        pygame.image.save(window, '/home/pi/Documents/python/capture/image_' + str(created_at) + '.jpg')
        
    clientID = ''

# Create a random clientID.
    for x in range(1,16):
        clientID += random.choice(string.alphanum)
        
    url = "https://api.openweathermap.org/data/2.5/onecall?lat=%s&lon=%s&appid=%s&units=metric" % (lat, lon, api_key)
    response = requests.get(url)
    data = json.loads(response.text)
    #print(data)
    current_temp = data["current"]["temp"]
    current_humid = data["current"]["humidity"]
    current_press = data["current"]["pressure"]
    visibulity = data["current"]["visibility"]
    clouds = data["current"]["clouds"]
    print("Temp is",current_temp)
    print("Humidity is",current_humid)
    print("Pressure is",current_press)
    print("Visibility is",visibulity )
    print("Clouds are",clouds)

    # build the payload string.
    payload = "field1=" + str(current_temp) + "&field2=" + str(current_humid) + "&field3=" + str(current_press)+ "&field4=" + str(visibulity)+ "&field5=" + str(clouds)

    # attempt to publish this data to the topic.
    try:
        publish.single(topic, payload, hostname=mqttHost, transport=tTransport, port=tPort,auth={'username':mqttUsername,'password':mqttAPIKey})
        print (" Published  ")
    except:
        print ("There was an error while publishing the data.")
        sleep(60*10)
    else:
        print("Sleep for 10 minutes")
        sleep(60*10)
	        
   
# Stopping the camera.

cam.stop()
