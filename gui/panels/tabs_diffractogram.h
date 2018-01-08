// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tabs_diffractogram.h
//! @brief     Defines class ViewDiffractogram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TABS_DIFFRACTOGRAM_H
#define TABS_DIFFRACTOGRAM_H

#include <QTabWidget>

//! Part of the main window that contains a diffractogram, with associated controls.

class ViewDiffractogram : public QTabWidget {
public:
    ViewDiffractogram();
};

#endif // TABS_DIFFRACTOGRAM_H
