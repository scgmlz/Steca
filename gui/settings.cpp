#include "settings.h"
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

Settings::Settings(rcstr group) {
  setFallbacksEnabled(false);
  beginGroup(group);
}

Settings::~Settings() {
  endGroup();
}

QVariant Settings::readVariant(rcstr key, const QVariant &def) {
  auto val = value(key,def);
  return val;
}

void Settings::saveVariant(rcstr key, const QVariant &val) {
  setValue(key,val);
}

void Settings::read(rcstr key, QCheckBox* box, bool def) {
  if (box) box->setChecked(readVariant(key,def).toBool());
}

void Settings::save(rcstr key, QCheckBox* box) {
  if (box) saveVariant(key,box->isChecked());
}

void Settings::read(rcstr key, QSpinBox* box, int def) {
  if (box) box->setValue(readVariant(key,def).toInt());
}

void Settings::save(rcstr key, QSpinBox* box) {
  if (box) saveVariant(key,box->value());
}

void Settings::read(rcstr key, QDoubleSpinBox* box, qreal def) {
  if (box) box->setValue(readVariant(key,def).toDouble());
}

void Settings::save(rcstr key, QDoubleSpinBox* box) {
  if (box) saveVariant(key,box->value());
}

Keys::Keys() {
  keyAddFiles          = Qt::CTRL|Qt::Key_O;
  keyDeleteFile        = QKey::Delete;
  keySetCorrectionFile = Qt::SHIFT|Qt::CTRL|Qt::Key_O;

  keyViewStatusbar     = Qt::Key_F12;
#ifndef Q_OS_OSX
  keyFullscreen        = Qt::Key_F11;
#endif
}

// eof
