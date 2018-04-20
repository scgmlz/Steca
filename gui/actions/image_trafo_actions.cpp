//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/image_trafo_actions.cpp
//! @brief     Implements class ImageTrafoActions
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "image_trafo_actions.h"
#include "core/session.h"

ImageTrafoActions::ImageTrafoActions()
{
    connect(&mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });
    connect(&rotateImage, &QAction::triggered, [this]() { setImageRotate(
                gSession->imageTransform().nextRotate()); });
}

void ImageTrafoActions::setImageRotate(const ImageTransform& rot)
{
    const char* rotateIconFile;
    const char* mirrorIconFile;

    switch (rot.val & 3) {
    case 0:
        rotateIconFile = ":/icon/rotate0";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 1:
        rotateIconFile = ":/icon/rotate1";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    case 2:
        rotateIconFile = ":/icon/rotate2";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 3:
        rotateIconFile = ":/icon/rotate3";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    default:
        qFatal("impossible rotation");
    }

    rotateImage.setIcon(QIcon(rotateIconFile));
    mirrorImage.setIcon(QIcon(mirrorIconFile));
    gSession->setImageTransformRotate(rot);
    // TODO gSession->imageCut().prevent_invalid_cuts()
    EMITS("ImageTrafoActions::setImageRotate", gSession->sigDetector());
}

void ImageTrafoActions::setImageMirror(bool on)
{
    mirrorImage.setChecked(on);
    gSession->setImageTransformMirror(on);
    EMITS("ImageTrafoActions::setImageMirror", gSession->sigDetector());
}
