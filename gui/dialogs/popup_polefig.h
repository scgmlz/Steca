//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_polefig.h
//! @brief     Defines class PopupPolefig
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef POPUP_POLEFIG_H
#define POPUP_POLEFIG_H

#include "qcr/engine/mixin.h"
#include "popup_base_dialog.h"

//! Popup display of the present pole figure.

class PopupPolefig : public PopupBaseDialog {
public:
    PopupPolefig();
};

#endif // POPUP_POLEFIG_H
