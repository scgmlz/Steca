//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_clusters.h
//! @brief     Defines class SubframeClusters
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SUBFRAME_CLUSTERS_H
#define SUBFRAME_CLUSTERS_H

#include "qcr/widgets/views.h"

//! Part of the main window that controls the measurements selection.

//! Shows the measurements in the selected files, and allow to select some of them
//! and to combine them by a constant factor

class SubframeClusters : public QcrDockWidget {
public:
    SubframeClusters();
};

#endif // SUBFRAME_CLUSTERS_H
