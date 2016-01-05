#include "gui_helpers.h"

QBoxLayout *hbox() {
  return new QHBoxLayout;
}

QBoxLayout *vbox() {
  return new QVBoxLayout;
}

QBoxLayout *boxLayout(Qt::Orientation orientation) {
  switch (orientation) {
  case Qt::Horizontal:
    return hbox();
  case Qt::Vertical:
    return vbox();
  }
  NOT_HERE
}

QGridLayout *grid(uint margin) {
  auto box = new QGridLayout;
  box->setMargin(margin);
  box->setSpacing(0);
  return box;
}

QLabel* label(rcstr text) {
  return new QLabel(text);
}

QLineEdit* editCell() {
  auto e = new QLineEdit;
  e->setMaximumWidth(e->sizeHint().height() * 2);
  return e;
}

QCheckBox* check(rcstr text) {
  return new QCheckBox(text);
}

QToolButton* textButton(QAction* action) {
  auto b = new QToolButton;
  b->setDefaultAction(action);
  b->setToolButtonStyle(Qt::ToolButtonTextOnly);
  return b;
}

QToolButton* iconButton(QAction* action) {
  auto b = new QToolButton;
  b->setDefaultAction(action);
  b->setToolButtonStyle(Qt::ToolButtonIconOnly);
  return b;
}

QRadioButton* radioButton(rcstr text) {
  return new QRadioButton(text);
}

ListView::ListView() {
  setAlternatingRowColors(true);
}

// eof
