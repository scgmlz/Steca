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
    Image() = delete;
    Image(const size2d&, float val);
    Image(const size2d& size, QVector<float>&& intens);
    Image(Image&) = delete;
    Image(Image&&) = default;

    const size2d& size() const { return size_; }

    void clear();

    bool isEmpty() const { return intens_.empty(); }

    void fill(float val, const size2d& size);

    float inten1d(int i) const { return intens_.at(i); }

    float inten2d(int ix, int iy) const { return inten1d(pointToIndex(ix, iy)); }

    void setInten1d(int i, float val) { intens_[i] = val; }

    void setInten2d(int ix, int iy, float val) { setInten1d(pointToIndex(ix, iy), val); }

    // Sum all intensities with new ones.
    void addImage(const Image&);

    const Range& rgeInten() const { return rangeInten_; }

private:
    size2d size_;
    std::vector<float> intens_;
    Range rangeInten_; // TODO: update Intensity Range when single pixel gets changed

    int pointToIndex(int ix, int iy) const { return iy * size_.w + ix; }
};

#endif // IMAGE_H
