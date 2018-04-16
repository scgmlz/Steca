//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/image.h
//! @brief     Defines class Image
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef IMAGE_H
#define IMAGE_H

#include "core/typ/array2d.h"
#include "core/typ/range.h"

//! Holds a detector image, and provides read and write access
class Image {
public:
    Image() {}
    Image(const size2d&);
    Image(const Array2D<float>&);
    Image(Image&) = delete;
    Image(Image&&) = default;

    const size2d& size() const { return intens_.size(); }

    void clear() { intens_.clear(); }

    bool isEmpty() const { return intens_.isEmpty(); }

    void fill(float val, const size2d& size) { intens_.fill(val, size); }

    float inten1d(int i) const { return intens_.at(i); }

    float inten2d(int i, int j) const { return intens_.at(i, j); }

    void setInten1d(int i, float val) { intens_.setAt(i, val); }

    void setInten2d(int i, int j, float val) { intens_.setAt(i, j, val); }

    void addInten2d(int i, int j, float val) { intens_.refAt(i, j) += val; }

    // Sum all intensities with new ones.
    void addImage(const Image&);

    const Range& rgeInten() const { return rgeInten_; }

private:
    Array2D<float> intens_;
    Range rgeInten_;
};

#endif // IMAGE_H
