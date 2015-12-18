#include "settings.h"
#include <QKeySequence>

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

QKeySequence const Settings::KEY_VIEW_MENU(Qt::SHIFT|Qt::META|Qt::Key_F12);
QKeySequence const Settings::KEY_VIEW_STATUS(Qt::SHIFT|Qt::META|Qt::Key_F10);
QKeySequence const Settings::KEY_VIEW_FULLSCREEN(Qt::SHIFT|Qt::META|Qt::Key_F11);

// eof
