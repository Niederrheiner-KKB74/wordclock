#include <ArduinoJson.h>

#include "config.h"
#include "grid.h"
#include "controller.h"
#include "httpServer.h"
#include "color.h"
#include "gui.h"
#include "wifi.h"
#include "timeSync.h"
#include "ota.h"
#include "led.h"
#include "display.h"
#include "symbols.h"

void Controller::index() {
  HttpServer::web.sendHeader("Location", "http://" + HttpServer::ip);
  Gui::sendIndex();
}

void Controller::saveColor() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json);

  Config::color_fg = Color::hexToRgb(doc["fg"].as<String>());
  Config::color_bg = Color::hexToRgb(doc["bg"].as<String>());
  Config::power_supply = doc["power_supply"].as<int>();
  Config::brightness =
    (doc["brightness"].as<double>() > Led::getMaxBrightnessPercnt()) ? Led::getMaxBrightnessPercnt() : doc["brightness"].as<double>();

  Config::healthcheck = doc["healthcheck"].as<int>() == 1;

  if (doc.containsKey("color_mode")) {
    Config::color_mode = constrain(doc["color_mode"].as<int>(), 0, COLOR_MODE_COUNT - 1);
    Config::color_fg2 = Color::hexToRgb(doc["fg2"].as<String>());
    Config::effect_speed = constrain(doc["effect_speed"].as<int>(), 1, 10);
  }

  Config::save();
  Display::invalidate();
  Grid::setTime(Time::hour, Time::minute);

  HttpServer::web.send(200, "text/html", "");
}

void Controller::saveTime() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json);

  String tz = doc["tz"].as<String>();
  String ntp = doc["ntp"].as<String>();
  tz.trim();
  ntp.trim();

  if (!Controller::isValidSetting(tz) || !Controller::isValidSetting(ntp)) {
    HttpServer::web.send(400, "text/plain", "Ungueltige Zeitzone oder NTP-Server");
    return;
  }

  Config::tz = tz;
  Config::ntp = ntp;

  Config::save();
  Time::applyConfig();
  Time::loop();

  HttpServer::web.send(200, "text/html", "");
}

// TZ rules and host names: printable ASCII, no quotes or backslashes, max. 63 chars.
bool Controller::isValidSetting(const String& value) {
  if (value.length() == 0 || value.length() > 63) {
    return false;
  }

  for (unsigned int i = 0; i < value.length(); i++) {
    char c = value.charAt(i);

    if (c < 0x21 || c > 0x7e || c == '"' || c == '\\' || c == '\'') {
      return false;
    }
  }

  return true;
}

void Controller::saveDnd() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, json);


  Config::dnd_active = doc["dnd_active"].as<int>() == 1;
  Config::dnd_start.hour = doc["dnd_start_hour"].as<int>();
  Config::dnd_start.minute = doc["dnd_start_minute"].as<int>();
  Config::dnd_end.hour = doc["dnd_end_hour"].as<int>();
  Config::dnd_end.minute = doc["dnd_end_minute"].as<int>();

  Config::save();
  Grid::setTime(Time::hour, Time::minute);

  HttpServer::web.send(200, "text/html", "");
}

// Removes control characters and limits the length (in bytes, UTF-8).
String Controller::cleanText(String text, unsigned int maxLength) {
  String out = "";

  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text.charAt(i);

    if ((uint8_t) c >= 0x20 && c != 0x7f) {
      out += c;
    }
  }

  out.trim();

  if (out.length() > maxLength) {
    // Do not cut a multi-byte character in half.
    unsigned int end = maxLength;
    while (end > 0 && (((uint8_t) out.charAt(end)) & 0xc0) == 0x80) {
      end--;
    }
    out = out.substring(0, end);
  }

  return out;
}

