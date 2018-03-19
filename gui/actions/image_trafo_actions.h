// ************************************************************************** //
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
// ************************************************************************** //

#ifndef IMAGETRAFOACTIONS_H
#define IMAGETRAFOACTIONS_H

#include "gui/base/controls.h"

//! Trigger and toggle to control image rotation and mirror.

class ImageTrafoActions : private QObject {
public:
    ImageTrafoActions();
    CTrigger rotateImage {"rotateImage", "Rotate", ":/icon/rotate0", Qt::CTRL | Qt::Key_R};
    CToggle mirrorImage {"mirrorImage", "Mirror", false, ":/icon/mirrorHorz"};
};

#endif // IMAGETRAFOACTIONS_H
