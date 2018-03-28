// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/image_transform.cpp
//! @brief     Implements class ImageTransform
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/data/image_transform.h"

ImageTransform::ImageTransform(int val_)
    : val(eTransform(val_ & 7))
{}

ImageTransform ImageTransform::mirror(bool on) const
{
    return on ? ImageTransform(val | MIRROR) : ImageTransform(val & ~MIRROR);
}

ImageTransform ImageTransform::rotateTo(const ImageTransform& rot) const
{
    return ImageTransform((val & MIRROR) | (rot.val & 3));
}

ImageTransform ImageTransform::nextRotate() const
{
    return rotateTo(val + 1);
}
