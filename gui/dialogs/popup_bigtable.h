//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_bigtable.h
//! @brief     Defines class PopupBigtable
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef POPUP_BIGTABLE_H
#define POPUP_BIGTABLE_H

#include "qcr/engine/mixin.h"
#include "popup_base_dialog.h"

//! Popup display of the present XY diagram.

class PopupBigtable : public PopupBaseDialog {
public:
    PopupBigtable();
};

#endif // POPUP_BIGTABLE_H
