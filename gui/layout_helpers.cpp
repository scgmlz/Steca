#include "layout_helpers.h"

#include <QFont>
#include <QFontMetrics>

QHBoxLayout *hbox() {
  return new QHBoxLayout;
}

QVBoxLayout *vbox() {
  return new QVBoxLayout;
}

QGridLayout *grid() {
  return new QGridLayout;
}

int textWidth(rcstr s) {
  QFont myFont;
  QFontMetrics fm(myFont);
  return fm.width(s);
}

// eof
