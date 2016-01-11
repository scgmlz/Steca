#include "defs.h"
#include <QMessageBox>
#include <QApplication>

void warn(QObject* parent, rcstr msg) {
  QMessageBox::warning(dynamic_cast<QWidget*>(parent), qAppName(), msg);
}

void warn(QObject* parent, rcstr msg, rcstr more) {
  str s(msg);
  if (!more.isEmpty()) s += ": " + more;
  QMessageBox::warning(dynamic_cast<QWidget*>(parent), qAppName(), s);
}

// eof
