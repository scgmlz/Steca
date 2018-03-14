// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/layout.h
//! @brief     Defines class DockWidget
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QDockWidget>
#include <QVBoxLayout>

//! A dock widget with VBoxLayout.
class DockWidget : public QDockWidget {
public:
    DockWidget(const QString& name, const QString& objectName);
protected:
    QVBoxLayout box_;
};

#endif // LAYOUT_H
