#ifndef WORDCLOCK_SYMBOLS_H
#define WORDCLOCK_SYMBOLS_H

#include <Arduino.h>
#include "fastled.h"

/*
 * Small pictures for special occasions, 11 x 10 pixels.
 *
 * Each picture is drawn with letters (see PALETTE in symbols.cpp); some
 * pixels are animated: candle flames flicker, the balls on the tree twinkle
 * and the heart beats.
 */
enum SymbolId : uint8_t {
  SYMBOL_NONE = 0,
  SYMBOL_CAKE = 1,
  SYMBOL_TREE = 2,
  SYMBOL_HEART = 3,
  SYMBOL_COUNT = 4
};

class Symbols {
  public:
    static const uint8_t WIDTH = 11;
    static const uint8_t HEIGHT = 10;

    static const char* name(uint8_t id);

    // Color of one pixel at time ms. Returns false for pixels that are off.
    static bool pixel(uint8_t id, uint8_t x, uint8_t y, uint32_t ms, CRGB& color);
};

#endif
