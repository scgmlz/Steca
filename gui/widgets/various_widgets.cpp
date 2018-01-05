// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/various_widgets.cpp
//! @brief     Implements classes BoxWidget, DockWidget
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "widgets/various_widgets.h"
#include "widgets/widget_makers.h"

BoxWidget::BoxWidget(Qt::Orientation orientation) {
    setLayout((box_ = wmaker::newBoxLayout(orientation)));
}


DockWidget::DockWidget(rcstr name, rcstr objectName) {
    setFeatures(DockWidgetMovable);
    setWindowTitle(name);
    setObjectName(objectName);

    setWidget(new QWidget);
    widget()->setLayout((box_ = wmaker::newVBoxLayout()));
}
