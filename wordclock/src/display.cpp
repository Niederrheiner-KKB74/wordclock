#include <Arduino.h>
#include <FastLED.h>

#include "display.h"
#include "config.h"
#include "led.h"
#include "font.h"
#include "symbols.h"
#include "occasions.h"

static const uint16_t FRAME_MS = 30;            // about 33 frames per second for animations
static const uint16_t REFRESH_MS = 1000;        // resend static content every second
static const uint32_t SYMBOL_MS = 7000;         // how long a picture is shown
static const uint16_t TEXT_MAX_COLUMNS = 600;   // about 100 characters
static const uint16_t TEXT_STEP_MS[] = {0, 170, 115, 75};  // per text speed 1-3

enum Content : uint8_t {
  CONTENT_TIME,
  CONTENT_SYMBOL,
  CONTENT_TEXT
};

// Time: which LEDs belong to the current time, and night mode.
static bool timeMask[NUM_LEDS];
static bool timeDark = false;

// Grid position of every LED (for color modes); extra LEDs get positions below the grid.
static int8_t ledX[NUM_LEDS];
static int8_t ledY[NUM_LEDS];

static Content content = CONTENT_TIME;
static bool dirty = true;
static uint32_t lastFrame = 0;

// Scrolling text
static uint8_t textColumns[TEXT_MAX_COLUMNS];
static uint16_t textLength = 0;
static uint16_t textOffset = 0;
static uint32_t textLastStep = 0;
static uint8_t textRepeatsLeft = 0;
static bool textEndless = false;
static bool manualText = false;

// Picture + text sequence (occasion or preview)
static bool sequenceActive = false;
static bool sequenceIsPreview = false;
static uint8_t sequenceSymbol = SYMBOL_NONE;
static String sequenceText;
static uint32_t symbolStart = 0;
static uint32_t lastSequenceEnd = 0;
static uint32_t lastOccasionCheck = 0;

static void setGrid(int x, int y, const CRGB& color) {
  if (x < 0 || y < 0 || x >= GRID_COLS || y >= GRID_ROWS) {
    return;
  }

  Led::ids[Led::getLedId(y * GRID_COLS + x)] = color;
}

static void prepareText(const String& text) {
  textLength = 0;
  unsigned int pos = 0;

  while (pos < text.length()) {
    uint16_t cp = Font::nextCodepoint(text, pos);
    uint16_t offset = 0;
    uint8_t width = 0;

    if (cp == ' ' || cp == 0xa0) {
      for (uint8_t i = 0; i < Font::SPACE_WIDTH && textLength < TEXT_MAX_COLUMNS; i++) {
        textColumns[textLength++] = 0;
      }
      continue;
    }

    if (!Font::find(cp, offset, width) && !Font::find('?', offset, width)) {
      continue;
    }

    if (textLength + width + 1 > TEXT_MAX_COLUMNS) {
      break;
    }

    for (uint8_t i = 0; i < width; i++) {
      textColumns[textLength++] = Font::column(offset + i);
    }

    textColumns[textLength++] = 0;  // gap between characters
  }
}

static void startText(const String& text, uint8_t repeat) {
  prepareText(text);
  textOffset = 0;
  textLastStep = millis();
  textRepeatsLeft = repeat;
  textEndless = (repeat == 0);
  content = CONTENT_TEXT;
  dirty = true;
}

static void endSequence() {
  sequenceActive = false;
  sequenceText = "";
  lastSequenceEnd = millis();

  if (!manualText) {
    content = CONTENT_TIME;
  }

  dirty = true;
}

static void startSequence(uint8_t symbol, const String& text, bool isPreview) {
  sequenceActive = true;
  sequenceIsPreview = isPreview;
  sequenceSymbol = symbol;
  sequenceText = text;

  if (symbol != SYMBOL_NONE) {
    content = CONTENT_SYMBOL;
    symbolStart = millis();
    dirty = true;
  } else if (text.length() > 0) {
    startText(text, 1);
  } else {
    endSequence();
  }
}

