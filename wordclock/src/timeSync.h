#ifndef WORDCLOCK_TIME_SYNC_H
#define WORDCLOCK_TIME_SYNC_H

#include <Arduino.h>

/*
 * Time keeping via the SNTP client built into the ESP8266 core.
 *
 * The local time (including daylight saving time) is calculated on the
 * device from a POSIX time zone rule such as "CET-1CEST,M3.5.0,M10.5.0/3".
 * No external web service is needed to determine the UTC offset.
 */
class Time {
  public:
    static int hour;
    static int minute;

    static void setup();
    static void loop();

    // Apply Config::tz and Config::ntp (call after changing them).
    static void applyConfig();

    // true once the clock has received a valid time from the NTP server.
    static bool isSynced();

  private:
    static char ntpServer[64];
};

#endif
