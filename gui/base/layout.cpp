// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/layout.cpp
//! @brief     Implements enhanced layouts and other widgets
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/base/layout.h"

QHBoxLayout* newQ::HBoxLayout() {
    auto ret = new QHBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QVBoxLayout* newQ::VBoxLayout() {
    auto ret = new QVBoxLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

QGridLayout* newQ::GridLayout() {
    auto ret = new QGridLayout;
    ret->setSpacing(2);
    ret->setMargin(2);
    return ret;
}

DockWidget::DockWidget(const QString& name, const QString& objectName) {
    setFeatures(DockWidgetMovable);
    setWindowTitle(name);
    setObjectName(objectName);
    setWidget(new QWidget);
    widget()->setLayout((box_ = newQ::VBoxLayout()));
}
