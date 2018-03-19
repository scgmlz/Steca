// ************************************************************************** //
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
// ************************************************************************** //

#include "image_trafo_actions.h"
#include "core/session.h"
#include "gui/mainwin.h"

ImageTrafoActions::ImageTrafoActions()
{
    connect(&mirrorImage, &QAction::toggled, [](bool on) { gGui->setImageMirror(on); });
    connect(&rotateImage, &QAction::triggered, []() { gGui->setImageRotate(
       gSession->imageTransform().nextRotate()); });
}
