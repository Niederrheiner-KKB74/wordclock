#ifndef WORDCLOCK_FASTLED_H
#define WORDCLOCK_FASTLED_H

/*
 * Includes FastLED with the WordClock's settings. Always include this file
 * instead of <FastLED.h>, so every part of the firmware uses the same settings.
 */

#include "../user_config.h"

/* Interrupts during the LED transfer
 *
 * 0: no interrupts while the LEDs are updated (default). Prevents wrong
 *    colors / flickering, mostly on the first LED of the chain, caused by
 *    WiFi interrupts disturbing the timing.
 * 1: allow interrupts (FastLED default on the ESP8266). Use this if the web
 *    interface or WiFi becomes unreliable.
 */
#ifndef FASTLED_ALLOW_INTERRUPTS
  #define FASTLED_ALLOW_INTERRUPTS 0
#endif

#include <FastLED.h>

#endif
