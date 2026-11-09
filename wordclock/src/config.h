#ifndef WORDCLOCK_CONFIG_H
#define WORDCLOCK_CONFIG_H

#include <Arduino.h>

#include "../user_config.h"
#include "types.h"

/***********************************************
 * CONFIG START
 **********************************************/

/* Grid rows
 *
 * Options
 * -------
 * 10: 10 rows
 * 11: 11 rows
 */
#ifndef GRID_ROWS
  #define GRID_ROWS 11
#endif

/* Grid cols
 *
 * Options
 * -------
 * 11: 11 cols
 */
#ifndef GRID_COLS
  #define GRID_COLS 11
#endif

/* Postition of first LED
 *
 * Options
 * -------
 * 0: top-left
 * 1: top-right
 * 2: buttom-left
 * 3: buttom-right
 */
#ifndef GRID_FIRST
  #define GRID_FIRST 2
#endif

/* Grid language
 *
 * Options
 * -------
 * 1: de-DE:     german
 * 2: de-DE_alt: german alternative
 * 3: de_DE_s: "Es ist" only at full and half hour
 * 4: de_DE_s1: "Es ist" only at full and half hour + Minutes in Last row (new front Plate De_de_S1)
 */
#ifndef GRID_LANGUAGE
  #define GRID_LANGUAGE de_DE
//#define GRID_LANGUAGE de_DE_alt
#endif

/* Position of single minute LEDs
 *
 * Options
 * -------
 * 0: before
 * 1: after
 */
#ifndef GRID_SINGLE_MINUTES
  #define GRID_SINGLE_MINUTES 1
#endif

#ifndef EXTRA_LEDS
  #define EXTRA_LEDS 4
#endif

/* Data PIN, where the LED strip is connected to */
#ifndef DATA_PIN
  #define DATA_PIN D4
#endif

/* Default time zone (POSIX TZ rule), used until another one is saved in the web interface.
 * Germany / Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
 */
#ifndef TZ_DEFAULT
  #define TZ_DEFAULT "CET-1CEST,M3.5.0,M10.5.0/3"
#endif

/* Firmware update over WiFi
 *
 * Web upload:      http://[YOUR_WORDCLOCK_IP]/firmware
 * Arduino IDE:     network port "wordclock" (Tools > Port)
 *
 * If OTA_PASSWORD is empty, updates are possible without a password for
 * everyone in your network. Set a password in user_config.h to protect them.
 */
#ifndef OTA_HOSTNAME
  #define OTA_HOSTNAME "wordclock"
#endif

#ifndef OTA_USERNAME
  #define OTA_USERNAME "admin"
#endif

#ifndef OTA_PASSWORD
  #define OTA_PASSWORD ""
#endif

#ifndef CONSUMPTION_D1_MINI
  #define CONSUMPTION_D1_MINI 170 // 170mA in active mode
#endif

#ifndef CONSUMPTION_PER_LED
  #define CONSUMPTION_PER_LED 60 // 60mA per LED on full brightness in white.
#endif

/* Number of special occasions (birthdays etc.) that can be set in the web interface */
#ifndef MAX_OCCASIONS
  #define MAX_OCCASIONS 6
#endif

/* Maximum length of scrolling texts in bytes (umlauts count twice) */
#define TEXT_MAX_LENGTH 100

#define FIRMWARE_VERSION "4.3.0"

/***********************************************
 * CONFIG END
 **********************************************/

enum ColorMode {
  COLOR_MODE_STATIC = 0,    // one color (foreground color)
  COLOR_MODE_CYCLE = 1,     // whole clock slowly cycles through the spectrum
  COLOR_MODE_RAINBOW = 2,   // rainbow running diagonally over the letters
  COLOR_MODE_GRADIENT = 3,  // gradient from foreground color (top) to second color (bottom)
  COLOR_MODE_WAVE = 4,      // waves of foreground and second color moving over the letters
  COLOR_MODE_COUNT = 5
};

enum DndMode {
  DND_MODE_OFF = 0,          // switch the clock off
  DND_MODE_DIM = 1,          // dim, keep the colors / color mode
  DND_MODE_DIM_COLOR = 2,    // dim and show the time in the night color (no animation)
  DND_MODE_COUNT = 3
};

typedef struct {
  uint8_t day;     // 1-31, 0 = entry not used
  uint8_t month;   // 1-12
  uint8_t symbol;  // see SymbolId in symbols.h
  String text;     // optional scrolling text
} occasion_t;

class Config {
  public:
    static color_t color_bg;
    static color_t color_fg;
    static int power_supply;
    static double brightness;
    static String tz;
    static bool dnd_active;
    static clock_time_t dnd_start;
    static clock_time_t dnd_end;
    static int dnd_mode;
    static double dnd_brightness;  // 0.01 - 1.0, used by the dim modes
    static color_t dnd_color;
    static String ntp;
    static bool healthcheck;

    static int color_mode;
    static color_t color_fg2;
    static int effect_speed;       // 1 (slow) - 10 (fast)

    static int text_speed;         // 1 (slow) - 3 (fast)

    static bool xmas;              // Christmas tree from 1st Advent to 26.12.
    static int occasion_interval;  // seconds of time display between pictures
    static occasion_t occasions[MAX_OCCASIONS];

    static void save();
    static void load();
};
#endif
