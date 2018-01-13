// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/widgets/various_widgets.cpp
//! @brief     Implements classes BoxWidget, DockWidget
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/base/various_widgets.h"
#include "gui/base/new_q.h"

BoxWidget::BoxWidget(Qt::Orientation orientation) {
    setLayout((box_ = newQ::BoxLayout(orientation)));
}


DockWidget::DockWidget(rcstr name, rcstr objectName) {
    setFeatures(DockWidgetMovable);
    setWindowTitle(name);
    setObjectName(objectName);

    setWidget(new QWidget);
    widget()->setLayout((box_ = newQ::VBoxLayout()));
}
