// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/gui_helpers.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui_helpers.h"
#include "def/idiomatic_for.h"
#include "def/numbers.h"
#include <QAction>
#include <QGroupBox>

int mWidth(QWidget const* w) {
    debug::ensure(w);
    return w->fontMetrics().width('m');
}

QBoxLayout* boxLayout(Qt::Orientation orientation) {
    switch (orientation) {
    case Qt::Horizontal: return hbox();
    case Qt::Vertical: return vbox();
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

QGridLayout* gridLayout() {
    auto grid = new QGridLayout;
    grid->setSpacing(2);
    grid->setMargin(2);
    return grid;
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
    auto cb = new QCheckBox(action ? action->text().toLower() : "");

    if (action) {
        QObject::connect(cb, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });

        QObject::connect(action, &QAction::toggled, [cb](bool on) { cb->setChecked(on); });

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


TreeView::TreeView() {
    setAlternatingRowColors(true);
}

int TreeView::sizeHintForColumn(int) const {
    return 3 * mWidth(this);
}

TreeListView::TreeListView() {
    setSelectionBehavior(SelectRows);
}

void TreeListView::setModel(QAbstractItemModel* model) {
    TreeView::setModel(model);
    hideColumn(0); // this should look like a list; 0th column is tree-like

    if (model) {
        connect(model, &QAbstractItemModel::modelReset, [this, model]() {
            for_i (model->columnCount())
                resizeColumnToContents(i);
        });
    }
}


LineView::LineView() {
    setReadOnly(true);
}

void LineView::setText(rcstr text) {
    QLineEdit::setText(text);
    setCursorPosition(0);
}


BoxWidget::BoxWidget(Qt::Orientation orientation) {
    setLayout((box_ = boxLayout(orientation)));
}


DockWidget::DockWidget(rcstr name, rcstr objectName, Qt::Orientation orientation) {
    setFeatures(DockWidgetMovable);
    setWindowTitle(name);
    setObjectName(objectName);

    setWidget(new QWidget);
    widget()->setLayout((box_ = boxLayout(orientation)));
}
