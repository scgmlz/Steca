// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      gui_helpers.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "gui_helpers.h"
#include <QAction>

//------------------------------------------------------------------------------

QBoxLayout *boxLayout(Qt::Orientation orientation) {
  switch (orientation) {
  case Qt::Horizontal:
    return hbox();
  case Qt::Vertical:
    return vbox();
  default:
    NEVER_HERE return nullptr;
  }
}

QBoxLayout *hbox() {
  auto box = new QHBoxLayout;
  box->setSpacing(2);
  return box;
}

QBoxLayout *vbox() {
  auto box = new QVBoxLayout;
  box->setSpacing(2);
  return box;
}

QGridLayout *gridLayout() {
  auto grid = new QGridLayout;
  grid->setSpacing(2);
  return grid;
}

QLabel *icon(rcstr fileName) {
  auto label = new QLabel;
  auto h = label->sizeHint().height();
  label->setPixmap(QIcon(fileName).pixmap(QSize(h,h)));
  return label;
}

QLabel* label(rcstr text) {
  return new QLabel(text);
}

static void setEmWidth(QWidget* w, uint emWidth) {
  w->setMaximumWidth(emWidth*w->fontMetrics().width('m'));
}

QLineEdit* editCell(uint emWidth) {
  auto cell = new QLineEdit;
  setEmWidth(cell,emWidth);
  return cell;
}

QLineEdit* readCell(uint emWidth) {
  auto cell = editCell(emWidth);
  cell->setReadOnly(true);
  return cell;
}

QSpinBox* spinCell(uint emWidth,int min,int max) {
  auto cell = new QSpinBox;
  setEmWidth(cell,emWidth);
  cell->setMinimum(min);
  cell->setMaximum(max>INT_MIN ? max : INT_MAX);
  return cell;
}

QDoubleSpinBox *spinCell(uint emWidth,qreal min, qreal max) {
  auto cell = new QDoubleSpinBox;
  setEmWidth(cell,emWidth);
  cell->setMinimum(min);
  cell->setMaximum(max>INT_MIN ? max : INT_MAX);
  return cell;
}

QCheckBox* check(rcstr text, QAction* action) {
  auto ch = new QCheckBox(text);
  if (action) QObject::connect(ch, &QCheckBox::toggled,[action](bool on) {
    action->setChecked(on);
  });
  return ch;
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

QComboBox* comboBox(str_lst const& items) {
  auto comboBox = new QComboBox();
  comboBox->addItems(items);
  return comboBox;
}

//------------------------------------------------------------------------------

TreeView::TreeView() {
  setAlternatingRowColors(true);
}

int TreeView::sizeHintForColumn(int) const {
  return 3 * fontMetrics().width('m');
}

//------------------------------------------------------------------------------

TreeListView::TreeListView() {
  setSelectionBehavior(SelectRows);
}

void TreeListView::setModel(QAbstractItemModel* model) {
  super::setModel(model);
  hideColumn(0);  // this should look like a list; 0th column is tree-like
}

//------------------------------------------------------------------------------

LineView::LineView() {
  setReadOnly(true);
}

void LineView::setText(rcstr text) {
  super::setText(text);
  super::setCursorPosition(0);
}

//------------------------------------------------------------------------------

BoxWidget::BoxWidget(Qt::Orientation orientation) {
  setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------

DockWidget::DockWidget(rcstr name,rcstr objectName,Qt::Orientation orientation) {
  setFeatures(NoDockWidgetFeatures);
  setWidget(new QWidget);
  widget()->setLayout((box_ = boxLayout(orientation)));

  setWindowTitle(name);
  setObjectName(objectName);
}

//------------------------------------------------------------------------------
// eof
