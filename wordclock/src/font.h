#ifndef WORDCLOCK_FONT_H
#define WORDCLOCK_FONT_H

#include <Arduino.h>

/*
 * Pixel font for the scrolling text: up to 5 columns x 8 rows per character
 * (rows 0-6 for capitals, row 7 for descenders), proportional width.
 * Covers ASCII letters, digits, common punctuation, German umlauts, ß, € and ♥.
 *
 * The drawings live in tools/font_gen.py, which generates font.cpp.
 */
class Font {
  public:
    static const uint8_t HEIGHT = 8;
    static const uint8_t SPACE_WIDTH;

    // Looks up a character. Each column is one byte, bit 0 = top row.
    static bool find(uint16_t codepoint, uint16_t& offset, uint8_t& width);
    static uint8_t column(uint16_t offset);

    static uint16_t nextCodepoint(const String& text, unsigned int& pos);
};

#endif
