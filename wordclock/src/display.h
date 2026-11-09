#ifndef WORDCLOCK_DISPLAY_H
#define WORDCLOCK_DISPLAY_H

#include <Arduino.h>
#include "fastled.h"

/*
 * Draws everything on the LEDs, several times per second:
 *
 * - the time (which LEDs belong to it is reported by the language file via
 *   mark() / unmark() / timeChanged()), colored by the selected color mode
 * - scrolling text
 * - pictures for special occasions, alternating with the time
 *
 * Priority: scrolling text started in the web interface > preview /
 * occasion (picture, then its text) > time.
 */
class Display {
  public:
    static void setup();
    static void loop();

    // Called by the language files when the time changes.
    static void unmark(int led);
    static void mark(int led);
    static void timeChanged(bool dark);

    // Scrolling text. repeat = 0 shows the text until stopText() is called.
    static void showText(const String& text, uint8_t repeat);
    static void stopText();

    // Shows a picture (if any) followed by the text (if any) once.
    static void preview(uint8_t symbol, const String& text);

    // Redraw on the next loop (after settings changed).
    static void invalidate();

    // Color of the time / text LED at grid position x, y (used for color modes).
    static CRGB effectColor(int x, int y, uint32_t ms);
};

#endif
