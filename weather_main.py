#!/usr/bin/env python3
# Author: Andrew Mohnkern
# Project: Weather Arduino
#
#
# Used code from
# https://roboticsbackend.com/raspberry-pi-arduino-serial-communication/

import requests
import serial
import json
import time

# ARGUMENTS
# http://api.openweathermap.org/data/2.5/forecast?id=524901&appid={API_key}
api_url = "http://api.openweathermap.org/data/2.5/forecast?"
api_loc = "zip=03264,US"
api_unit = "units=imperial"
api_key = "appid=61d761fc40f988a6549265538987c0b5"
req_para = '&'.join([api_loc, api_key, api_unit])
req_url = api_url+req_para

# TEMPERATURE
def get_temperature():
	response = requests.post(req_url)
	data = response.json()
	temperature = data['list'][0]['main']['temp'] # in degrees F
	return temperature


# CONNECT
print("Weather python script start")
connected = False
while not connected:
	try:
		ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
		connected = True
		ser.flush()

	except:
		time.sleep(1)
		print("Trying again!")

print("Connected!")



# WAIT FOR REQUEST
time_count = 0

while True:

	try: 
		if ser.in_waiting > 0:
			line = ser.readline().decode('utf-8').rstrip()
			print("Received request")
			print(line)
			if line == "req_weather": # respond to weather request
				print("Replying")
				ser.write(str(get_temperature()).encode("utf-8"))

		# Awake poster
		#time.sleep(0.5)
		#time_count = time_count + 1
		#if time_count == 20:
		#	time_count = 0
		#	ser.write("awake".encode("utf-8"))
		#	print("Sent Awake Message")
	except OSError:
		print("Lost connection, retrying!")
		connected = False
		while not connected:
			try:
				ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
				connected = True
				ser.flush()
			except:
				time.sleep(1)
				print("Trying again!")
		print("Reconnected")
