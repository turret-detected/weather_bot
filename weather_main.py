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

# ARGUMENTS
#http://api.openweathermap.org/data/2.5/forecast?id=524901&appid={API key}
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


# WAIT FOR REQUEST
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
ser.flush()
print("Weather python script start")

while True:
	if ser.in_waiting > 0:
		line = ser.readline().decode('utf-8').rstrip()
		print("Received request")
		print(line)
		if line == "req_weather":
			#print("Received request")
			print("Replying")
			ser.write(str(get_temperature()).encode("utf-8"))
			# do the thing



