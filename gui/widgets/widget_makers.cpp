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

QBoxLayout* wmaker::newBoxLayout(Qt::Orientation orientation) {
    switch (orientation) {
    case Qt::Horizontal: return wmaker::newHBoxLayout();
    case Qt::Vertical: return wmaker::newVBoxLayout();
    }
    NEVER return nullptr;
}

QBoxLayout* wmaker::newHBoxLayout() {
    auto ret = new QHBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QBoxLayout* wmaker::newVBoxLayout() {
    auto ret = new QVBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QGridLayout* wmaker::newGridLayout() {
    auto grid = new QGridLayout;
    grid->setSpacing(2);
    grid->setMargin(2);
    return grid;
}

QLabel* wmaker::newLabel(rcstr text) {
    return new QLabel(text);
}

QLineEdit* wmaker::newLineEdit(uint emWidth) {
    auto ret = new QLineEdit;
    setEmWidth(ret, emWidth);
    return ret;
}

QLineEdit* wmaker::newLineDisplay(uint emWidth) {
    auto ret = wmaker::newLineEdit(emWidth);
    ret->setReadOnly(true);
    return ret;
}

QSpinBox* wmaker::newSpinBox(uint emWidth, int min, int max) {
    auto ret = new QSpinBox;
    setEmWidth(ret, emWidth);
    ret->setMinimum(min);
    ret->setMaximum(max > min ? max : min);
    return ret;
}

QDoubleSpinBox* wmaker::newDoubleSpinBox(uint emWidth, qreal min, qreal max) {
    auto ret = new QDoubleSpinBox;
    setEmWidth(ret, emWidth);
    ret->setMinimum(min);
    ret->setMaximum(max > min ? max : min);
    return ret;
}

QCheckBox* wmaker::newCheckBox(rcstr text) {
    return new QCheckBox(text);
}

QCheckBox* wmaker::newCheckBox(QAction* action) {
    if (!action)
        return new QCheckBox("");
    auto ret = new QCheckBox(action->text().toLower());
    QObject::connect(ret, &QCheckBox::toggled, [action](bool on) { action->setChecked(on); });
    QObject::connect(action, &QAction::toggled, [ret](bool on) { ret->setChecked(on); });
    ret->setToolTip(action->toolTip());
    ret->setChecked(action->isChecked());
    return ret;
}

QToolButton* wmaker::newTextButton(QAction* action) {
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonTextOnly);
    return ret;
}

QToolButton* wmaker::newIconButton(QAction* action) {
    auto ret = new QToolButton;
    ret->setDefaultAction(action);
    ret->setToolButtonStyle(Qt::ToolButtonIconOnly);
    return ret;
}

QRadioButton* wmaker::newRadioButton(rcstr text) {
    return new QRadioButton(text);
}
