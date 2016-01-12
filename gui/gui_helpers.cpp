#include "gui_helpers.h"

QBoxLayout *hbox() {
  return new QHBoxLayout;
}

QBoxLayout *vbox() {
  return new QVBoxLayout;
}

QBoxLayout *boxLayout(Qt::Orientation orientation) {
  switch (orientation) {
  default:
    NOT_HERE
  case Qt::Horizontal:
    return hbox();
  case Qt::Vertical:
    return vbox();
  }
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
  auto cell = new QLineEdit;
  cell->setMaximumWidth(cell->sizeHint().height() * 2);
  return cell;
}

QSpinBox* spinCell(int min,int max) {
  auto cell = new QSpinBox;
  cell->setMaximumWidth(cell->sizeHint().height() * 2);
  cell->setMinimum(min);
  cell->setMaximum(max);
  return cell;
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
