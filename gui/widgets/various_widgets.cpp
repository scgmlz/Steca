// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/various_widgets.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "widgets/various_widgets.h"
#include "widgets/widget_makers.h"

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
