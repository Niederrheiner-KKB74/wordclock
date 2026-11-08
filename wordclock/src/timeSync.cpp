#include <Arduino.h>
#include <time.h>

#include "timeSync.h"
#include "config.h"
#include "grid.h"

// Any timestamp before 2021-01-01 means "no NTP answer received yet".
static const time_t VALID_TIME_THRESHOLD = 1609459200;

void Time::setup() {
  Time::applyConfig();
}

void Time::applyConfig() {
  // The SNTP client keeps a pointer to the server name, so it must live in a
  // buffer that is not freed or reallocated when Config::ntp changes.
  strncpy(Time::ntpServer, Config::ntp.c_str(), sizeof(Time::ntpServer) - 1);
  Time::ntpServer[sizeof(Time::ntpServer) - 1] = '\0';

  // Sets the TZ rule and (re)starts SNTP. Re-sync happens automatically.
  configTime(Config::tz.c_str(), Time::ntpServer);

  Serial.print("Time zone: ");
  Serial.println(Config::tz);
  Serial.print("NTP server: ");
  Serial.println(Time::ntpServer);
}

bool Time::isSynced() {
  return time(nullptr) >= VALID_TIME_THRESHOLD;
}

void Time::loop() {
  if (!Time::isSynced()) {
    return;
  }

  time_t now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);

  // Comparing the hour as well catches jumps caused by a DST switch or a
  // changed time zone, not only regular minute changes.
  if (local.tm_min != Time::minute || local.tm_hour != Time::hour) {
    Time::hour = local.tm_hour;
    Time::minute = local.tm_min;
    Grid::setTime(Time::hour, Time::minute);
  }
}

int Time::hour = -1;
int Time::minute = -1;
char Time::ntpServer[64] = "pool.ntp.org";