static void afterSymbol() {
  if (sequenceText.length() > 0) {
    startText(sequenceText, 1);
  } else {
    endSequence();
  }
}

static void afterText() {
  if (manualText) {
    manualText = false;
    content = CONTENT_TIME;
    dirty = true;
    lastSequenceEnd = millis();
    return;
  }

  endSequence();
}

static void scheduleOccasions(uint32_t now) {
  if (manualText || sequenceActive || timeDark || now - lastOccasionCheck < 1000) {
    return;
  }

  lastOccasionCheck = now;

  if (now - lastSequenceEnd < (uint32_t) Config::occasion_interval * 1000UL) {
    return;
  }

  uint8_t symbol = SYMBOL_NONE;
  String text;

  if (Occasions::today(symbol, text)) {
    startSequence(symbol, text, false);
  } else {
    lastSequenceEnd = now;
  }
}

static void advanceText(uint32_t now) {
  if (now - textLastStep < TEXT_STEP_MS[constrain(Config::text_speed, 1, 3)]) {
    return;
  }

  textLastStep = now;
  textOffset++;

  // One pass is over when the last column has left the grid on the left.
  if (textOffset >= textLength + GRID_COLS) {
    if (textEndless || textRepeatsLeft > 1) {
      if (!textEndless) {
        textRepeatsLeft--;
      }
      textOffset = 0;
    } else {
      afterText();
    }
  }
}

static void render(uint32_t now) {
  CRGB background = timeDark
    ? CRGB(0, 0, 0)
    : CRGB(Config::color_bg.r * 0.2, Config::color_bg.g * 0.2, Config::color_bg.b * 0.2);

  for (int i = 0; i < NUM_LEDS; i++) {
    Led::ids[i] = background;
  }

  if (content == CONTENT_TIME) {
    if (!timeDark) {
      for (int i = 0; i < NUM_LEDS; i++) {
        if (timeMask[i]) {
          Led::ids[i] = Display::effectColor(ledX[i], ledY[i], now);
        }
      }
    }
  } else if (content == CONTENT_SYMBOL) {
    int xOff = (GRID_COLS - Symbols::WIDTH) / 2;
    int yOff = (GRID_ROWS - Symbols::HEIGHT) / 2;
    CRGB color;

    for (uint8_t y = 0; y < Symbols::HEIGHT; y++) {
      for (uint8_t x = 0; x < Symbols::WIDTH; x++) {
        if (Symbols::pixel(sequenceSymbol, x, y, now - symbolStart, color)) {
          setGrid(x + xOff, y + yOff, color);
        }
      }
    }
  } else if (content == CONTENT_TEXT) {
    int yOff = (GRID_ROWS - Font::HEIGHT) / 2;

    for (int x = 0; x < GRID_COLS; x++) {
      int index = (int) textOffset + x - GRID_COLS;

      if (index < 0 || index >= textLength) {
        continue;
      }

      uint8_t bits = textColumns[index];

      for (int y = 0; y < Font::HEIGHT; y++) {
        if (bits & (1 << y)) {
          setGrid(x, y + yOff, Display::effectColor(x, y + yOff, now));
        }
      }
    }
  }

  // Safety net for pictures and text, which light more LEDs than the time:
  // FastLED reduces the brightness if the power supply would be overloaded.
  // FastLED assumes ~42 mA per white LED, this project 60 mA, hence 70 %.
  int budget = Config::power_supply - CONSUMPTION_D1_MINI;
  FastLED.setMaxPowerInVoltsAndMilliamps(5, max(100, budget * 7 / 10));
  FastLED.setBrightness(Config::brightness * 255);
  FastLED.show();
}

