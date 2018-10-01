//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_bigtable.cpp
//! @brief     Implements class PopupBigtable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/popup_bigtable.h"
#include "gui/mainwin.h"
#include "gui/view/bigtable.h"
#include <QVBoxLayout>

PopupBigtable::PopupBigtable()
    : PopupBaseDialog(gGui, "table#", "Table")
{
    auto* bigtableView = new BigtableView;
    bigtableView->setMinimumSize(600,600);

    auto* layout = new QVBoxLayout;
    layout->addWidget(bigtableView);
    layout->setStretch(0,1000);
    // TODO (issue#76) add info about used parameters
    setLayout(layout);

    bigtableView->refresh();
    show();
}
