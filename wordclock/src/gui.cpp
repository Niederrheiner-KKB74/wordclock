#include <Arduino.h>
#include <time.h>

#include "gui.h"
#include "httpServer.h"
#include "color.h"
#include "config.h"
#include "led.h"
#include "ota.h"
#include "timeSync.h"
#include "symbols.h"

static const char* const COLOR_MODES[] = {
  "Einfarbig",
  "Farbwechsel (langsam durch das Spektrum)",
  "Regenbogen (wandert über die Buchstaben)",
  "Farbverlauf (oben Vordergrundfarbe, unten zweite Farbe)",
  "Farbwelle (zwei Farben wandern über die Buchstaben)"
};

// Common time zones as POSIX TZ rules (label, rule).
static const char* const TZ_PRESETS[][2] = {
  {"Deutschland, Österreich, Schweiz (MEZ/MESZ)", "CET-1CEST,M3.5.0,M10.5.0/3"},
  {"Großbritannien, Irland, Portugal (WEZ/WESZ)", "GMT0BST,M3.5.0/1,M10.5.0"},
  {"Finnland, Griechenland, Rumänien (OEZ/OESZ)", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
  {"Türkei (UTC+3)", "<+03>-3"},
  {"Russland, Moskau (UTC+3)", "MSK-3"},
  {"USA Ostküste (New York)", "EST5EDT,M3.2.0,M11.1.0"},
  {"USA Central (Chicago)", "CST6CDT,M3.2.0,M11.1.0"},
  {"USA Mountain (Denver)", "MST7MDT,M3.2.0,M11.1.0"},
  {"USA Westküste (Los Angeles)", "PST8PDT,M3.2.0,M11.1.0"},
  {"UTC", "UTC0"}
};

String Gui::pad(int value) {
  if(value < 10) {
    return "0" + String(value);
  }

  return String(value);
}

String Gui::htmlOption(const String& label, const String& value, const String& store) {
  String content = "<option value=\"" + value + "\"";
  if (store == value) {
    content += " selected=\"selected\"";
  }

  content += ">" + label + "</option>";

  return content;
}

String Gui::escape(const String& value) {
  String out = value;
  out.replace("&", "&amp;");
  out.replace("<", "&lt;");
  out.replace(">", "&gt;");
  out.replace("\"", "&quot;");

  return out;
}

String Gui::getTimeStatus() {
  if (!Time::isSynced()) {
    return "Noch keine Zeit vom NTP-Server empfangen.";
  }

  time_t now = time(nullptr);
  struct tm local;
  localtime_r(&now, &local);

  String status = "Uhrzeit der WordClock: " + Gui::pad(local.tm_hour) + ":" + Gui::pad(local.tm_min);
  status += (local.tm_isdst > 0) ? " (Sommerzeit)" : " (Normalzeit)";

  return status;
}

String Gui::createStyleSheet() {
  String content = "";

  content += "body { font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; background-color: #eee; color: #333;}";
  content += "nav { position: fixed; top: 0; left: 0; right: 0; background-color: #3d72a8; list-style: none; margin: 0; display: flex;}";
  content += "nav li  {margin: 0; padding: 10px; flex: 1; text-align: center; color: #eee; cursor: pointer; }";
  content += "nav li.icon { padding: 6px; width: 50px; flex-grow: 0; flex-shrink: 0; flex-basis: 50px; cursor: inherit; }";
  content += "nav li.icon img { vertical-align: middle; }";
  content += "nav li.active { color: #ccc; }";
  content += "nav li svg path { fill: #eee; }";
  content += "nav li.active svg path { fill: #ccc; }";
  content += "nav.disabled li:not(.icon) { display: none; }";
  content += "main { margin-top: 60px; margin-bottom: 70px; }";
  content += "section { display: none; }";
  content += "section.active { display: block; }";
  content += "section div { color: #3d72a8; margin-bottom: 20px; }";
  content += "section div label { display: block; font-size: 0.8em; margin-bottom: 5px; }";
  content += "section div input { display: block; background-color:#fff; width: 100%; border: 0; padding: 0; margin: 0; border-bottom: 1px solid #3d72a8; height: 30px; }";
  content += "section div select { display: block; background-color:#fff; width: 100%; border: 0; padding: 0; margin: 0; border-bottom: 1px solid #3d72a8; height: 30px; }";
  content += "section div input[type=\"color\"] { border: 0; }";
  content += "section div div.time { display: flex; }";
  content += "section div div.time span { flex-basis: 20px; text-align: center; font-weight: bold; font-size: 1.3em; }";
  content += "section div div.time select { display: inline-block; flex: 1; }";
  content += "#reset_wifi { background-color: #3d72a8; border: 0; width: 100%; color: #eee; padding: 10px; cursor: pointer; }";
  content += "#reset_wifi_message { display: none; text-align: center;}";
  content += "p.info { color: #333; font-size: 0.9em; margin: 0 0 20px 0; }";
  content += "fieldset { border: 0; border-top: 2px solid #3d72a8; padding: 10px 0 0 0; margin: 0 0 25px 0; }";
  content += "legend { color: #3d72a8; font-weight: bold; padding-right: 8px; }";
  content += "button.action { display: block; width: 100%; background-color: #3d72a8; color: #eee; border: 0; padding: 10px; margin-bottom: 10px; font-size: 1em; cursor: pointer; }";
  content += "button.secondary { background-color: #fff; color: #3d72a8; border: 1px solid #3d72a8; }";
  content += "button.preview { background-color: #fff; color: #3d72a8; border: 1px solid #3d72a8; padding: 6px 12px; cursor: pointer; }";
  content += "a.button { display: block; background-color: #3d72a8; color: #eee; padding: 10px; text-align: center; text-decoration: none; }";
  content += "footer { position: fixed; bottom: 0; left: 0; right: 0; background-color: #3d72a8; text-align: right; }";
  content += "footer button { background-color: #3d72a8; color: #eee; border: 0; padding: 15px; text-transform: uppercase; font-weight: bold; }";
  content += "footer button:hover,footer button:active { color: #ccc; cursor: pointer; }";

  return content;
}

String Gui::createNav() {
  String content = "";

  content += "<nav>";
  content += "<li class=\"icon\"><img src=\"/logo.svg\" width=\"32\"></li>";
  content += "<li class=\"active\" name=\"color\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M17.5,12A1.5,1.5 0 0,1 16,10.5A1.5,1.5 0 0,1 17.5,9A1.5,1.5 0 0,1 19,10.5A1.5,1.5 0 0,1 17.5,12M14.5,8A1.5,1.5 0 0,1 13,6.5A1.5,1.5 0 0,1 14.5,5A1.5,1.5 0 0,1 16,6.5A1.5,1.5 0 0,1 14.5,8M9.5,8A1.5,1.5 0 0,1 8,6.5A1.5,1.5 0 0,1 9.5,5A1.5,1.5 0 0,1 11,6.5A1.5,1.5 0 0,1 9.5,8M6.5,12A1.5,1.5 0 0,1 5,10.5A1.5,1.5 0 0,1 6.5,9A1.5,1.5 0 0,1 8,10.5A1.5,1.5 0 0,1 6.5,12M12,3A9,9 0 0,0 3,12A9,9 0 0,0 12,21A1.5,1.5 0 0,0 13.5,19.5C13.5,19.11 13.35,18.76 13.11,18.5C12.88,18.23 12.73,17.88 12.73,17.5A1.5,1.5 0 0,1 14.23,16H16A5,5 0 0,0 21,11C21,6.58 16.97,3 12,3Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"time\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M12,20A8,8 0 0,0 20,12A8,8 0 0,0 12,4A8,8 0 0,0 4,12A8,8 0 0,0 12,20M12,2A10,10 0 0,1 22,12A10,10 0 0,1 12,22C6.47,22 2,17.5 2,12A10,10 0 0,1 12,2M12.5,7V12.25L17,14.92L16.25,16.15L11,13V7H12.5Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"dnd\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M17.75,4.09L15.22,6.03L16.13,9.09L13.5,7.28L10.87,9.09L11.78,6.03L9.25,4.09L12.44,4L13.5,1L14.56,4L17.75,4.09M21.25,11L19.61,12.25L20.2,14.23L18.5,13.06L16.8,14.23L17.39,12.25L15.75,11L17.81,10.95L18.5,9L19.19,10.95L21.25,11M18.97,15.95C19.8,15.87 20.69,17.05 20.16,17.8C19.84,18.25 19.5,18.67 19.08,19.07C15.17,23 8.84,23 4.94,19.07C1.03,15.17 1.03,8.83 4.94,4.93C5.34,4.53 5.76,4.17 6.21,3.85C6.96,3.32 8.14,4.21 8.06,5.04C7.79,7.9 8.75,10.87 10.95,13.06C13.14,15.26 16.1,16.22 18.97,15.95M17.33,17.97C14.5,17.81 11.7,16.64 9.53,14.5C7.36,12.31 6.2,9.5 6.04,6.68C3.23,9.82 3.34,14.64 6.35,17.66C9.37,20.67 14.19,20.78 17.33,17.97Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"occasions\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M12,6C13.11,6 14,5.1 14,4C14,3.62 13.9,3.27 13.71,2.97L12,0L10.29,2.97C10.1,3.27 10,3.62 10,4A2,2 0 0,0 12,6M16.6,16L15.53,14.92L14.45,16C13.15,17.29 10.87,17.3 9.56,16L8.5,14.92L7.4,16C6.75,16.65 5.88,17 4.96,17C4.23,17 3.56,16.77 3,16.39V21A1,1 0 0,0 4,22H20A1,1 0 0,0 21,21V16.39C20.44,16.77 19.77,17 19.04,17C18.12,17 17.25,16.65 16.6,16M18,9H13V7H11V9H6A3,3 0 0,0 3,12V13.54C3,14.62 3.88,15.5 4.96,15.5C5.5,15.5 6,15.3 6.34,14.93L8.5,12.8L10.61,14.93C11.35,15.67 12.64,15.67 13.38,14.93L15.5,12.8L17.65,14.93C18,15.3 18.5,15.5 19.03,15.5C20.11,15.5 21,14.62 21,13.54V12A3,3 0 0,0 18,9Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"text\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M20,2H4A2,2 0 0,0 2,4V22L6,18H20A2,2 0 0,0 22,16V4C22,2.89 21.1,2 20,2M6,9H18V11H6M14,14H6V12H14M18,8H6V6H18\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"wifi\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M12,21L15.6,16.2C14.6,15.45 13.35,15 12,15C10.65,15 9.4,15.45 8.4,16.2L12,21M12,3C7.95,3 4.21,4.34 1.2,6.6L3,9C5.5,7.12 8.62,6 12,6C15.38,6 18.5,7.12 21,9L22.8,6.6C19.79,4.34 16.05,3 12,3M12,9C9.3,9 6.81,9.89 4.8,11.4L6.6,13.8C8.1,12.67 9.97,12 12,12C14.03,12 15.9,12.67 17.4,13.8L19.2,11.4C17.19,9.89 14.7,9 12,9Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "<li name=\"firmware\">";
  content += "<svg style=\"width:24px;height:24px\" viewBox=\"0 0 24 24\">";
  content += "<path fill=\"#eee\" d=\"M9,16V10H5L12,3L19,10H15V16H9M5,20V18H19V20H5Z\" />";
  content += "</svg>";
  content += "</li>";
  content += "</nav>";

  return content;
}

String Gui::createScript() {
  String content = "";

  content += "window.onload=function(){";
  content += "var e=function(id){return document.getElementById(id)};";
  content += "var nav=document.querySelector('nav'),tabs=nav.querySelectorAll('li'),sections=document.querySelectorAll('main section');";
  content += "var save=e('save'),resetWifi=e('reset_wifi'),resetMsg=e('reset_wifi_message'),current='color';";
  content += "var put=function(url,d,ok){fetch(url,{method:'PUT',headers:{'Content-Type':'application/json'},body:JSON.stringify(d)})";
  content += ".then(function(r){if(!r.ok)return r.text().then(function(t){alert('Fehler: '+t)});if(ok)ok();})";
  content += ".catch(function(){alert('Die WordClock ist nicht erreichbar.')});};";
  content += "tabs.forEach(function(tab){tab.onclick=function(ev){var t=ev.currentTarget;if(!t.hasAttribute('name'))return;";
  content += "current=t.getAttribute('name');";
  content += "tabs.forEach(function(x){x.classList.remove('active')});t.classList.add('active');";
  content += "sections.forEach(function(x){x.classList.remove('active')});e(current).classList.add('active');";
  content += "save.style.display=(current=='wifi'||current=='firmware'||current=='text')?'none':'inline-block';}});";
  content += "var preset=e('tz_preset'),tz=e('tz');";
  content += "preset.onchange=function(){if(preset.value!='custom')tz.value=preset.value;};";
  content += "tz.oninput=function(){preset.value='custom';for(var i=0;i<preset.options.length;i++){if(preset.options[i].value==tz.value)preset.value=tz.value;}};";
  content += "var opts=function(attr,first,last,label){document.querySelectorAll('select['+attr+']').forEach(function(s){var h='';";
  content += "for(var i=first;i<=last;i++)h+='<option value=\"'+i+'\">'+label(i)+'</option>';s.innerHTML=h;s.value=s.getAttribute(attr);});};";
  content += "var months=['Januar','Februar','März','April','Mai','Juni','Juli','August','September','Oktober','November','Dezember'];";
  content += "opts('data-days',0,31,function(i){return i?i+'.':'–'});opts('data-months',1,12,function(i){return months[i-1]});";
  content += "var mode=e('color_mode'),upd=function(){var m=+mode.value;";
  content += "e('fg_row').style.display=(m==1||m==2)?'none':'block';";
  content += "e('fg2_row').style.display=(m>=3)?'block':'none';";
  content += "e('speed_row').style.display=(m==0||m==3)?'none':'block';};";
  content += "mode.onchange=upd;upd();";
  content += "save.onclick=function(){var d={};";
  content += "if(current=='color'){d.fg=e('fg').value;d.bg=e('bg').value;d.power_supply=e('power_supply').value;d.brightness=e('brightness').value;d.healthcheck=e('healthcheck').value;";
  content += "d.color_mode=mode.value;d.fg2=e('fg2').value;d.effect_speed=e('effect_speed').value;}";
  content += "else if(current=='time'){d.tz=tz.value;d.ntp=e('ntp').value;}";
  content += "else if(current=='dnd'){d.dnd_active=e('dnd_active').value;d.dnd_start_hour=e('dnd_s_h').value;d.dnd_start_minute=e('dnd_s_m').value;d.dnd_end_hour=e('dnd_e_h').value;d.dnd_end_minute=e('dnd_e_m').value;}";
  content += "else if(current=='occasions'){d.xmas=e('xmas').value;d.interval=e('interval').value;d.occasions=[];";
  content += "for(var i=0;i<" + String(MAX_OCCASIONS) + ";i++){d.occasions.push({day:e('o'+i+'_d').value,month:e('o'+i+'_m').value,symbol:e('o'+i+'_s').value,text:e('o'+i+'_t').value});}}";
  content += "else return;";
  content += "put('/api/'+current,d,function(){if(current=='time')location.reload();});};";
  content += "document.querySelectorAll('button.preview').forEach(function(b){b.onclick=function(){var i=b.getAttribute('data-i');";
  content += "if(i=='xmas')put('/api/preview',{symbol:" + String((int) SYMBOL_TREE) + ",text:'Frohe Weihnachten'});";
  content += "else put('/api/preview',{symbol:e('o'+i+'_s').value,text:e('o'+i+'_t').value});};});";
  content += "e('text_show').onclick=function(){put('/api/text',{text:e('text_value').value,repeat:e('text_repeat').value,speed:e('text_speed').value});};";
  content += "e('text_stop').onclick=function(){fetch('/api/text',{method:'DELETE'});};";
  content += "resetWifi.onclick=function(){fetch('/api/wifi',{method:'DELETE',headers:{'Content-Type':'application/json'},body:''});";
  content += "nav.classList.add('disabled');resetWifi.style.display='none';resetMsg.style.display='block';};";
  content += "};";

  return content;
}

String Gui::createContent() {
  String content = "";

  content += "<main>";

  content += "<section id=\"color\" class=\"active\">";
  content += "<div><label>Farbmodus</label><select id=\"color_mode\">";
  for (int i = 0; i < COLOR_MODE_COUNT; i++) {
    content += Gui::htmlOption(COLOR_MODES[i], String(i), String(Config::color_mode));
  }
  content += "</select></div>";
  content += "<div id=\"fg_row\"><label>Vordergrundfarbe</label><input id=\"fg\" value=\"#" + Color::rgbToHex(Config::color_fg) + "\" type=\"color\"></div>";
  content += "<div id=\"fg2_row\"><label>Zweite Farbe</label><input id=\"fg2\" value=\"#" + Color::rgbToHex(Config::color_fg2) + "\" type=\"color\"></div>";
  content += "<div id=\"speed_row\"><label>Geschwindigkeit</label><select id=\"effect_speed\">";
  for (int i = 1; i <= 10; i++) {
    String label = String(i);
    if (i == 1) label += " (sehr langsam)";
    if (i == 10) label += " (schnell)";
    content += Gui::htmlOption(label, String(i), String(Config::effect_speed));
  }
  content += "</select></div>";
  content += "<div><label>Hintergrundfarbe</label><input id=\"bg\" value=\"#" + Color::rgbToHex(Config::color_bg) + "\" type=\"color\"></div>";
  content += "<div><label>Stromversorgung in mA</label><input id=\"power_supply\" type=\"number\" min=0 step=\"100\" value=\"" + String(Config::power_supply) + "\"></div>";
  content += "<div>";
  content += "<label>Helligkeit</label>";
  content += "<select id=\"brightness\">";

  for (double brightness_percnt = 0.0; brightness_percnt < Led::getMaxBrightnessPercnt(); brightness_percnt+=0.05) {
    String label = String((int) (brightness_percnt * 100 + 0.5)) + "&percnt;"; // adding 0.5 for rounding.
    content += Gui::htmlOption(label, String(brightness_percnt), String(Config::brightness));
  }
  if (Led::getMaxBrightnessPercnt() < 1.0) { // show maximum brightness percentage between the 5% steps.
    String label = String((int) (Led::getMaxBrightnessPercnt() * 100 + 0.5)) + "&percnt;";
    content += Gui::htmlOption(label, String(Led::getMaxBrightnessPercnt()), String(Config::brightness));
  }

  content += "</select>";
  content += "<div><label>LED check bei Start durchführen</label><select id=\"healthcheck\">";
  content += Gui::htmlOption("Inaktiv", String(0), String(Config::healthcheck));
  content += Gui::htmlOption("Aktiv", String(1), String(Config::healthcheck));
  content += "</select></div>";

  content += "</div>";
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"time\">";
  content += "<p class=\"info\">" + Gui::getTimeStatus() + "</p>";
  content += "<div><label>Zeitzone</label><select id=\"tz_preset\">";

  bool isPreset = false;
  for (unsigned int i = 0; i < sizeof(TZ_PRESETS) / sizeof(TZ_PRESETS[0]); i++) {
    String rule = TZ_PRESETS[i][1];
    isPreset = isPreset || (rule == Config::tz);
    content += Gui::htmlOption(TZ_PRESETS[i][0], Gui::escape(rule), Gui::escape(Config::tz));
  }

  content += "<option value=\"custom\"" + String(isPreset ? "" : " selected=\"selected\"") + ">Eigene Regel</option>";
  content += "</select></div>";
  content += "<div><label>Zeitzonenregel (POSIX-Format, Sommer-/Winterzeit wird automatisch umgestellt)</label>";
  content += "<input id=\"tz\" type=\"text\" maxlength=\"63\" value=\"" + Gui::escape(Config::tz) + "\"></div>";
  content += "<div><label>NTP-Server</label><input id=\"ntp\" type=\"text\" maxlength=\"63\" value=\"" + Gui::escape(Config::ntp) + "\"></div>";
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"dnd\">";
  content += "<div><label>Nachtmodus</label><select id=\"dnd_active\">";
  content += Gui::htmlOption("Inaktiv", String(0), String(Config::dnd_active));
  content += Gui::htmlOption("Aktiv", String(1), String(Config::dnd_active));
  content += "</select></div>";
  content += "<div><label>Start</label><div class=\"time\">";
  content += "<select id=\"dnd_s_h\">";

  for(int i = 0; i < 24; i++) {
    content += Gui::htmlOption(Gui::pad(i), String(i), String(Config::dnd_start.hour));
  }

  content += "</select><span>:</span><select id=\"dnd_s_m\">";

  for(int i = 0; i < 60; i = i + 5) {
    content += Gui::htmlOption(Gui::pad(i), String(i), String(Config::dnd_start.minute));
  }

  content += "</select></div></div>";
  content += "<div><label>Ende</label><div class=\"time\"><select id=\"dnd_e_h\">";

  for(int i = 0; i < 24; i++) {
    content += Gui::htmlOption(Gui::pad(i), String(i), String(Config::dnd_end.hour));
  }

  content += "</select><span>:</span><select id=\"dnd_e_m\">";

  for(int i = 0; i < 60; i = i + 5) {
    content += Gui::htmlOption(Gui::pad(i), String(i), String(Config::dnd_end.minute));
  }

  content += "</select></div></div>";
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"occasions\">";
  content += "<p class=\"info\">An diesen Tagen zeigt die Uhr im Wechsel mit der Uhrzeit ein Bild und danach den Text als Laufschrift.</p>";
  content += "<div><label>Wechsel: Uhrzeit anzeigen für</label><select id=\"interval\">";
  {
    const int intervals[] = {15, 30, 60, 120, 300, 600};
    const char* const labels[] = {"15 Sekunden", "30 Sekunden", "1 Minute", "2 Minuten", "5 Minuten", "10 Minuten"};
    for (int i = 0; i < 6; i++) {
      content += Gui::htmlOption(labels[i], String(intervals[i]), String(Config::occasion_interval));
    }
  }
  content += "</select></div>";
  content += "<fieldset><legend>Weihnachten</legend>";
  content += "<div><label>Weihnachtsbaum vom 1. Advent bis 26.12. (ab 24.12. mit &bdquo;Frohe Weihnachten&ldquo;)</label><select id=\"xmas\">";
  content += Gui::htmlOption("Aktiv", "1", Config::xmas ? "1" : "0");
  content += Gui::htmlOption("Inaktiv", "0", Config::xmas ? "1" : "0");
  content += "</select></div>";
  content += "<button class=\"preview\" data-i=\"xmas\">Vorschau</button>";
  content += "</fieldset>";

  for (int i = 0; i < MAX_OCCASIONS; i++) {
    const occasion_t& o = Config::occasions[i];
    String id = "o" + String(i);

    content += "<fieldset><legend>Anlass " + String(i + 1) + "</legend>";
    // Day and month options are added by the script to keep the page small.
    content += "<div><label>Datum (jedes Jahr)</label><div class=\"time\">";
    content += "<select id=\"" + id + "_d\" data-days=\"" + String((int) o.day) + "\"></select><span></span>";
    content += "<select id=\"" + id + "_m\" data-months=\"" + String(o.month == 0 ? 1 : (int) o.month) + "\"></select>";
    content += "</div></div>";
    content += "<div><label>Bild</label><select id=\"" + id + "_s\">";
    for (int s = 0; s < SYMBOL_COUNT; s++) {
      content += Gui::htmlOption(Symbols::name(s), String(s), String(o.day == 0 ? (int) SYMBOL_CAKE : (int) o.symbol));
    }
    content += "</select></div>";
    content += "<div><label>Text (optional), z. B. &bdquo;Alles Gute, Anna!&ldquo;</label>";
    content += "<input id=\"" + id + "_t\" type=\"text\" maxlength=\"80\" value=\"" + Gui::escape(o.text) + "\"></div>";
    content += "<button class=\"preview\" data-i=\"" + String(i) + "\">Vorschau</button>";
    content += "</fieldset>";
    Gui::emit(content);
  }
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"text\">";
  content += "<p class=\"info\">Der Text läuft über die Uhr und danach erscheint wieder die Uhrzeit. Er nutzt den Farbmodus aus dem Farb-Reiter.</p>";
  content += "<div><label>Text</label><input id=\"text_value\" type=\"text\" maxlength=\"80\" placeholder=\"z. B. Hallo Welt!\"></div>";
  content += "<div><label>Wiederholen</label><select id=\"text_repeat\">";
  content += "<option value=\"1\">1 &times;</option><option value=\"2\">2 &times;</option><option value=\"3\">3 &times;</option>";
  content += "<option value=\"5\">5 &times;</option><option value=\"10\">10 &times;</option><option value=\"0\">Dauerhaft (bis Stopp)</option>";
  content += "</select></div>";
  content += "<div><label>Geschwindigkeit (gilt auch für die Texte bei Anlässen)</label><select id=\"text_speed\">";
  content += Gui::htmlOption("Langsam", "1", String(Config::text_speed));
  content += Gui::htmlOption("Mittel", "2", String(Config::text_speed));
  content += Gui::htmlOption("Schnell", "3", String(Config::text_speed));
  content += "</select></div>";
  content += "<button class=\"action\" id=\"text_show\">Anzeigen</button>";
  content += "<button class=\"action secondary\" id=\"text_stop\">Stopp</button>";
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"wifi\">";
  content += "<button id=\"reset_wifi\" type=\"submit\">WiFi Einstellungen zurücksetzen</button>";
  content += "<div id=\"reset_wifi_message\">Die WiFi Einstellungen wurden zurückgesetzt. Es besteht keine Verbindung mehr zu der WordClock. Um die WordClock weiterhin zu verwenden muss das WiFi erneut eingerichtet werden.</div>";
  content += "</section>";
  Gui::emit(content);

  content += "<section id=\"firmware\">";
  content += "<p class=\"info\">Installierte Version: " + String(FIRMWARE_VERSION) + " (erstellt " + String(__DATE__) + ")</p>";
  content += "<p class=\"info\">Platz für neue Firmware: " + String(ESP.getFreeSketchSpace() / 1024) + " KB</p>";
  content += "<a class=\"button\" href=\"/firmware\">Neue Firmware installieren</a>";
  content += "</section>";
  Gui::emit(content);
  content += "</main>";

  return content;
}

String Gui::createFooter() {
  String content = "";

  content += "<footer>";
  content += "<button id=\"save\" type=\"submit\" class=\"button\">Speichern</button>";
  content += "</footer>";

  return content;
}


String Gui::render() {
  String content = "";

  //HttpServer::change();

  content += "<!DOCTYPE html><html><head>";
  content += "<meta charset=\"utf-8\">";
  content += "<link rel=\"manifest\" href=\"/manifest.json\">";
  content += "<link rel=\"icon\" type=\"image/svg+xml\" href=\"logo.svg\" sizes=\"any\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<style>";
  content += Gui::createStyleSheet();
  content += "</style>";
  content += "<script>";
  content += Gui::createScript();
  content += "</script>";
  content += "</head>";
  Gui::emit(content);
  content += "<body>";

  content += "<div id=\"c\">";
  content += Gui::createNav();
  Gui::emit(content);
  content += Gui::createContent();
  content += Gui::createFooter();
  content += "</div>";
  content += "</body></html>";

  return content;
}

// Sends a part of the page right away when streaming (keeps the RAM usage low).
void Gui::emit(String& content) {
  if (Gui::streaming && content.length() > 0) {
    HttpServer::web.sendContent(content);
    content = "";
  }
}

String Gui::index() {
  Gui::streaming = false;
  return Gui::render();
}

// The page is ~20 KB. Building it as one String could fail on the ESP8266,
// so it is sent in parts (chunked transfer encoding).
void Gui::sendIndex() {
  HttpServer::web.setContentLength(CONTENT_LENGTH_UNKNOWN);
  HttpServer::web.send(200, "text/html", "");

  Gui::streaming = true;
  String rest = Gui::render();
  Gui::streaming = false;

  if (rest.length() > 0) {
    HttpServer::web.sendContent(rest);
  }

  HttpServer::web.sendContent("");  // end of the chunked response
}

bool Gui::streaming = false;

String Gui::firmware() {
  String content = "";

  content += "<!DOCTYPE html><html><head>";
  content += "<meta charset=\"utf-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<title>WordClock Firmware</title>";
  content += "<style>";
  content += "body { font-family: Arial, \"Helvetica Neue\", Helvetica, sans-serif; background-color: #eee; color: #333; max-width: 600px; margin: 0 auto; padding: 16px; }";
  content += "h1 { color: #3d72a8; font-size: 1.4em; }";
  content += "input[type=file] { display: block; width: 100%; margin: 20px 0; }";
  content += "button, a { display: block; box-sizing: border-box; width: 100%; background-color: #3d72a8; color: #eee; border: 0; padding: 10px; text-align: center; text-decoration: none; font-size: 1em; cursor: pointer; margin-bottom: 10px; }";
  content += "button:disabled { background-color: #999; cursor: wait; }";
  content += "#busy { display: none; font-weight: bold; }";
  content += "</style>";
  content += "</head><body>";
  content += "<h1>Firmware-Update</h1>";
  content += "<p>Installierte Version: " + String(FIRMWARE_VERSION) + " (erstellt " + String(__DATE__) + ")</p>";
  content += "<p>Wähle die Datei <b>wordclock.ino.bin</b> aus (Arduino IDE: <i>Sketch &gt; Kompilierte Binärdatei exportieren</i>). ";
  content += "Die Einstellungen der Uhr bleiben erhalten.</p>";
  content += "<form method=\"POST\" action=\"/update\" enctype=\"multipart/form-data\" onsubmit=\"var b=document.getElementById('upload');b.disabled=true;b.textContent='Wird hochgeladen …';document.getElementById('busy').style.display='block';\">";
  content += "<input type=\"file\" name=\"firmware\" accept=\".bin,.bin.gz\" required>";
  content += "<button id=\"upload\" type=\"submit\">Firmware hochladen und installieren</button>";
  content += "</form>";
  content += "<p id=\"busy\">Bitte warten und die Uhr nicht vom Strom trennen. Nach dem Hochladen startet sie neu; das dauert etwa eine Minute.</p>";
  content += "<a href=\"/\">Zurück</a>";
  content += "</body></html>";

  return content;
}