CRGB Display::effectColor(int x, int y, uint32_t ms) {
  CRGB fg(Config::color_fg.r, Config::color_fg.g, Config::color_fg.b);
  CRGB fg2(Config::color_fg2.r, Config::color_fg2.g, Config::color_fg2.b);

  // Duration of one full color cycle: speed 1 = 10 min, 3 = 67 s, 10 = 6 s.
  int speed = constrain(Config::effect_speed, 1, 10);
  uint32_t period = 600000UL / (uint32_t) (speed * speed);
  uint8_t phase = (uint8_t) ((ms % period) * 256UL / period);

  switch (Config::color_mode) {
    case COLOR_MODE_CYCLE:
      return CRGB(CHSV(phase, 255, 255));

    case COLOR_MODE_RAINBOW:
      return CRGB(CHSV((uint8_t) (phase - (x + y) * 10), 255, 255));

    case COLOR_MODE_GRADIENT: {
      int row = constrain(y, 0, GRID_ROWS - 1);
      return blend(fg, fg2, (uint8_t) (row * 255 / (GRID_ROWS - 1)));
    }

    case COLOR_MODE_WAVE:
      return blend(fg, fg2, sin8((uint8_t) (y * 22 + x * 6 - phase)));

    default:
      return fg;
  }
}

void Display::setup() {
  for (int i = 0; i < NUM_LEDS; i++) {
    ledX[i] = -1;
    ledY[i] = -1;
    timeMask[i] = false;
  }

  for (int id = 0; id < GRID_ROWS * GRID_COLS; id++) {
    int led = Led::getLedId(id);

    if (led >= 0 && led < NUM_LEDS) {
      ledX[led] = id % GRID_COLS;
      ledY[led] = id / GRID_COLS;
    }
  }

  // Single minute LEDs: spread along the bottom edge.
  int extra = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (ledX[i] < 0) {
      ledX[i] = (extra * (GRID_COLS - 1)) / max(1, EXTRA_LEDS - 1);
      ledY[i] = GRID_ROWS - 1;
      extra++;
    }
  }

  lastSequenceEnd = millis();
  dirty = true;
}

void Display::loop() {
  uint32_t now = millis();

  scheduleOccasions(now);

  // Night mode started during an occasion: stop showing it.
  if (sequenceActive && !sequenceIsPreview && timeDark && !manualText) {
    endSequence();
  }

  if (content == CONTENT_SYMBOL && now - symbolStart >= SYMBOL_MS) {
    afterSymbol();
  }

  if (content == CONTENT_TEXT) {
    advanceText(now);
  }

  bool animated = (content != CONTENT_TIME) || (!timeDark && Config::color_mode != COLOR_MODE_STATIC);

  // Static content is resent every second as well: a transmission error
  // (typically on the first LED of the chain) would otherwise stay visible
  // until the next minute.
  bool due = now - lastFrame >= (animated ? FRAME_MS : REFRESH_MS);

  if (!dirty && !due) {
    return;
  }

  lastFrame = now;
  dirty = false;
  render(now);
}

void Display::unmark(int led) {
  if (led >= 0 && led < NUM_LEDS) {
    timeMask[led] = false;
  }
}

void Display::mark(int led) {
  if (led >= 0 && led < NUM_LEDS) {
    timeMask[led] = true;
  }
}

void Display::timeChanged(bool dark) {
  timeDark = dark;
  dirty = true;
}

void Display::showText(const String& text, uint8_t repeat) {
  if (sequenceActive) {
    sequenceActive = false;
    sequenceText = "";
  }

  manualText = true;
  startText(text, repeat);
}

void Display::stopText() {
  if (manualText) {
    manualText = false;
    content = CONTENT_TIME;
    lastSequenceEnd = millis();
    dirty = true;
  }

  if (sequenceActive) {
    endSequence();
  }
}

void Display::preview(uint8_t symbol, const String& text) {
  manualText = false;
  startSequence(symbol, text, true);
}

void Display::invalidate() {
  dirty = true;
}