void Controller::saveOccasions() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(4096);
  if (deserializeJson(doc, json)) {
    HttpServer::web.send(400, "text/plain", "Ungueltige Daten");
    return;
  }

  Config::xmas = doc["xmas"].as<int>() == 1;
  Config::occasion_interval = constrain(doc["interval"].as<int>(), 10, 3600);

  JsonArray list = doc["occasions"];
  for (int i = 0; i < MAX_OCCASIONS; i++) {
    occasion_t& o = Config::occasions[i];

    if (i < (int) list.size()) {
      int day = list[i]["day"].as<int>();
      int month = list[i]["month"].as<int>();
      bool valid = day >= 1 && day <= 31 && month >= 1 && month <= 12;

      o.day = valid ? day : 0;
      o.month = valid ? month : 0;
      o.symbol = constrain(list[i]["symbol"].as<int>(), 0, SYMBOL_COUNT - 1);
      o.text = Controller::cleanText(String(list[i]["text"] | ""), TEXT_MAX_LENGTH);
    } else {
      o.day = 0;
      o.month = 0;
      o.symbol = SYMBOL_NONE;
      o.text = "";
    }
  }

  Config::save();

  HttpServer::web.send(200, "text/html", "");
}

void Controller::showText() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, json)) {
    HttpServer::web.send(400, "text/plain", "Ungueltige Daten");
    return;
  }

  String text = Controller::cleanText(String(doc["text"] | ""), TEXT_MAX_LENGTH);

  if (doc.containsKey("speed")) {
    int speed = constrain(doc["speed"].as<int>(), 1, 3);

    if (speed != Config::text_speed) {
      Config::text_speed = speed;
      Config::save();
    }
  }

  if (text.length() == 0) {
    HttpServer::web.send(400, "text/plain", "Bitte einen Text eingeben");
    return;
  }

  int repeat = doc.containsKey("repeat") ? constrain(doc["repeat"].as<int>(), 0, 99) : 1;
  Display::showText(text, repeat);

  HttpServer::web.send(200, "text/html", "");
}

void Controller::stopText() {
  Display::stopText();
  HttpServer::web.send(200, "text/html", "");
}

void Controller::preview() {
  String json = HttpServer::web.arg("plain");

  DynamicJsonDocument doc(1024);
  if (deserializeJson(doc, json)) {
    HttpServer::web.send(400, "text/plain", "Ungueltige Daten");
    return;
  }

  uint8_t symbol = constrain(doc["symbol"].as<int>(), 0, SYMBOL_COUNT - 1);
  String text = Controller::cleanText(String(doc["text"] | ""), TEXT_MAX_LENGTH);

  if (symbol == SYMBOL_NONE && text.length() == 0) {
    HttpServer::web.send(400, "text/plain", "Kein Bild und kein Text");
    return;
  }

  Display::preview(symbol, text);

  HttpServer::web.send(200, "text/html", "");
}

void Controller::deleteWiFi() {
  Wifi::reset();
  ESP.restart();
  HttpServer::web.send(200, "text/html", "");
}

void Controller::firmware() {
  if (!Ota::authenticate()) {
    return;
  }

  HttpServer::web.send(200, "text/html", Gui::firmware());
}

void Controller::getLogoSvg() {
  String content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><svg width=\"1024\" height=\"1024\" version=\"1.1\" viewBox=\"0 0 270.93 270.93\" xmlns=\"http://www.w3.org/2000/svg\"><rect x=\".13216\" y=\".13216\" width=\"270.67\" height=\"270.67\" rx=\"26.433\" ry=\"26.433\" fill=\"#333\"/><rect x=\"15.992\" y=\"15.992\" width=\"238.95\" height=\"238.95\" rx=\"23.335\" ry=\"23.335\" fill=\"#eee\"/><g fill=\"#3d72a8\"><circle cx=\"90.488\" cy=\"50.271\" r=\"13.098\"/><circle cx=\"47.096\" cy=\"50.271\" r=\"13.098\"/><circle cx=\"90.488\" cy=\"93.662\" r=\"13.098\"/><circle cx=\"133.88\" cy=\"93.662\" r=\"13.098\"/><circle cx=\"177.4\" cy=\"93.662\" r=\"13.098\"/><circle cx=\"133.88\" cy=\"180.45\" r=\"13.098\"/><circle cx=\"177.4\" cy=\"180.45\" r=\"13.098\"/><circle cx=\"220.79\" cy=\"180.45\" r=\"13.098\"/><circle cx=\"47.096\" cy=\"180.45\" r=\"13.098\"/></g></svg>";

  HttpServer::web.send(200, "image/svg+xml", content);
}

