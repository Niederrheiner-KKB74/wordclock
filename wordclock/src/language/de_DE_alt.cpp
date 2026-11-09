#include <Arduino.h>

#include "de_DE_alt.h"

#include "../config.h"
#include "../dnd.h"
#include "../led.h"
#include "../display.h"

void Grid_de_DE_alt::setTime(int hour, int minute) { 
  if(hour == -1 || minute == -1) {
	return;
  }

  if(DND::active(hour, minute)) {
	for(int i = 0; i < NUM_LEDS; i++) {
	  Display::unmark(i);
	}

	Display::timeChanged(true);
	
	return;
  }
  
  int singleMinute = minute % 5;
  int hourLimit = 6;
  
  minute = (minute - (minute % 5));
  
  if(minute >= 15) {
	hour += 1;
  }

  minute = minute / 5;
  hour = hour % 12;

  for(int i = 0; i < NUM_LEDS; i++) {
	Display::unmark(i);
  }
  
  for(int i = 0; i < 5; i++) {
	Display::mark(Led::getLedId(Grid_de_DE_alt::time_it_is[i]));
  }

  for(int m = 0; m < 12; m++) {
	if(Grid_de_DE_alt::time_minutes[minute][m] >= 0) {
	  Display::mark(Led::getLedId(Grid_de_DE_alt::time_minutes[minute][m]));
	}
  }

  if(hour == 1 && minute == 0) {
	hourLimit = 3;
  }
  
  for(int h = 0; h < hourLimit; h++) {   
	if(Grid_de_DE_alt::time_hours[hour][h] >= 0) {
	  Display::mark(Led::getLedId(Grid_de_DE_alt::time_hours[hour][h]));
	}
  }

  if(GRID_SINGLE_MINUTES == 1) {
	// single minutes
	for(int s = (NUM_LEDS - EXTRA_LEDS); s < (NUM_LEDS - EXTRA_LEDS + singleMinute); s++) {
	  Display::mark(s);
	}
  } else {
	for(int s = 0; s < singleMinute; s++) {
	  Display::mark(s);
	}
  }
Display::timeChanged(false);
}

int Grid_de_DE_alt::time_it_is[5] = {0, 1, 3, 4, 5}; // es ist
	
int Grid_de_DE_alt::time_minutes[12][12] = {
  {107, 108, 109,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // uhr
  {  7,   8,   9,  10,  35,  36,  37,  38,  -1,  -1,  -1,  -1}, // fünf nach
  { 11,  12,  13,  14,  35,  36,  37,  38,  -1,  -1,  -1,  -1}, // zehn nach
  { 26,  27,  28,  29,  30,  31,  32,  -1,  -1,  -1,  -1,  -1}, // viertel
  { 11,  12,  13,  14,  39,  40,  41,  44,  45,  46,  47,  -1}, // zehn vor halb
  {  7,   8,   9,  10,  39,  40,  41,  44,  45,  46,  47,  -1}, // fünf vor halb
  { 44,  45,  46,  47,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1}, // halb
  {  7,   8,   9,  10,  35,  36,  37,  38,  44,  45,  46,  47}, // fünf nach halb
  { 11,  12,  13,  14,  35,  36,  37,  38,  44,  45,  46,  47}, // zehn nach halb
  { 22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  -1}, // dreiviertel			X
  { 11,  12,  13,  14,  39,  40,  41,  -1,  -1,  -1,  -1,  -1}, // zehn vor
  {  7,   8,   9,  10,  39,  40,  41,  -1,  -1,  -1,  -1,  -1}  // fünf vor
};

int Grid_de_DE_alt::time_hours[12][6] = {
  { 49,  50,  51,  52,  53,  -1}, // zwölf
  { 57,  58,  59,  60,  -1,  -1}, // eins
  { 55,  56,  57,  58,  -1,  -1}, // zwei
  { 67,  68,  69,  70,  -1,  -1}, // drei
  { 84,  85,  86,  87,  -1,  -1}, // vier
  { 73,  74,  75,  76,  -1,  -1}, // fünf
  {100, 101, 102, 103, 104,  -1}, // sechs
  { 60,  61,  62,  63,  64,  65}, // sieben
  { 89,  90,  91,  92,  -1,  -1}, // acht
  { 80,  81,  82,  83,  -1,  -1}, // neun
  { 93,  94,  95,  96,  -1,  -1}, // zehn
  { 77,  78,  79,  -1,  -1,  -1}  // elf
};
