# Test weather data
# https://openweathermap.org/

import requests

#http://api.openweathermap.org/data/2.5/forecast?id=524901&appid={API key}
api_url = "http://api.openweathermap.org/data/2.5/forecast?"
api_loc = "zip=03110,US"
api_unit = "units=imperial"
api_key = "appid=61d761fc40f988a6549265538987c0b5"
req_para = '&'.join([api_loc, api_key, api_unit])
req_url = api_url+req_para

print(req_url)
response = requests.post(req_url)
print(response.json())
