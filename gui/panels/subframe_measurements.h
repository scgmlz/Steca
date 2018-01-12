// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_measurements.h
//! @brief     Defines class SubframeMeasurements
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_MEASUREMENTS_H
#define SUBFRAME_MEASUREMENTS_H

#include "gui/widgets/new_q.h"
#include "gui/widgets/various_widgets.h"

//! Part of the main window that controls the measurements selection.

//! Shows the measurements in the selected files, and allow to select some of them
//! and to combine them by a constant factor

class SubframeMeasurements : public DockWidget {
public:
    SubframeMeasurements();
};

#endif // SUBFRAME_MEASUREMENTS_H
