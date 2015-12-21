#include "metrics.h"
#include <QFont>
#include <QFontMetrics>

int textWidth(rcstr s) {
  QFont myFont;
  QFontMetrics fm(myFont);
  return fm.width(s);
}

// eof
