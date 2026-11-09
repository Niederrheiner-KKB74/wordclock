#include <Arduino.h>
#include "fastled.h"

#include "symbols.h"

/*
 * Palette
 * -------
 * .  off
 * g  green          b  brown          w  white
 * p  pink           o  sponge         r  red
 * y  candle flame (flickers)
 * Y  star / yellow ball (twinkles)
 * R  red ball (twinkles)
 * B  blue ball (twinkles)
 */

static const char CAKE[] PROGMEM =
  "..y..y..y.."
  "..y..y..y.."
  "..w..w..w.."
  ".ppppppppp."
  ".popppoppo."
  ".ooooooooo."
  ".rrrrrrrrr."
  ".ooooooooo."
  ".ooooooooo."
  "wwwwwwwwwww";

static const char TREE[] PROGMEM =
  ".....Y....."
  ".....g....."
  "....ggg...."
  "...ggRgg..."
  "..ggggBgg.."
  "...ggYgg..."
  "..gRgggBg.."
  ".gggYgggRg."
  "gBgggRgggYg"
  "....bbb....";

static const char HEART[] PROGMEM =
  "..........."
  "..rr...rr.."
  ".rprr.rrrr."
  ".rrrrrrrrr."
  ".rrrrrrrrr."
  "..rrrrrrr.."
  "...rrrrr..."
  "....rrr...."
  ".....r....."
  "...........";

static const char* const NAMES[] = {"Kein Bild", "Geburtstagstorte", "Weihnachtsbaum", "Herz"};

const char* Symbols::name(uint8_t id) {
  return id < SYMBOL_COUNT ? NAMES[id] : NAMES[0];
}

// Smooth 0..255 wave with its own phase per pixel, period about 2 seconds.
static uint8_t twinkle(uint8_t x, uint8_t y, uint32_t ms) {
  uint8_t phase = (x * 37 + y * 71) & 0xff;
  return sin8((uint8_t) (ms / 8) + phase);
}

// Flame flicker: changes about every 90 ms, differs per candle.
static uint8_t flicker(uint8_t x, uint32_t ms) {
  uint32_t step = ms / 90;
  uint32_t seed = (step * 2654435761u) ^ (x * 40503u);
  seed ^= seed >> 13;
  seed *= 0x5bd1e995u;
  seed ^= seed >> 15;
  return 150 + (seed % 106);
}

// Double beat like a heart: two quick pulses, then a pause (period 1.2 s).
static uint8_t heartbeat(uint32_t ms) {
  uint32_t t = ms % 1200;

  if (t < 120) {
    return 160 + (t * 95) / 120;
  }
  if (t < 240) {
    return 255 - ((t - 120) * 95) / 120;
  }
  if (t < 360) {
    return 160 + ((t - 240) * 70) / 120;
  }
  if (t < 600) {
    return 230 - ((t - 360) * 70) / 240;
  }

  return 160;
}

bool Symbols::pixel(uint8_t id, uint8_t x, uint8_t y, uint32_t ms, CRGB& color) {
  const char* bitmap;

  switch (id) {
    case SYMBOL_CAKE:  bitmap = CAKE;  break;
    case SYMBOL_TREE:  bitmap = TREE;  break;
    case SYMBOL_HEART: bitmap = HEART; break;
    default: return false;
  }

  if (x >= WIDTH || y >= HEIGHT) {
    return false;
  }

  char c = pgm_read_byte(&bitmap[y * WIDTH + x]);

  switch (c) {
    case 'g': color = CRGB(0, 130, 10); break;
    case 'b': color = CRGB(110, 45, 0); break;
    case 'w': color = CRGB(170, 170, 150); break;
    case 'p': color = CRGB(255, 70, 140); break;
    case 'o': color = CRGB(220, 130, 30); break;
    case 'r': color = CRGB(230, 0, 10); break;
    case 'y': {
      uint8_t v = flicker(x, ms);
      color = CRGB(255, 90 + v / 3, 0);
      color.nscale8_video(v);
      break;
    }
    case 'Y': color = CRGB(255, 190, 0); color.nscale8_video(110 + twinkle(x, y, ms) * 145 / 255); break;
    case 'R': color = CRGB(255, 0, 0);   color.nscale8_video(70 + twinkle(x, y, ms) * 185 / 255); break;
    case 'B': color = CRGB(0, 70, 255);  color.nscale8_video(70 + twinkle(x, y, ms) * 185 / 255); break;
    default: return false;
  }

  if (id == SYMBOL_HEART) {
    color.nscale8_video(heartbeat(ms));
  }

  return true;
}
