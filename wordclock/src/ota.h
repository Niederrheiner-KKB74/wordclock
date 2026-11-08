#ifndef WORDCLOCK_OTA_H
#define WORDCLOCK_OTA_H

#include <Arduino.h>

/*
 * Firmware update over WiFi.
 *
 * - Web upload: GET /firmware shows an upload form, POST /update receives
 *   the .bin file (handled by ESP8266HTTPUpdateServer from the ESP8266 core).
 * - Arduino IDE: the clock appears as network port OTA_HOSTNAME.
 *
 * Both are protected by OTA_USERNAME / OTA_PASSWORD if a password is set.
 */
class Ota {
  public:
    static void setup();
    static void loop();

    static bool isProtected();

    // Returns false (and asks the browser for credentials) if a password is
    // set and the request is not authenticated.
    static bool authenticate();
};

#endif
