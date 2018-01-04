// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/widget_makers.cpp
//! @brief     Implements functions that return new widgets with Steca-standard settings
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "widgets/widget_makers.h"
#include "def/numbers.h"
#include <QAction>

static void setEmWidth(QWidget* w, uint emWidth) {
    w->setMaximumWidth(to_i(emWidth) * w->fontMetrics().width('m'));
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
    if (!action)
        return new QCheckBox("");
    auto ret = new QCheckBox(action->text().toLower());
    QObject::connect(ret, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    QObject::connect(action, &QAction::toggled, [ret](bool on) { ret->setChecked(on); });
    ret->setToolTip(action->toolTip());
    ret->setChecked(action->isChecked());
    return ret;
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
