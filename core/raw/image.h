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

#include "core/typ/range.h"
#include "core/typ/size2d.h"

//! Holds a detector image, and provides read and write access
class Image {
public:
    Image() {} // empty image
    Image(const size2d&, float val);
    Image(const size2d& size, std::vector<float>&& intens);
    Image(const Image&) = delete;
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    void clear();
    void fill(float val, const size2d& size);
    void setInten1d(int i, float val) { intens_[i] = val; }
    void setInten2d(int ix, int iy, float val) { setInten1d(pointToIndex(ix, iy), val); }
    void addImage(const Image&); //!< add pointwise

    bool isEmpty() const { return intens_.empty(); }
    const size2d& size() const { return size_; }
    float inten1d(int i) const { return intens_[i]; }
    float inten2d(int ix, int iy) const { return inten1d(pointToIndex(ix, iy)); }
    const Range& rgeInten() const { return rangeInten_; }

private:
    size2d size_;
    std::vector<float> intens_;
    Range rangeInten_; // TODO: update Intensity Range when single pixel gets changed

    int pointToIndex(int ix, int iy) const { return iy * size_.w + ix; }
};

#endif // IMAGE_H
