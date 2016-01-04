#ifndef SETTINGS_H
#define SETTINGS_H

#include "defs.h"
#include <QSettings>
#include <QKeySequence>

class Settings: public QSettings {
  SUPER(Settings, QSettings)
public:
  Settings(rcstr group = "");

  str group;

  str   readStr(rcstr key, rcstr def);
  void  saveStr(rcstr key, rcstr val);
};

class Keys {
public:
  Keys();

  typedef QKeySequence QKey;

  QKey
    keyAddFiles, keyDeleteFile, keySetCorrectionFile,
    keyViewFiles, keyViewInfo, keyViewStatusbar
  #ifndef Q_OS_OSX
    , keyFullscreen
  #endif
    ;
};

#endif
