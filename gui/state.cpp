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
#include "core/calc/peak_info.h" // dataTags
#include "qcr/widgets/controls.h"
#include <QDebug>

GuiState::GuiState()
{
    polefigShowGridPts = new QcrCheckBox {"gridPts", "grid points"};

    diagramX = new QcrComboBox {"diagramCoordX"};
    diagramY = new QcrComboBox {"diagramCoordY"};

    // initialize
    QStringList tags = PeakInfo::dataTags(false);
    for (int i=0; i< (Metadata::numAttributes(false) - Metadata::numAttributes(true)); ++i)
        tags.removeLast(); // remove all tags that are not numbers
    // qDebug() << "diagram tags: " << tags.join(" ");
    diagramX->addItems(tags);
    diagramY->addItems(tags);

    bigtableShowCol.resize(PeakInfo::dataTags(false).count());
    std::fill(bigtableShowCol.begin(), bigtableShowCol.end(), true);
}
