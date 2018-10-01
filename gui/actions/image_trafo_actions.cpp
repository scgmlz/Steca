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
    mirrorImage.setHook([this](bool on){ setImageMirror(on); });
    rotateImage.setTriggerHook([this](){ doImageRotate(); });
}

void ImageTrafoActions::doImageRotate()
{
    gSession->params.imageTransform.doRotate();
    const char* rotateIconFile;
    const char* mirrorIconFile;
    switch (gSession->params.imageTransform.rotation) {
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
}

void ImageTrafoActions::setImageMirror(bool on)
{
    gSession->params.imageTransform.mirror = on;
}
