# WordClock
[![Version](https://img.shields.io/badge/version-4.2.3-green.svg?style=for-the-badge)](#) [![mantained](https://img.shields.io/maintenance/yes/2020.svg?style=for-the-badge)](#)

[![maintainer](https://img.shields.io/badge/maintainer-Goran%20Zunic%20%40panbachi-blue.svg?style=for-the-badge)](https://www.panbachi.de)

**Font and german layout based on https://www.mikrocontroller.net/articles/Word_Clock**

# Requirements

## Healthcheck
* [FastLED](https://github.com/FastLED/FastLED) 3.5.0 or newer (older versions make the first LED flicker with the ESP8266 core 3.x)

## WordClock
* [ESP8266 Arduino Core](https://github.com/esp8266/Arduino) 3.x
* [FastLED](https://github.com/FastLED/FastLED) 3.5.0 or newer (older versions make the first LED flicker with the ESP8266 core 3.x)
* [WifiManager](https://github.com/tzapu/WiFiManager)
* [ArduinoJson](https://github.com/bblanchon/ArduinoJson) 6.x

`NTPClient` is no longer needed: time sync and time zone handling use the SNTP client built into the ESP8266 core.

# Assemble
[![Word-Clock - Die WLAN Wort-Uhr zum selber bauen](https://img.youtube.com/vi/FvAM1t0tISE/0.jpg)](https://www.youtube.com/watch?v=FvAM1t0tISE)

# Customizations
## Modify `user_config.h` to change config
| Key                 | Values                                                           | Comment                  |
|---------------------|------------------------------------------------------------------|--------------------------|
| GRID_ROWS           | 10<br>11                                                         | count of rows            |
| GRID_COLS           | 11                                                               | count of cols            |
| GRID_FIRST          | 0: top-left<br>1: top-right<br>2: buttom-left<br>3: buttom-right | position of first led    |
| GRID_LANGUAGE       | de_DE: german<br>de_DE_alt: german alternative                   | langauge                 |
| GRID_SINGLE_MINUTES | 0: before<br>1: after                                            | position of minutes leds |
| TZ_DEFAULT          | POSIX TZ rule, default `CET-1CEST,M3.5.0,M10.5.0/3`              | initial time zone        |
| OTA_PASSWORD        | any string, default empty (no password)                          | protects firmware updates |
| OTA_HOSTNAME        | default `wordclock`                                              | network name for updates |

# Installation
Make your Arduino ready for Wemos D1 mini e.g. https://makesmart.net/blog/read/esp8266-d1-mini-programmieren-der-start-mit-der-arduino-ide
- Change the grid settings in the `wordclock/user_config.h`.
- Upload the sketch `wordclock/wordclock.ino` to your Wemos D1 mini (or other ESP8266).
- Enjoy

# Usage
The WordClock creates a WiFi-Hotspot with the name "WordClock". Connect e.g. your smartphone to the hotspot and you will be forwared to the config page, where you can set the WiFi credentials.
Check in your router what IP-adress your WordClock got. Open a webbrowser and go to `http://[YOUR_WORDCLOCK_IP]`. For (example) the IP-adress `192.168.178.20` go to `http://192.168.178.20`.

# Functions

## Set color
Choose a color mode, the foreground and background color and the brightness.

| Color mode  | Effect |
|-------------|--------|
| Einfarbig   | all words in the foreground color |
| Farbwechsel | the whole clock slowly cycles through the color spectrum |
| Regenbogen  | a rainbow moves diagonally over the letters |
| Farbverlauf | gradient from the foreground color (top) to the second color (bottom) |
| Farbwelle   | waves of the foreground and second color move over the letters |

The speed (1-10) sets how long one color cycle takes: from 10 minutes (1) to 6 seconds (10). The scrolling text uses the same color mode.

## Special occasions
On up to six dates per year (birthdays, anniversaries, ...) the clock shows a picture (birthday cake, Christmas tree or heart) alternating with the time, followed by an optional scrolling text such as "Alles Gute, Anna!". The candles flicker, the balls on the tree twinkle and the heart beats.

The Christmas tree is shown automatically from the 1st Advent to December 26th, with "Frohe Weihnachten" from December 24th (can be switched off). A birthday on the same day takes priority. Use *Vorschau* to see a picture right away. Occasions are not shown during the night mode (DND).

## Scrolling text
Enter a text, choose how often it repeats (or until *Stopp*) and the speed. Letters, digits, German umlauts, ß, € and ♥ are supported; up to 100 bytes.

The text can also be sent by other systems, e.g. Home Assistant:

```
PUT http://[YOUR_WORDCLOCK_IP]/api/text      {"text": "Essen ist fertig!", "repeat": 2, "speed": 2}
DELETE http://[YOUR_WORDCLOCK_IP]/api/text   (stops text and pictures)
PUT http://[YOUR_WORDCLOCK_IP]/api/preview   {"symbol": 1, "text": "Alles Gute!"}   (1 = cake, 2 = tree, 3 = heart)
```

## Set time zone
Choose your time zone from the list or enter your own [POSIX TZ rule](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv). The clock switches between daylight saving time and standard time by itself; no internet service besides the NTP server is needed. The time tab shows the clock's current time so you can check the setting.

Configs from older firmware versions only contain a fixed UTC offset. After updating, the clock starts with `TZ_DEFAULT` (Central European Time) until you save another time zone.

## Firmware update over WiFi
The first installation has to be done via USB. After that, new firmware can be installed over WiFi:

* **Web interface:** In the Arduino IDE choose *Sketch > Export Compiled Binary*. Open the *Firmware* tab of the web interface (or `http://[YOUR_WORDCLOCK_IP]/firmware`) and upload `wordclock.ino.bin`. The LEDs stay as they are during the upload, then the clock restarts.
* **Arduino IDE:** Choose the network port `wordclock` under *Tools > Port* and upload as usual. The LEDs fill up blue to show the progress.

Settings are kept as long as you compile with the same *Flash Size* option (e.g. *4MB (FS:2MB OTA:~1019KB)* for the Wemos D1 mini). Set `OTA_PASSWORD` in `user_config.h` to protect updates; the web upload then asks for user `admin` and this password. If mDNS works in your network, the clock is also reachable as `http://wordclock.local`.

## Set DND
You can set a time span in which the clock should be switched off.

## PWA
You can use the webbrowser function "Add to start screen" to install the webinterface as an app to your android (and possibly ios) smartphone.

# Create own grid-layout
To create your own grid-layout visit the Wiki: [Create Grid-Layout](https://github.com/panbachi/wordclock/wiki/Create-Grid-Layout)

# Support me / Follow me
[![Web](https://img.shields.io/badge/www-panbachi.de-blue.svg?style=flat-square&colorB=3d72a8&colorA=333333)](https://www.panbachi.de)
[![Facebook](https://img.shields.io/badge/-%40panbachi.de-blue.svg?style=flat-square&logo=facebook&colorB=3B5998&colorA=333333)](https://www.facebook.com/panbachi.de/)
[![Twitter](https://img.shields.io/badge/-%40panbachi.de-blue.svg?style=flat-square&logo=twitter&colorB=1DA1F2&colorA=333333)](https://twitter.com/panbachi)
[![Instagram](https://img.shields.io/badge/-%40panbachi.de-blue.svg?style=flat-square&logo=instagram&colorB=E4405F&colorA=333333)](http://instagram.com/panbachi.de)
[![YouTube](https://img.shields.io/badge/-%40panbachi.de-blue.svg?style=flat-square&logo=youtube&colorB=FF0000&colorA=333333)](https://www.youtube.com/channel/UCO7f2L7ZsDCpOtRfKnPqNow)
