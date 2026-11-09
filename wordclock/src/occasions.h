#ifndef WORDCLOCK_OCCASIONS_H
#define WORDCLOCK_OCCASIONS_H

#include <Arduino.h>

/*
 * Special occasions: user defined dates (birthdays, anniversaries, ...)
 * from the web interface plus the Christmas season (1st Advent - 26.12.).
 */
class Occasions {
  public:
    // Picture and text for the given date. Returns false if there is none.
    static bool forDate(int day, int month, int year, uint8_t& symbol, String& text);

    // Same for the current local date (false if the time is not known yet).
    static bool today(uint8_t& symbol, String& text);

    // Day of December of the 1st Advent (values <= 0 mean a day in November:
    // 0 = 30.11., -1 = 29.11., ...).
    static int firstAdvent(int year);
};

#endif
