#ifndef WORDCLOCK_CONTROLLER_H
#define WORDCLOCK_CONTROLLER_H

#include <Arduino.h>

class Controller {
  public:
    static void index();
    static void saveColor();    
    static void saveTime();    
    static void saveDnd();    
    static void deleteWiFi();
    static void firmware();
    static void getLogoSvg();
    static void getManifest();
    static void notFound();

  private:
    static bool isValidSetting(const String& value);
};

#endif
