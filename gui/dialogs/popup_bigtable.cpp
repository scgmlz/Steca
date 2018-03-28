// ************************************************************************************************
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
// ************************************************************************************************

#include "popup_bigtable.h"
#include "gui/view/bigtable.h"
#include "gui/mainwin.h"
#include <QVBoxLayout>

PopupBigtable::PopupBigtable()
    : CModelessDialog(gGui, "table#")
{
    auto* dataView = new DataView;
    dataView->setMinimumSize(600,600);

    auto* layout = new QVBoxLayout;
    layout->addWidget(dataView);
    layout->setStretch(0,1000);
    setLayout(layout);

    dataView->refresh();
    show();
}
