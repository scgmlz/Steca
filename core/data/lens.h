//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/lens.h
//! @brief     Defines ImageLens
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef LENS_H
#define LENS_H

#include "core/typ/curve.h"
#include "core/typ/size2d.h"

class Image;

//! A lens for a single Image.

class ImageLens {
public:
    ImageLens(const Image&, bool trans, bool cut);

    size2d imgSize() const;
    float imageInten(int i, int j) const;
    const Range& rgeInten(bool fixed) const;

private:
    void doTrans(int& i, int& j) const;
    void doCut(int& i, int& j) const;
    size2d transCutSize(size2d) const;
    bool trans_, cut_;
    const Image& image_;
    mutable Range rgeInten_;
};

#endif // LENS_H
