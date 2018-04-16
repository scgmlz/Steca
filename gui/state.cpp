//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/state.cpp
//! @brief     Defines class GuiState
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "state.h"
#include "core/data/peak_info.h"
#include "core/def/idiomatic_for.h"
#include "qcr/widgets/controls.h"
#include <QDebug>

GuiState::GuiState()
{
    polefigShowGridPts = new CCheckBox {"gridPts", "grid points"};

    diagramX = new CComboBox {"xAxis"};
    diagramY = new CComboBox {"yAxis"};

    // initialize
    QStringList tags = PeakInfo::dataTags(false);
    for_i (Metadata::numAttributes(false) - Metadata::numAttributes(true))
        tags.removeLast(); // remove all tags that are not numbers
    diagramX->addItems(tags);
    diagramY->addItems(tags);

    bigtableShowCol.fill(true, PeakInfo::dataTags(false).count());
}
