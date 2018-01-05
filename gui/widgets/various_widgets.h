// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/widgets/various_widgets.h
//! @brief     Defines classes TreeView, TreeListView, LineView, BoxWidget, DockWidget
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef VARIOUS_WIDGETS_H
#define VARIOUS_WIDGETS_H

#include "typ/str.h"
#include <QBoxLayout>
#include <QDockWidget>
#include <QLineEdit>

//! Read-only version of QLineEdit

class LineView : public QLineEdit {
public:
    LineView();

    void setText(rcstr);
};

//! a widget with a box layout

class BoxWidget : public QWidget {
public:
    BoxWidget(Qt::Orientation);
    QBoxLayout& box() const { return *box_; }

protected:
    QBoxLayout* box_;
};

//! a dock widget that acts as BoxWidget

class DockWidget : public QDockWidget {
private:
public:
    DockWidget(rcstr name, rcstr objectName, Qt::Orientation);

protected:
    QBoxLayout* box_;
};

#endif // VARIOUS_WIDGETS_H
