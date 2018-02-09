// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/layout.h
//! @brief     Defines enhanced layouts and other widgets
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
#include <QGridLayout>

//! Contains functions that return new Qt layouts.
namespace newQ {

QHBoxLayout* HBoxLayout();
QVBoxLayout* VBoxLayout();
QGridLayout* GridLayout();

} // namespace newQ

//! A dock widget with VBoxLayout.
class DockWidget : public QDockWidget {
public:
    DockWidget(const QString& name, const QString& objectName);

protected:
    QVBoxLayout* box_;
};

#endif // LAYOUT_H
