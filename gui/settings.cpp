#include "settings.h"

Settings::Settings(rcstr group_): super(), group(group_) {
  setFallbacksEnabled(false);
}

str Settings::readStr(rcstr key, rcstr def) {
  beginGroup(group);
  str val = value(key,def).toString();
  endGroup();
  return val;
}

void Settings::saveStr(rcstr key, rcstr val) {
  beginGroup(group);
  setValue(key,val);
  endGroup();
}

Keys::Keys() {
  keyAddFiles          = Qt::CTRL|Qt::Key_O;
  keyDeleteFile        = QKey::Delete;
  keySetCorrectionFile = Qt::SHIFT|Qt::CTRL|Qt::Key_O;

  keyViewFiles         = Qt::Key_F9;
  keyViewInfo          = Qt::Key_F10;
  keyViewStatusbar     = Qt::Key_F11;
#ifndef Q_OS_OSX
  keyFullscreen        = Qt::Key_F12;
#endif
}

// eof
