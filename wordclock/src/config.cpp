#include <FS.h>
#include <ArduinoJson.h>

#include "config.h"
#include "types.h"
#include "timeSync.h"
#include "led.h"

void Config::save() {
  File file = SPIFFS.open("/wordclock_config.json", "w");

  if(!file) {
    Serial.println("Can't open wordclock_config.json for writing");
    return;
  }

  Serial.println("Save config.");

  DynamicJsonDocument doc(4096);
  doc["color_bg_r"] = Config::color_bg.r;
  doc["color_bg_g"] = Config::color_bg.g;
  doc["color_bg_b"] = Config::color_bg.b;
  doc["color_fg_r"] = Config::color_fg.r;
  doc["color_fg_g"] = Config::color_fg.g;
  doc["color_fg_b"] = Config::color_fg.b;
  doc["power_supply"] = Config::power_supply;
  doc["brightness"] = Config::brightness;
  doc["tz_posix"] = Config::tz;
  doc["dnd_active"] = Config::dnd_active;
  doc["dnd_start_hour"] = Config::dnd_start.hour;
  doc["dnd_start_minute"] = Config::dnd_start.minute;
  doc["dnd_end_hour"] = Config::dnd_end.hour;
  doc["dnd_end_minute"] = Config::dnd_end.minute;
  doc["dnd_mode"] = Config::dnd_mode;
  doc["dnd_brightness"] = Config::dnd_brightness;
  doc["dnd_color_r"] = Config::dnd_color.r;
  doc["dnd_color_g"] = Config::dnd_color.g;
  doc["dnd_color_b"] = Config::dnd_color.b;
  doc["ntp"] = Config::ntp;
  doc["healthcheck"] = Config::healthcheck;
  doc["color_mode"] = Config::color_mode;
  doc["color_fg2_r"] = Config::color_fg2.r;
  doc["color_fg2_g"] = Config::color_fg2.g;
  doc["color_fg2_b"] = Config::color_fg2.b;
  doc["effect_speed"] = Config::effect_speed;
  doc["text_speed"] = Config::text_speed;
  doc["xmas"] = Config::xmas;
  doc["occasion_interval"] = Config::occasion_interval;

  JsonArray occasions = doc.createNestedArray("occasions");
  for (int i = 0; i < MAX_OCCASIONS; i++) {
    JsonObject o = occasions.createNestedObject();
    o["d"] = Config::occasions[i].day;
    o["m"] = Config::occasions[i].month;
    o["s"] = Config::occasions[i].symbol;
    o["t"] = Config::occasions[i].text;
  }

  serializeJson(doc, file);

  file.close();
}

