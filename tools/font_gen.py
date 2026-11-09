#!/usr/bin/env python3
"""Generates wordclock/src/font.cpp from the glyph drawings below.

Each glyph is drawn on a grid of up to 5 columns x 8 rows ('#' = LED on).
Rows 0-6 hold capitals and digits, row 7 is for descenders (g, j, p, q, y).
Empty columns on the left and right are trimmed, so narrow characters
(i, l, 1, punctuation) take less space in the scrolling text.

Usage: python3 tools/font_gen.py > wordclock/src/font.cpp
"""

import sys

G = {}


def glyph(chars, *rows):
    rows = list(rows) + [""] * (8 - len(rows))
    for ch in chars:
        G[ch] = rows


_ = ""  # empty row

# --- capitals -------------------------------------------------------------
glyph("A", " ### ", "#   #", "#   #", "#####", "#   #", "#   #", "#   #")
glyph("B", "#### ", "#   #", "#   #", "#### ", "#   #", "#   #", "#### ")
glyph("C", " ### ", "#   #", "#    ", "#    ", "#    ", "#   #", " ### ")
glyph("D", "#### ", "#   #", "#   #", "#   #", "#   #", "#   #", "#### ")
glyph("E", "#####", "#    ", "#    ", "#### ", "#    ", "#    ", "#####")
glyph("F", "#####", "#    ", "#    ", "#### ", "#    ", "#    ", "#    ")
glyph("G", " ### ", "#   #", "#    ", "# ###", "#   #", "#   #", " ####")
glyph("H", "#   #", "#   #", "#   #", "#####", "#   #", "#   #", "#   #")
glyph("I", "###", " # ", " # ", " # ", " # ", " # ", "###")
glyph("J", "  ###", "   # ", "   # ", "   # ", "   # ", "#  # ", " ##  ")
glyph("K", "#   #", "#  # ", "# #  ", "##   ", "# #  ", "#  # ", "#   #")
glyph("L", "#    ", "#    ", "#    ", "#    ", "#    ", "#    ", "#####")
glyph("M", "#   #", "## ##", "# # #", "# # #", "#   #", "#   #", "#   #")
glyph("N", "#   #", "#   #", "##  #", "# # #", "#  ##", "#   #", "#   #")
glyph("O", " ### ", "#   #", "#   #", "#   #", "#   #", "#   #", " ### ")
glyph("P", "#### ", "#   #", "#   #", "#### ", "#    ", "#    ", "#    ")
glyph("Q", " ### ", "#   #", "#   #", "#   #", "# # #", "#  # ", " ## #")
glyph("R", "#### ", "#   #", "#   #", "#### ", "# #  ", "#  # ", "#   #")
glyph("S", " ####", "#    ", "#    ", " ### ", "    #", "    #", "#### ")
glyph("T", "#####", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ", "  #  ")
glyph("U", "#   #", "#   #", "#   #", "#   #", "#   #", "#   #", " ### ")
glyph("V", "#   #", "#   #", "#   #", "#   #", "#   #", " # # ", "  #  ")
glyph("W", "#   #", "#   #", "#   #", "# # #", "# # #", "# # #", " # # ")
glyph("X", "#   #", "#   #", " # # ", "  #  ", " # # ", "#   #", "#   #")
glyph("Y", "#   #", "#   #", " # # ", "  #  ", "  #  ", "  #  ", "  #  ")
glyph("Z", "#####", "    #", "   # ", "  #  ", " #   ", "#    ", "#####")
glyph("Ä", " # # ", _, " ### ", "#   #", "#####", "#   #", "#   #")
glyph("Ö", " # # ", _, " ### ", "#   #", "#   #", "#   #", " ### ")
glyph("Ü", " # # ", _, "#   #", "#   #", "#   #", "#   #", " ### ")

# --- small letters --------------------------------------------------------
glyph("a", _, _, " ### ", "    #", " ####", "#   #", " ####")
glyph("b", "#    ", "#    ", "# ## ", "##  #", "#   #", "#   #", "#### ")
glyph("c", _, _, " ### ", "#    ", "#    ", "#   #", " ### ")
glyph("d", "    #", "    #", " ## #", "#  ##", "#   #", "#   #", " ####")
glyph("e", _, _, " ### ", "#   #", "#####", "#    ", " ### ")
glyph("f", "  ## ", " #  #", " #   ", "###  ", " #   ", " #   ", " #   ")
glyph("g", _, _, " ####", "#   #", "#   #", " ####", "    #", " ### ")
glyph("h", "#    ", "#    ", "# ## ", "##  #", "#   #", "#   #", "#   #")
glyph("i", " # ", _, "## ", " # ", " # ", " # ", "###")
glyph("j", "   #", _, "  ##", "   #", "   #", "   #", "#  #", " ## ")
glyph("k", "#   ", "#   ", "#  #", "# # ", "##  ", "# # ", "#  #")
glyph("l", "## ", " # ", " # ", " # ", " # ", " # ", "###")
glyph("m", _, _, "## # ", "# # #", "# # #", "#   #", "#   #")
glyph("n", _, _, "# ## ", "##  #", "#   #", "#   #", "#   #")
glyph("o", _, _, " ### ", "#   #", "#   #", "#   #", " ### ")
glyph("p", _, _, "#### ", "#   #", "#   #", "#### ", "#    ", "#    ")
glyph("q", _, _, " ####", "#   #", "#   #", " ####", "    #", "    #")
glyph("r", _, _, "# ## ", "##  #", "#    ", "#    ", "#    ")
glyph("s", _, _, " ####", "#    ", " ### ", "    #", "#### ")
glyph("t", " #   ", " #   ", "###  ", " #   ", " #   ", " #  #", "  ## ")
glyph("u", _, _, "#   #", "#   #", "#   #", "#  ##", " ## #")
glyph("v", _, _, "#   #", "#   #", "#   #", " # # ", "  #  ")
glyph("w", _, _, "#   #", "#   #", "# # #", "# # #", " # # ")
glyph("x", _, _, "#   #", " # # ", "  #  ", " # # ", "#   #")
glyph("y", _, _, "#   #", "#   #", "#   #", " ####", "    #", " ### ")
glyph("z", _, _, "#####", "   # ", "  #  ", " #   ", "#####")
glyph("ä", " # # ", _, " ### ", "    #", " ####", "#   #", " ####")
glyph("ö", " # # ", _, " ### ", "#   #", "#   #", "#   #", " ### ")
glyph("ü", " # # ", _, "#   #", "#   #", "#   #", "#  ##", " ## #")
glyph("ß", " ##  ", "#  # ", "#  # ", "# #  ", "#  # ", "#  # ", "# #  ")

# --- digits ---------------------------------------------------------------
glyph("0", " ### ", "#   #", "#  ##", "# # #", "##  #", "#   #", " ### ")
glyph("1", " # ", "## ", " # ", " # ", " # ", " # ", "###")
glyph("2", " ### ", "#   #", "    #", "   # ", "  #  ", " #   ", "#####")
glyph("3", "#####", "   # ", "  #  ", "   # ", "    #", "#   #", " ### ")
glyph("4", "   # ", "  ## ", " # # ", "#  # ", "#####", "   # ", "   # ")
glyph("5", "#####", "#    ", "#### ", "    #", "    #", "#   #", " ### ")
glyph("6", "  ## ", " #   ", "#    ", "#### ", "#   #", "#   #", " ### ")
glyph("7", "#####", "    #", "   # ", "  #  ", " #   ", " #   ", " #   ")
glyph("8", " ### ", "#   #", "#   #", " ### ", "#   #", "#   #", " ### ")
glyph("9", " ### ", "#   #", "#   #", " ####", "    #", "   # ", " ##  ")

# --- punctuation and symbols ----------------------------------------------
glyph("!", "#", "#", "#", "#", "#", _, "#")
glyph("?", " ### ", "#   #", "    #", "   # ", "  #  ", _, "  #  ")
glyph(".", _, _, _, _, _, _, "#")
glyph(",", _, _, _, _, _, " #", " #", "# ")
glyph(":", _, _, "#", _, _, "#")
glyph(";", _, _, " #", _, _, " #", " #", "# ")
glyph("-", _, _, _, "###")
glyph("+", _, "  #  ", "  #  ", "#####", "  #  ", "  #  ")
glyph("=", _, _, "#####", _, "#####")
glyph("'", "#", "#")
glyph("\"", "# #", "# #")
glyph("(", "  #", " # ", "#  ", "#  ", "#  ", " # ", "  #")
glyph(")", "#  ", " # ", "  #", "  #", "  #", " # ", "#  ")
glyph("/", "    #", "    #", "   # ", "  #  ", " #   ", "#    ", "#    ")
glyph("&", " ##  ", "#  # ", "# #  ", " #   ", "# # #", "#  # ", " ## #")
glyph("#", " # # ", " # # ", "#####", " # # ", "#####", " # # ", " # # ")
glyph("*", _, "  #  ", "# # #", " ### ", "# # #", "  #  ")
glyph("%", "##   ", "##  #", "   # ", "  #  ", " #   ", "#  ##", "   ##")
glyph("°", " # ", "# #", " # ")
glyph("€", "  ###", " #   ", "#### ", " #   ", "#### ", " #   ", "  ###")
glyph("_", _, _, _, _, _, _, _, "#####")
glyph("♥❤", _, " # # ", "#####", "#####", " ### ", "  #  ")  # hearts

SPACE_WIDTH = 3


def columns(rows):
    width = max(len(r) for r in rows)
    cols = []
    for x in range(width):
        bits = 0
        for y, r in enumerate(rows):
            if x < len(r) and r[x] == "#":
                bits |= 1 << y
        cols.append(bits)
    while cols and cols[0] == 0:
        cols.pop(0)
    while cols and cols[-1] == 0:
        cols.pop()
    return cols


def main():
    for ch, rows in G.items():
        assert len(rows) == 8, ch
        assert all(len(r) <= 5 for r in rows), ch
        assert columns(rows), ch

    entries = sorted(G.items(), key=lambda kv: ord(kv[0]))
    data, index = [], []
    for ch, rows in entries:
        cols = columns(rows)
        index.append((ord(ch), len(data), len(cols), ch))
        data.extend(cols)

    out = sys.stdout
    out.write("// Generated by tools/font_gen.py - edit the drawings there, not this file.\n")
    out.write("#include <Arduino.h>\n\n#include \"font.h\"\n\n")
    out.write("static const uint8_t FONT_DATA[] PROGMEM = {\n")
    for i in range(0, len(data), 16):
        out.write("  " + ", ".join("0x%02x" % b for b in data[i:i + 16]) + ",\n")
    out.write("};\n\n")
    out.write("struct FontGlyph {\n  uint16_t codepoint;\n  uint16_t offset;\n  uint8_t width;\n};\n\n")
    out.write("static const FontGlyph FONT_INDEX[] PROGMEM = {\n")
    for cp, off, w, ch in index:
        label = ch if ch.isprintable() and ch not in "\\" else "U+%04X" % cp
        out.write("  {0x%04x, %3d, %d}, // %s\n" % (cp, off, w, label))
    out.write("};\n\n")
    out.write("static const uint16_t FONT_COUNT = %d;\n" % len(index))
    out.write("const uint8_t Font::SPACE_WIDTH = %d;\n" % SPACE_WIDTH)
    out.write(FONT_LOOKUP)


FONT_LOOKUP = r"""
// Binary search in the (sorted) glyph index. Returns false if the font has
// no drawing for this character.
bool Font::find(uint16_t codepoint, uint16_t& offset, uint8_t& width) {
  int lo = 0;
  int hi = FONT_COUNT - 1;

  while (lo <= hi) {
    int mid = (lo + hi) / 2;
    uint16_t cp = pgm_read_word(&FONT_INDEX[mid].codepoint);

    if (cp == codepoint) {
      offset = pgm_read_word(&FONT_INDEX[mid].offset);
      width = pgm_read_byte(&FONT_INDEX[mid].width);
      return true;
    }

    if (cp < codepoint) {
      lo = mid + 1;
    } else {
      hi = mid - 1;
    }
  }

  return false;
}

uint8_t Font::column(uint16_t offset) {
  return pgm_read_byte(&FONT_DATA[offset]);
}

// Decodes the next UTF-8 character of text, starting at pos (advances pos).
uint16_t Font::nextCodepoint(const String& text, unsigned int& pos) {
  uint8_t c = text.charAt(pos++);

  if (c < 0x80) {
    return c;
  }

  int extra = 0;
  uint16_t cp = 0;

  if ((c & 0xe0) == 0xc0) {
    cp = c & 0x1f;
    extra = 1;
  } else if ((c & 0xf0) == 0xe0) {
    cp = c & 0x0f;
    extra = 2;
  } else {
    // 4-byte sequences (emoji) and invalid bytes: skip continuation bytes.
    while (pos < text.length() && (((uint8_t) text.charAt(pos)) & 0xc0) == 0x80) {
      pos++;
    }
    return '?';
  }

  for (int i = 0; i < extra && pos < text.length(); i++) {
    cp = (cp << 6) | (((uint8_t) text.charAt(pos++)) & 0x3f);
  }

  return cp;
}
"""

if __name__ == "__main__":
    main()
