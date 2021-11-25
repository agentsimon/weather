import requests
import json

api_key = "YouKey form OpenWeather"
lat = "10.075239"
lon = "108.224136"
location = "Turan"


url = "https://api.openweathermap.org/data/2.5/weather?q={}&units=metric&appid={}".format(location, api_key)
response = requests.get(url)
data = json.loads(response.text)
print(data)
print("Temp is ", data["main"]["temp"])
sunrise = data["sys"]["sunrise"]
dusk = data["sys"]["sunset"]
print(dusk, sunrise)
