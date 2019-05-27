//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/popup_base_dialog.h
//! @brief     Defines class PopupDiagram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef POPUP_BASE_DIALOG_H
#define POPUP_BASE_DIALOG_H

#include "QCR/engine/mixin.h"
#include "core/session.h"

//! Popup display of the present XY diagram.

class PopupBaseDialog : public QcrModelessDialog {
protected:
    PopupBaseDialog(QWidget* parent, const QString& name, const QString& title)
        : QcrModelessDialog(parent, name)
    {
        int jP = gSession->peaksSettings.selectedIndex();
        bool isInterpolated = gSession->params.interpolParams.enabled.val();
        setWindowTitle(title + " for peak " + QString::number(jP + 1)
                       + (isInterpolated ? " (interpolated)" : ""));
    }
};

#endif // POPUP_BASE_DIALOG_H
