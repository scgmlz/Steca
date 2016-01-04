#include "gui_helpers.h"
#include <stdarg.h>

#include <QFont>
#include <QFontMetrics>
#include <QAction>

QHBoxLayout *hbox(uint margin) {
  auto box = new QHBoxLayout;
  box->setMargin(margin);
  box->setSpacing(0);
  return box;
}

QVBoxLayout *vbox(uint margin) {
  auto box = new QVBoxLayout;
  box->setMargin(margin);
  box->setSpacing(0);
  return box;
}

QBoxLayout *boxLayout(Qt::Orientation orientation, uint margin) {
  switch (orientation) {
  case Qt::Horizontal:
    return hbox(margin);
  case Qt::Vertical:
    return vbox(margin);
  }
}

QGridLayout *grid(uint margin) {
  auto box = new QGridLayout;
  box->setMargin(margin);
  box->setSpacing(0);
  return box;
}

int textWidth(rcstr s) {
  QFont myFont;
  QFontMetrics fm(myFont);
  return fm.width(s);
}

int textHeight() {
  QFont myFont;
  QFontMetrics fm(myFont);
  return fm.height();
}

QLabel* label(rcstr text) {
  return new QLabel(text);
}

EditCell::EditCell(): super() {
  int h = super::sizeHint().height();
  setMaximumWidth(2*h);
}

EditCell* editCell() {
  return new EditCell();
}

QCheckBox* check(rcstr text) {
  return new QCheckBox(text);
}

PushButton::PushButton(rcstr text): PushButton(QIcon(),text) {
}

PushButton::PushButton(QIcon const& icon): PushButton(icon,str::null) {
}

PushButton::PushButton(QIcon const& icon, rcstr text): super(icon,text), action(nullptr) {
}

void PushButton::setAction(QAction* action_) {
  action = action_;
  setToolTip(action ? action->toolTip() : str::null);

  disconnect();
  if (action) connect(this, &PushButton::clicked, action, &QAction::trigger);
}

void PushButton::enable(bool on) {
  setEnabled(on);
  if (action) action->setEnabled(on);
}

PushButton* pushButton(rcstr text) {
  return new PushButton(text);
}

PushButton* pushButton(QIcon const& icon) {
  return new PushButton(icon);
}

PushButton* pushButton(QIcon const& icon, rcstr text) {
  return new PushButton(icon,text);
}

IconButton::IconButton(QIcon const& icon) {
  setToolButtonStyle(Qt::ToolButtonIconOnly);
  setCheckable(true); setChecked(true);
//  setIcon(icon);
}

IconButton::IconButton(rcstr iconFile): IconButton(QIcon(iconFile)){
}

IconButton* iconButton(QIcon const& icon) {
  return new IconButton(icon);
}

IconButton* iconButton(rcstr iconFile) {
  return new IconButton(iconFile);
}

QRadioButton* radioButton(rcstr text) {
  return new QRadioButton(text);
}

// eof
