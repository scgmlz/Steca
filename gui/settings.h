#ifndef SETTINGS_H
#define SETTINGS_H

#include "defs.h"
#include <QSettings>

class Settings: public QSettings {
  SUPER(Settings, QSettings)
public:
  Settings(rcstr group = "");

  str group;

  str   readStr(rcstr key, rcstr def);
  void  saveStr(rcstr key, rcstr val);

  static QKeySequence const KEY_VIEW_MENU;
  static QKeySequence const KEY_VIEW_STATUS;
  static QKeySequence const KEY_VIEW_FULLSCREEN;
};

#endif
