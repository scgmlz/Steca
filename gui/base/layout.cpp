// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/layout.cpp
//! @brief     Implements class DockWidget
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/base/layout.h"

DockWidget::DockWidget(const QString& name, const QString& objectName)
{
    setFeatures(DockWidgetMovable);
    setWindowTitle(name);
    setObjectName(objectName);
    setWidget(new QWidget);
    widget()->setLayout(&box_);
}
