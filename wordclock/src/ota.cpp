#include <ESP8266WiFi.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include "fastled.h"

#include "ota.h"
#include "config.h"
#include "httpServer.h"
#include "led.h"

static ESP8266HTTPUpdateServer httpUpdater;

bool Ota::isProtected() {
  return strlen(OTA_PASSWORD) > 0;
}

bool Ota::authenticate() {
  if (!Ota::isProtected()) {
    return true;
  }

  if (!HttpServer::web.authenticate(OTA_USERNAME, OTA_PASSWORD)) {
    HttpServer::web.requestAuthentication();
    return false;
  }

  return true;
}

// Show the upload progress on the clock: the LEDs fill up one by one.
static void showProgress(unsigned int progress, unsigned int total) {
  if (total == 0) {
    return;
  }

  int lit = (int) ((uint32_t) progress * NUM_LEDS / total);

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i < lit) {
      Led::ids[i].setRGB(0, 40, 80);
    } else {
      Led::ids[i].setRGB(0, 0, 0);
    }
  }

  FastLED.show();
}

void Ota::setup() {
  // Web upload, POST /update (and the library's own minimal form on GET /update).
  if (Ota::isProtected()) {
    httpUpdater.setup(&HttpServer::web, "/update", OTA_USERNAME, OTA_PASSWORD);
  } else {
    httpUpdater.setup(&HttpServer::web, "/update");
  }

  // Upload from the Arduino IDE ("Tools > Port > network ports").
  // ArduinoOTA also starts mDNS, so the web interface is reachable as
  // http://OTA_HOSTNAME.local in most networks.
  ArduinoOTA.setHostname(OTA_HOSTNAME);

  if (Ota::isProtected()) {
    ArduinoOTA.setPassword(OTA_PASSWORD);
  }

  ArduinoOTA.onStart([]() {
    Serial.println("OTA update started.");
    showProgress(0, 1);
  });

  ArduinoOTA.onProgress(showProgress);

  ArduinoOTA.onEnd([]() {
    Serial.println("OTA update finished, rebooting.");
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.print("OTA update failed, error ");
    Serial.println(error);
  });

  ArduinoOTA.begin();
  MDNS.addService("http", "tcp", 80);

  Serial.print("Firmware update: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/firmware");
}

void Ota::loop() {
  ArduinoOTA.handle();
}
