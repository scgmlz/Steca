// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_setup.h
//! @brief     Defines class SubframeSetup
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_SETUP_H
#define SUBFRAME_SETUP_H

#include "gui/base/new_q.h"

//! Part of the main window that contains geometry, background, and image controls.

class SubframeSetup : public QTabWidget {
public:
    SubframeSetup();
};

#endif // SUBFRAME_SETUP_H
