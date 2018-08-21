//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/image_trafo_actions.h
//! @brief     Defines class ImageTrafoActions
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef IMAGE_TRAFO_ACTIONS_H
#define IMAGE_TRAFO_ACTIONS_H

#include "core/pars/image_transform.h"
#include "qcr/widgets/controls.h"

//! Trigger and toggle to control image rotation and mirror.

class ImageTrafoActions {
public:
    ImageTrafoActions();
    QcrTrigger rotateImage {"rotateImage", "Rotate", ":/icon/rotate0", Qt::CTRL | Qt::Key_R};
    QcrToggle mirrorImage {"mirrorImage", "Mirror", false, ":/icon/mirrorHorz"};
private:
    void doImageRotate();
    void setImageMirror(bool on);
};

#endif // IMAGE_TRAFO_ACTIONS_H
