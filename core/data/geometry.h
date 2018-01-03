// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/geometry.h
//! @brief     Defines classes Geometry, ImageCut
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "typ/array2d.h"
#include "typ/ij.h"
#include "typ/range.h"

//! detector geometry

class Geometry {
public:
    static preal const MIN_DETECTOR_DISTANCE;
    static preal const MIN_DETECTOR_PIXEL_SIZE;

    static preal const DEF_DETECTOR_DISTANCE;
    static preal const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();

    COMPARABLE(Geometry const&);

    preal detectorDistance; // the distance from the sample to the detector
    preal pixSize; // size of the detector pixel
    IJ midPixOffset;
};

//! image cut (margins)

class ImageCut {
public:
    uint left, top, right, bottom;

    ImageCut();
    ImageCut(uint left, uint top, uint right, uint bottom);
    COMPARABLE(ImageCut const&);
    void update(bool topLeftFirst, bool linked, ImageCut const& cut, size2d size);

    size2d marginSize() const;
};

#endif // GEOMETRY_H
