// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/subframe_image.h
//! @brief     Defines class SubframeImage
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef SUBFRAME_IMAGE_H
#define SUBFRAME_IMAGE_H

#include "gui/base/controls.h"

//! Part of the main window that shows a detector image, with associated controls.

//! There are two tabs: data image and correction data image.

class SubframeImage final : public CTabWidget {
public:
    SubframeImage();
    ~SubframeImage();
private:
    void render();
    class DataImageTab* dataImageTab_;
    class CorrImageTab* corrImageTab_;
};

#endif // SUBFRAME_IMAGE_H
