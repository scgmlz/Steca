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

WaitCursor::WaitCursor() {
  QApplication::setOverrideCursor(Qt::WaitCursor);
}

WaitCursor::~WaitCursor() {
  QApplication::restoreOverrideCursor();
}

BoolGuard::BoolGuard(bool& b_): b(b_) {
  b = true;
}

BoolGuard::~BoolGuard() {
  b = false;
}

// eof
