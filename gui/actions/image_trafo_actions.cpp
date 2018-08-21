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

#include "gui/actions/image_trafo_actions.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

ImageTrafoActions::ImageTrafoActions()
{
    QObject::connect(&mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });
    QObject::connect(&rotateImage, &QAction::triggered, [this]() { doImageRotate(); });
}

void ImageTrafoActions::doImageRotate()
{
    const ImageTransform& rot = gSession->params.imageTransform.nextRotate();
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
    gSession->params.imageTransform.rotateTo(rot);
}

void ImageTrafoActions::setImageMirror(bool on)
{
    mirrorImage.programaticallySetValue(on);
    gSession->params.imageTransform.mirror(on);
}
