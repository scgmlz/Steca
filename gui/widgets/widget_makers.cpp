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
#include "widgets/various_widgets.h"
#include "def/numbers.h"
#include <QAction>

static void setEmWidth(QWidget* w, uint emWidth) {
    w->setMaximumWidth(to_i(emWidth) * w->fontMetrics().width('m'));
}

BoxWidget* wmaker::newTab(QTabWidget* panel, rcstr title) {
    auto ret = new BoxWidget(Qt::Vertical);
    panel->addTab(ret, title);
    return ret;
}

QBoxLayout* boxLayout(Qt::Orientation orientation) {
    switch (orientation) {
    case Qt::Horizontal: return hbox();
    case Qt::Vertical: return vbox();
    }
    NEVER return nullptr;
}

QBoxLayout* hbox() {
    auto ret = new QHBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QBoxLayout* vbox() {
    auto ret = new QVBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
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
    auto ret = new QLineEdit;
    setEmWidth(ret, emWidth);
    return ret;
}

QLineEdit* readCell(uint emWidth) {
    auto ret = editCell(emWidth);
    ret->setReadOnly(true);
    return ret;
}

QSpinBox* spinCell(uint emWidth, int min, int max) {
    auto ret = new QSpinBox;
    setEmWidth(cell, emWidth);
    cell->setMinimum(min);
    cell->setMaximum(max > min ? max : min);
    return cell;
}

QDoubleSpinBox* spinDoubleCell(uint emWidth, qreal min, qreal max) {
    auto ret = new QDoubleSpinBox;
    setEmWidth(ret, emWidth);
    ret->setMinimum(min);
    ret->setMaximum(max > min ? max : min);
    return ret;
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
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonTextOnly);
    return ret;
}

QToolButton* iconButton(QAction* action) {
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonIconOnly);
    return ret;
}

QRadioButton* radioButton(rcstr text) {
    return new QRadioButton(text);
}
