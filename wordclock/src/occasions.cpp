#include <Arduino.h>
#include <time.h>

#include "occasions.h"
#include "config.h"
#include "symbols.h"
#include "timeSync.h"

// Day of week, 0 = Sunday (Sakamoto's algorithm).
static int dayOfWeek(int year, int month, int day) {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

  if (month < 3) {
    year -= 1;
  }

  return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;
}

int Occasions::firstAdvent(int year) {
  // The 4th Advent is the last Sunday before Christmas (18. - 24.12.).
  int fourthAdvent = 24 - dayOfWeek(year, 12, 24);
  return fourthAdvent - 21;
}

bool Occasions::forDate(int day, int month, int year, uint8_t& symbol, String& text) {
  // User defined dates come first, so a birthday on 24.12. still shows the cake.
  for (int i = 0; i < MAX_OCCASIONS; i++) {
    const occasion_t& o = Config::occasions[i];

    if (o.day == day && o.month == month && (o.symbol != SYMBOL_NONE || o.text.length() > 0)) {
      symbol = o.symbol;
      text = o.text;
      return true;
    }
  }

  if (Config::xmas) {
    int advent = Occasions::firstAdvent(year);
    bool season = false;

    if (month == 12 && day <= 26 && day >= advent) {
      season = true;
    } else if (month == 11 && advent <= 0 && day >= 30 + advent) {
      season = true;
    }

    if (season) {
      symbol = SYMBOL_TREE;
      text = (month == 12 && day >= 24) ? "Frohe Weihnachten" : "";
      return true;
    }
  }

  return false;
}

bool Occasions::today(uint8_t& symbol, String& text) {
  if (!Time::isSynced()) {
    return false;
  }

  time_t now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);

  return Occasions::forDate(local.tm_mday, local.tm_mon + 1, local.tm_year + 1900, symbol, text);
}
