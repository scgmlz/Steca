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
    : size_(size),
      intens_(size.count(), val),
      rangeInten_(val, val)
{ }

Image::Image(const size2d& size, QVector<float>&& intens)
    : size_(size)
{
    ASSERT(intens.count() == size.count());

    intens_ = std::vector<float>(size.count());
    rangeInten_.set(intens[0], intens[0]);

    for_i(intens_.size()) { // doing it this way, so only on loop ovr intens is needed.
        intens_[i] = intens[i];
        rangeInten_.extendBy(intens[i]);
    }
}

void  Image::clear()
{
    size_ = size2d(0, 0);
    intens_.clear();
    rangeInten_.invalidate();
}

void Image::fill(float val, const size2d& size)
{
    int oldSize = intens_.size();
    int newSize = size.count();
    size_ = size;
    intens_.resize(newSize, val); // sets only new pixels to val
    auto maxIndex = std::min(oldSize, newSize);
    for_i(maxIndex) { // set all remaining pixels to val
        intens_[i] = val;
    }
    rangeInten_.set(val, val); // set Range to val
}


void Image::addImage(const Image& that)
{
    if (!(size() == that.size())) THROW("inconsistent image size");

    rangeInten_.extendBy(that.rgeInten());
    for_i(intens_.size()) {
        intens_[i] += that.intens_[i];
    }
}
