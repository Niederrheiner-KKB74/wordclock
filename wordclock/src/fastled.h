#ifndef WORDCLOCK_FASTLED_H
#define WORDCLOCK_FASTLED_H

/*
 * Includes FastLED with the WordClock's settings. Always include this file
 * instead of <FastLED.h>, so every part of the firmware uses the same settings.
 */

#include "../user_config.h"

/* Interrupts during the LED transfer
 *
 * Not set: FastLED's default for the ESP8266 (interrupts allowed between
 * LEDs). Can be set in user_config.h:
 * 0: no interrupts while the LEDs are updated
 * 1: allow interrupts
 */

#include <FastLED.h>

/* FastLED versions before 3.5.0 make the first LED of the chain flicker in
 * random colors with the ESP8266 Arduino core 3.x (the timing of the first
 * bit is disturbed). 3.5.0 aborts and repeats such a transfer.
 */
#if FASTLED_VERSION < 3005000
  #error "FastLED 3.5.0 oder neuer noetig: Sketch > Bibliothek einbinden > Bibliotheken verwalten > FastLED aktualisieren. (FastLED 3.5.0 or newer required, older versions make the first LED flicker.)"
#endif

#endif