void Controller::getManifest() {
  String content = "";

  content += "{";
  content += "\"background_color\": \"#FFFFFF\",";
  content += "\"description\": \"panbachi.de WordClock\",";
  content += "\"dir\": \"ltr\",";
  content += "\"display\": \"standalone\",";
  content += "\"icons\": [{\"sizes\": \"128x128\", \"src\": \"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIAAAACACAMAAAD04JH5AAAAXVBMVEUzMzMAAAAzMzMyMjIzMzMzMzMyMjI1NTXu7u4zMzM9cqiIiIhIeq3AztyZss1kjrji5uh8nsFzmb7T2+SvwtWmvNJWhLPM1+FPT0/Hx8erq6ujo6NycnJcXFyKqMdkRsj5AAAACHRSTlPwANm7MYVREuXaafsAAAKuSURBVHja7NHJeoMwDEVhx2aQHXkgDAEC7/+aTRqD+KDtKjKb/su74WCJS5TlSopEpMqzSxQDCqkTk8UmoFT6BKpcAjKpTyGzd0Ap9Ulk+R2g9GnUK6DQJyqeAbsDzNOV0TTvjnARmd7oxwDMwtjrjUzkmjwgiYcmuVD0+wMkMtAjKCH1YoBkBr2QQqR8/+MVxBrQQ1L9IWCEpMZDQICkwj5ghsTmXcAEG3XTWtizbVPD50y7gCus7t48oYMth6/R3+FTrr8GWDRvNyC3uKFlDwhoFo7+f90wcAc4s0JYII2OO8AbYuNmDfHcAWhIG7fWEOQO6Axp4tYY0nEHVIbUcasNqbgD7E/n9jRa7gB6AqxhcUd6AOYAKkALxCJ9nz0AbNWhdwG2gvPYVRb4A/72H/DFjh3qAAjDQBi2E5BAN5C8/2PiGMlPmolT7OxS8an2bgYYMDEAiyjdWLEqAVzF3Nm4GiED8BjxavWsXJ/BEAF4jtO7Hf2xKQBZIImP5LK93kIHYCRjdmN6XTQAhtIsvTY9gLF8NL+fGgCLyWiDuTQAVrO0wx0YFABQTrMWu/dBAYD1vJYy2uO1AH5Q5D8ZP7yGBhhggAEGGHC3b8YqCANREGytRCIhTbj//0zLHIwcTKGrsFuGBxnSBOa9LUABClCAAhSgAFcCGxNEiEoM6p0R40XlwOACAP4REaLyej8IxN4QkaLywOACAP6R0aJyTA8PvTtmhKikz9v19pwRopKkd30/wGhRueFTmQsKRovKfRp86BsSRohKUA1/RcN4UXlgcAEA/8h4UfncOEgA+kdGiEoO/tHf8F0KUIACFKAABShAAT4OcN6+nPPXzvvjBYd8xSNecknVfPJFp0jVa2oc5stu8bpfvPCYr3zGS6/52m+8+JyvfufL78n6/wsOPRSv+uhUewAAAABJRU5ErkJggg==\", \"type\": \"image/png\"}],";
  content += "\"lang\": \"en-US\",";
  content += "\"name\": \"panbachi.de WordClock\",";
  content += "\"short_name\": \"WordClock\",";
  content += "\"start_url\": \"/\",";
  content += "\"theme_color\": \"#3d72a8\"";
  content += "}";

  HttpServer::web.send(200, "application/json", content);
}

void Controller::notFound() {
  if(HttpServer::web.method() == HTTP_OPTIONS) {
    HttpServer::web.sendHeader("Allow", "DELETE,GET,HEAD,OPTIONS,POST,PUT");
    HttpServer::web.send(200, "text/html", "");

    return;
  }

  HttpServer::web.send(404, "text/html", "NOT FOUND");
}