void Config::load() {
  Config::color_bg.r = 0;
  Config::color_bg.g = 0;
  Config::color_bg.b = 0;

  Config::color_fg.r = 255;
  Config::color_fg.g = 255;
  Config::color_fg.b = 255;

  Config::power_supply = 500; // default: 500mA (USB 2.0 specification)
  Config::brightness = 0.5;

  Config::tz = TZ_DEFAULT;

  Config::dnd_active = false;
  Config::dnd_start.hour = -1;
  Config::dnd_start.minute = -1;
  Config::dnd_end.hour = -1;
  Config::dnd_end.minute = -1;
  Config::dnd_mode = DND_MODE_OFF;
  Config::dnd_brightness = 0.05;
  Config::dnd_color.r = 255;
  Config::dnd_color.g = 60;
  Config::dnd_color.b = 0;
  Config::ntp = "pool.ntp.org";

  Config::healthcheck = false;

  Config::color_mode = COLOR_MODE_STATIC;
  Config::color_fg2.r = 0;
  Config::color_fg2.g = 80;
  Config::color_fg2.b = 255;
  Config::effect_speed = 3;
  Config::text_speed = 2;
  Config::xmas = true;
  Config::occasion_interval = 60;

  for (int i = 0; i < MAX_OCCASIONS; i++) {
    Config::occasions[i].day = 0;
    Config::occasions[i].month = 0;
    Config::occasions[i].symbol = 0;
    Config::occasions[i].text = "";
  }

  File file = SPIFFS.open("/wordclock_config.json", "r");

  if(!file) {
    Serial.println("Failed to open config file.");
    Config::save();
    return;
  }

  Serial.println("Load config.");

  DynamicJsonDocument doc(4096);
  deserializeJson(doc, file);

  Config::color_bg.r = doc["color_bg_r"].as<int>();
  Config::color_bg.g = doc["color_bg_g"].as<int>();
  Config::color_bg.b = doc["color_bg_b"].as<int>();

  Config::color_fg.r = doc["color_fg_r"].as<int>();
  Config::color_fg.g = doc["color_fg_g"].as<int>();
  Config::color_fg.b = doc["color_fg_b"].as<int>();

  if(doc["power_supply"]) {
    Config::power_supply = doc["power_supply"].as<int>();
  }

  if(doc["brightness"]) {
    Config::brightness =
      (doc["brightness"].as<double>() > Led::getMaxBrightnessPercnt()) ? Led::getMaxBrightnessPercnt() : doc["brightness"].as<double>();
  }

  // Configs saved by older firmware versions only contain a fixed UTC offset
  // ("timezone"); they keep TZ_DEFAULT until a rule is saved in the web interface.
  if (doc["tz_posix"]) {
    Config::tz = doc["tz_posix"].as<String>();
  }

  Config::dnd_active = doc["dnd_active"].as<bool>();
  Config::dnd_start.hour = doc["dnd_start_hour"].as<int>();
  Config::dnd_start.minute = doc["dnd_start_minute"].as<int>();
  Config::dnd_end.hour = doc["dnd_end_hour"].as<int>();
  Config::dnd_end.minute = doc["dnd_end_minute"].as<int>();

  // Added in 4.3.0 - older configs switch the clock off at night as before.
  if (doc.containsKey("dnd_mode")) {
    Config::dnd_mode = constrain(doc["dnd_mode"].as<int>(), 0, DND_MODE_COUNT - 1);
    Config::dnd_brightness = constrain(doc["dnd_brightness"].as<double>(), 0.01, 1.0);
    Config::dnd_color.r = doc["dnd_color_r"].as<int>();
    Config::dnd_color.g = doc["dnd_color_g"].as<int>();
    Config::dnd_color.b = doc["dnd_color_b"].as<int>();
  }

  if(doc["ntp"]) {
    Config::ntp = doc["ntp"].as<String>();
  }

  if(doc["healthcheck"]){
    Config::healthcheck = doc["healthcheck"].as<bool>();
  }

  // Settings added in 4.2.0 - older configs keep the defaults above.
  if (doc.containsKey("color_mode")) {
    Config::color_mode = constrain(doc["color_mode"].as<int>(), 0, COLOR_MODE_COUNT - 1);
    Config::color_fg2.r = doc["color_fg2_r"].as<int>();
    Config::color_fg2.g = doc["color_fg2_g"].as<int>();
    Config::color_fg2.b = doc["color_fg2_b"].as<int>();
    Config::effect_speed = constrain(doc["effect_speed"].as<int>(), 1, 10);
  }

  if (doc.containsKey("text_speed")) {
    Config::text_speed = constrain(doc["text_speed"].as<int>(), 1, 3);
  }

  if (doc.containsKey("xmas")) {
    Config::xmas = doc["xmas"].as<bool>();
    Config::occasion_interval = constrain(doc["occasion_interval"].as<int>(), 10, 3600);
  }

  JsonArray occasions = doc["occasions"];
  for (int i = 0; i < MAX_OCCASIONS && i < (int) occasions.size(); i++) {
    Config::occasions[i].day = occasions[i]["d"].as<int>();
    Config::occasions[i].month = occasions[i]["m"].as<int>();
    Config::occasions[i].symbol = occasions[i]["s"].as<int>();
    Config::occasions[i].text = String(occasions[i]["t"] | "");
  }

  file.close();
}

color_t Config::color_bg{};
color_t Config::color_fg{};
int Config::power_supply{};
double Config::brightness{};
String Config::tz{};
bool Config::dnd_active{};
clock_time_t Config::dnd_start{};
clock_time_t Config::dnd_end{};
int Config::dnd_mode{};
double Config::dnd_brightness{};
color_t Config::dnd_color{};
String Config::ntp{};
bool Config::healthcheck{};
int Config::color_mode{};
color_t Config::color_fg2{};
int Config::effect_speed{};
int Config::text_speed{};
bool Config::xmas{};
int Config::occasion_interval{};
occasion_t Config::occasions[MAX_OCCASIONS];
