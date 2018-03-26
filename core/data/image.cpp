// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/image.cpp
//! @brief     Implements class Image
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/data/image.h"

Image::Image(const size2d& size) {
    fill(0, size);
}

Image::Image(const inten_arr& that) : Image(that.size()) {
    addIntens(that);
}

void Image::addIntens(const Image& that) THROWS {
    if (!(size() == that.size())) THROW("inconsistent image size");
    for (int i = 0; i < size().w; ++i)
        for (int j = 0; j < size().h; ++j) {
            float inten = that.inten(i, j);
            rgeInten_.extendBy(inten);
            addInten(i, j, inten);
        }
}
