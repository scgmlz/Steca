// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/layout.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef LAYOUT_H
#define LAYOUT_H

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

//! Contains functions that return new Qt layouts.

namespace newQ {

QBoxLayout* HBoxLayout();
QBoxLayout* VBoxLayout();
QGridLayout* GridLayout();

} // namespace newQ

#endif // LAYOUT_H
