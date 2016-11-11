// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      gui_helpers.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "gui_helpers.h"
#include <QAction>
#include <QGroupBox>

//------------------------------------------------------------------------------

void GridLayout::addRowStretch(int stretch) {
  setRowStretch(rowCount(), stretch);
}

void GridLayout::addColumnStretch(int stretch) {
  setColumnStretch(columnCount(), stretch);
}

GridLayout* GridLayout::groupBox(QLayout &addTo, rcstr title) {
  auto group = new QGroupBox(title);
  addTo.addWidget(group);
  auto grid = gridLayout();
  group->setLayout(grid);
  return grid;
}

int mWidth(QWidget const* w) {
  EXPECT(w)
  return w->fontMetrics().width('m');
}

//------------------------------------------------------------------------------

QBoxLayout* boxLayout(Qt::Orientation orientation) {
  switch (orientation) {
  case Qt::Horizontal:
    return hbox();
  case Qt::Vertical:
    return vbox();
  }

  NEVER return nullptr;
}

QBoxLayout* hbox() {
  auto box = new QHBoxLayout;
  box->setSpacing(2);
  box->setMargin(2);
  return box;
}

QBoxLayout* vbox() {
  auto box = new QVBoxLayout;
  box->setSpacing(2);
  box->setMargin(2);
  return box;
}

GridLayout* gridLayout() {
  auto grid = new GridLayout;
  grid->setSpacing(2);
  grid->setMargin(2);
  return grid;
}

QLabel* icon(rcstr fileName) {
  auto l = new QLabel;
  auto h = l->sizeHint().height();
  l->setPixmap(QIcon(fileName).pixmap(QSize(h, h)));
  return l;
}

QLabel* label(rcstr text) {
  return new QLabel(text);
}

static void setEmWidth(QWidget* w, uint emWidth) {
  w->setMaximumWidth(to_i(emWidth) * mWidth(w));
}

QLineEdit* editCell(uint emWidth) {
  auto cell = new QLineEdit;
  setEmWidth(cell, emWidth);
  return cell;
}

QLineEdit* readCell(uint emWidth) {
  auto cell = editCell(emWidth);
  cell->setReadOnly(true);
  return cell;
}

QSpinBox* spinCell(uint emWidth, int min, int max) {
  auto cell = new QSpinBox;
  setEmWidth(cell, emWidth);
  cell->setMinimum(min);
  cell->setMaximum(max > min ? max : min);
  return cell;
}

QDoubleSpinBox* spinDoubleCell(uint emWidth, qreal min, qreal max) {
  auto cell = new QDoubleSpinBox;
  setEmWidth(cell, emWidth);
  cell->setMinimum(min);
  cell->setMaximum(max > min ? max : min);
  return cell;
}

QCheckBox* check(rcstr text) {
  return new QCheckBox(text);
}

QCheckBox* check(QAction* action) {
  auto cb = new QCheckBox(action ? action->text().toLower() : EMPTY_STR);

  if (action) {
    QObject::connect(cb, &QCheckBox::toggled, [action](bool on) {
      action->setChecked(on);
    });

    QObject::connect(action, &QAction::toggled, [cb](bool on) {
      cb->setChecked(on);
    });

    cb->setToolTip(action->toolTip());
    cb->setChecked(action->isChecked());
  }

  return cb;
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

QComboBox* comboBox(str_lst::rc items) {
  auto comboBox = new QComboBox();
  comboBox->addItems(items.sup());
  return comboBox;
}

//------------------------------------------------------------------------------

TreeView::TreeView() {
  setAlternatingRowColors(true);
}

int TreeView::sizeHintForColumn(int) const {
  return 3 * mWidth(this);
}

//------------------------------------------------------------------------------

TreeListView::TreeListView() {
  setSelectionBehavior(SelectRows);
}

void TreeListView::setModel(QAbstractItemModel* model) {
  super::setModel(model);
  hideColumn(0);  // this should look like a list; 0th column is tree-like

  if (model) {
    connect(model, &QAbstractItemModel::modelReset, [this, model]() {
      for_i (model->columnCount())
        resizeColumnToContents(i);
    });
  }
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

DockWidget::DockWidget(rcstr name, rcstr objectName,
                       Qt::Orientation orientation) {
  setFeatures(DockWidgetMovable);
  setWindowTitle(name);
  setObjectName(objectName);

  setWidget(new QWidget);
  widget()->setLayout((box_ = boxLayout(orientation)));
}

//------------------------------------------------------------------------------
// eof
