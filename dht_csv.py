#!/usr/bin/env python
# -*- coding:utf-8 -*-
import csv
# import standard python modules.
from time import sleep, strftime, time
from datetime import datetime
# import adafruit dht library.
import Adafruit_DHT
from subprocess import call
import subprocess
import os
import time
import datetime
# Delay in-between sensor readings, in seconds.
DHT_READ_TIMEOUT = 5

# Pin connected to DHT22 data pin
DHT_DATA_PIN = 26

# Set up DHT22 Sensor.
dht22_sensor = Adafruit_DHT.DHT22

def cam_capture():
    os.system('fswebcam -r 320x240 -S 3 --jpeg 75 --no-banner --save /home/pi/Documents/python/capture/%Y-%m-%d-%H:%M.jpg') 

with open('capture/write.csv', mode = 'w') as filecsv:
		while True:
			cam_capture()
			print("Camera captured and saved")
			humidity, temperature = Adafruit_DHT.read_retry(dht22_sensor, DHT_DATA_PIN)
			created_at = datetime.datetime.now()
			print("Time is",created_at,"Room temp=",int(temperature),"C ","Room humidity=",int(humidity),"%")
			headers = ['Created at','Local temp','Local humidity']
			writer = csv.DictWriter(filecsv,fieldnames=headers)
			writer.writeheader()
			writer.writerow({'Created at':created_at,'Local temp':temperature,'Local humidity':humidity})
			print("Waiting 10 minutes")
			sleep(10*60)
