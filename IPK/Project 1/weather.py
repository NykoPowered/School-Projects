# IPK
# 1. Projekt
# Nikolas Masica, xmasic00

import socket
import json
import sys


api_key = sys.argv[1]
city = sys.argv[2]
MYAPIKEY= 'dd1d6bf4780025948e8aaf193edf55be'
PORT = 80
HOST = 'api.openweathermap.org'
GET_URL = "GET /data/2.5/weather?q="+city +"&APPID="+api_key+"&units=metric\r\n HTTP/1.1\r\n"
GET_URL_ENCODE = GET_URL.encode()
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  #vytvorenie socket
    s.connect((HOST, PORT))                              #pripojenie na HOST
except socket.error:                            #chybove hlasenie ak sa nepodarilo pripojit na HOST
    print("Failed to connect\n")
    sys.exit();
try:
    s.sendall(GET_URL_ENCODE)                   #GET request
    received_data = s.recv(4096)                #ulozenie odpovedi zo servera do premennej
except socket.error:                            #chybove hlasenie
    print("Sendall failed\n")
    sys.exit();


try:
    decoded_data = received_data.decode()           #decode obdrzanych dat
    jdata = json.loads(decoded_data)                    #spracovanie dat json-om
except socket.error:
    printf("json failed\n")
    sys.exit();
##############################################################
#ziskanie jednotlivych informacii z odpovedi zo servera
##############################################################
if jdata.get('name'):
	city=jdata.get('name')
else:
	print("WRONG CITY OR API KEY\n")
	sys.exit();
country=jdata.get('sys').get('country')
temp=jdata.get('main').get('temp')
humidity=jdata.get('main').get('humidity')
pressure=jdata.get('main').get('pressure')
sky=jdata['weather'][0]['description']
wind_speed=jdata.get('wind').get('speed')
wind_speed = wind_speed * 3.6
if jdata.get('wind').get('deg'):                            ##wind degree niekedy nie je obdrzane v odpovedi zo sevrera preto v takomto pripade sa namiesto hodnoty vypise "Not measured"
    wind_deg = jdata.get('wind').get('deg')
else:
    wind_deg = "Not measured"
####################################
#VYPIS INFORMACII	
###################################
print("Current weather in: "f'{city}' ", " f'{country}')
print(f'{sky}')
print("Temperature:", f'{temp}'"\xb0" + "C")
print("Humidity:", f'{humidity}'"%")
print("Pressure:", f'{pressure}',"hPa")
print("Wind-speed:",f'{wind_speed}',"km/h")
print("Wind-degree: " f'{wind_deg}')




