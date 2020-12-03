
#!/usr/bin/env python
# -*- coding:utf-8 -*-
import csv
# import standard python modules.
from time import sleep, strftime, time
from datetime import datetime
# import adafruit dht library.
import Adafruit_DHT
from gpiozero import CPUTemperature
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

# headers you want in the order you want
#headers = ['Created at','CPU temp','Local temp','Local humidity']

with open('capture/write.csv','w') as csvfile:
		while True:
			#writer.writeheader()
			humidity, temperature = Adafruit_DHT.read_retry(dht22_sensor, DHT_DATA_PIN)
			cpu = CPUTemperature()
			temp = cpu.temperature
			created_at = datetime.datetime.now()
			print("Time is",created_at," CPU Temp=",int(temp),"C ","Room temp=",int(temperature),"C ","Room humidity=",int(humidity),"%")
			os.system('fswebcam -r 320x240 -S 3 --jpeg 75 --no-banner --save /home/pi/Documents/python/capture/%Y-%m-%d-%H:%M.jpg')
			#datawriter = csv.writer(csvfile, delimiter=' ',quotechar='|', quoting=csv.QUOTE_MINIMAL)
			#writer = csv.DictWriter(
			#csvfile, fieldnames=['Created at','CPU temp','Local temp','Local humidity'])
			#writer.writeheader()
			csvfile.write(str(created_at)+','+str(temp)+','+str(temperature)+','+str(humidity)+ "\n")
			#csvfile.close()
			print("Waiting 10 minutes")
			sleep(10*60)
