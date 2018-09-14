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
#include "core/misc/exception.h"
#include "qcr/base/debug.h"

Image::Image(const size2d& size, float val)
    : size_(size)
    , intens_(size.count(), val)
    , rangeInten_(val, val)
{}

Image::Image(const size2d& size, std::vector<float>&& intens)
    : size_(size)
    , intens_(std::move(intens))
{
    ASSERT(intens_.size() == size.count());
    rangeInten_.set(intens_[0], intens_[0]);
    for (const auto val: intens_)
        rangeInten_.extendBy(val);
}

void Image::clear()
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
    for (int i=0; i<qMin(oldSize, newSize); ++i) // set all remaining pixels to val
        intens_[i] = val;
    rangeInten_.set(val, val); // set Range to val
}

void Image::addImage(const Image& that)
{
    ASSERT(size() == that.size());
    rangeInten_.extendBy(that.rgeInten());
    for (int i=0; i<intens_.size(); ++i)
        intens_[i] += that.intens_[i];
}
