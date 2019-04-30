# IPK - Počítačové komunikace a sítě
### Varianta 2: Klient pro OpenWeatherMap API
### Autor: Nikolas Masica, <xmasic00>
## Popis zadania:

1. Úlohou je vytvoriť program - **klienta rozhrania OpenWeatherMap**, ktorý bude schopný prostrednictvom HTTP requestov získavať vybrané informácie z dátového zdroja. Pre prístup je nutná bezplatná registrácia a získanie klúča, ktorý sa používa v requestoch pre autentizáciu. Výsledkom bude informácia o aktuálnom počasí a následujúce informácie: popis počasia, teplota, vlhkosť, tlak, rýchlost a smer vetra. Používajte metrické jednotky pri uvádzaní všetkych hodnôt.

## Konvencia mena klientskej aplikácie a jej povinných vstupných parametrov
```bash
make run api_key=<API klúč> city=<Mesto>
```
- **API_klúč** - api klúč použivateľa, ktorý obdržíte pri registrácii na stránke openweathermap.org.
- **Mesto** - Názov mesta. V prípade viacslovného názvu mesta (napr. New York) uvádzajte názov mesta štýlom `city="New York"`

## Popis riešenia

- Projekt používa knižnice `socket, json, sys`.
- Vytvoril som si socket pomocou knižnice `socket`. Poslal `GET` request serveru a obdržané informácie uložil do premennej
- Jednotlivé informácie o počasí z `GET` requestu získam pomocou knižnice `json` a funkcie `jdata.get()`
- Pri riešení projektu som čerpal z prednášok IPK, dsotupných v informačnom systéme


## Ukážka spustenia
```bash
make run api_key=dd1d6bf4780025948e8aaf193edf55be city=Brno
```
##### Výstup
```bash
Current weather in: Brno, CZ
clear sky
Temperature: 9.14°C
Humidity: 43%
Pressure: 1010 hPa
Wind-speed: 6.2 km/h
Wind-degree: 240
```

##### Výstup v prípade, že chýba nejaka meraná informácia
Namiesto chýbajúcej informácie sa vypíše **"Not Measured"**
```bash
Current weather in: Stebník, SK
broken clouds
Temperature: 4.26°C
Humidity: 67%
Pressure: 978 hPa
Wind-speed: 1.016 km/h
Wind-degree: Not Measured

```
##### Výstup(stderr)
V prípade, že bol zadaný zlý API kľúč alebo mesto
```bash
WRONG CITY OR API KEY
```
V prípade, že sa nepodarilo pripojiť na server
```bash
Failed to connect
```
V prípade, že sa nepodarilo obdržat odpoveď zo servera
```bash
Sendall failed
```
V prípade, že json nedokázal spracovať odpoveď zo servera
```bash
json failed
```

