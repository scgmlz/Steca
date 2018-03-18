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

#include "gui/base/controls.h"

//! Part of the main window that contains geometry, background, and image controls.

class SubframeSetup : public CTabWidget {
public:
    SubframeSetup();
    bool editingBaseline() const;
    bool editingPeaks() const;
private:
    void updateTabsAvailability();
};

#endif // SUBFRAME_SETUP_H
