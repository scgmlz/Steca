//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/image.cpp
//! @brief     Implements class Image
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/raw/image.h"

Image::Image(const size2d& size)
{
    fill(0, size);
}

Image::Image(const size2d& size, float val)
{
    fill(val, size);
}

// TODO: reimplement this less indirectly
Image::Image(const Array2D<float>& that)
    : Image(that.size())
{
    addImage(that);
}

void Image::addImage(const Image& that)
{
    if (!(size() == that.size())) THROW("inconsistent image size");
    for (int i = 0; i < size().w; ++i)
        for (int j = 0; j < size().h; ++j) {
            float inten = that.inten2d(i, j);
            rgeInten_.extendBy(inten);
            addInten2d(i, j, inten);
        }
}
