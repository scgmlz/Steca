// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/image.h
//! @brief     Defines class Image
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IMAGE_H
#define IMAGE_H

#include "core/typ/array2d.h"
#include "core/typ/range.h"
#include "core/typ/types.h"

typedef Array2D<inten_t> inten_arr;

//! Holds a detector image, and provides read and write access
class Image final {
    public:
    Image(const size2d& = size2d(0, 0));
    Image(const inten_arr&);

    const size2d& size() const { return intens_.size(); }

    void clear() { intens_.clear(); }

    bool isEmpty() const { return intens_.isEmpty(); }

    void fill(inten_t val, const size2d& size) { intens_.fill(val, size); }

    inten_t inten(int i) const { return intens_.at(i); }

    inten_t inten(int i, int j) const { return intens_.at(i, j); }

    void setInten(int i, inten_t val) { intens_.setAt(i, val); }

    void setInten(int i, int j, inten_t val) { intens_.setAt(i, j, val); }

    void addInten(int i, int j, inten_t val) { intens_.refAt(i, j) += val; }

    // Sum all intensities with new ones.
    void addIntens(const Image&) THROWS;

    const Range& rgeInten() const { return rgeInten_; }

private:
    inten_arr intens_;
    Range rgeInten_;
};

typedef QSharedPointer<Image> shp_Image;

#endif // IMAGE_H
