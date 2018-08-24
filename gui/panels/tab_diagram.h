//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_diagram.h
//! @brief     Defines class DiagramTab
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef TAB_DIAGRAM_H
#define TAB_DIAGRAM_H

#include "core/calc/peak_info.h"
#include "qcr/widgets/views.h"

//! Mainframe tab to plot a pair of fit results or metadata, with associated controls.

class DiagramTab : public QcrWidget {
public:
    DiagramTab();
};

#endif // TAB_DIAGRAM_H
