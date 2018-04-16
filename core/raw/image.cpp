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
#include "core/def/idiomatic_for.h"

Image::Image(const size2d& size, float val)
{
    fill(val, size);
}

Image::Image(const size2d& size, QVector<float>&& intens)
{
    ASSERT(intens.count() == size.count());
    intens_.fill(0., size);
    for_i (intens.count())
        setInten1d(i, intens.at(i));
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
