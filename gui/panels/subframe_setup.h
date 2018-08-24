//  ***********************************************************************************************
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
//  ***********************************************************************************************

#ifndef SUBFRAME_SETUP_H
#define SUBFRAME_SETUP_H

#include "qcr/widgets/controls.h"

//! Subframe of the main window, with tabs to control detector, fits, &c

class SubframeSetup : public QcrTabWidget {
public:
    SubframeSetup();
};

#endif // SUBFRAME_SETUP_H
