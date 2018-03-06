// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/geometry.h
//! @brief     Defines classes Geometry, ImageCut, ScatterDirection, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "core/typ/angles.h"
#include "core/typ/array2d.h"
#include "core/typ/ij.h"
#include "core/typ/range.h"

//! Detector geometry.
class Geometry {
public:
    static qreal const MIN_DETECTOR_DISTANCE;
    static qreal const MIN_DETECTOR_PIXEL_SIZE;

    static qreal const DEF_DETECTOR_DISTANCE;
    static qreal const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();

    COMPARABLE(const Geometry&);

    qreal detectorDistance; // the distance from the sample to the detector
    qreal pixSize; // size of the detector pixel
    IJ midPixOffset;
};


//! Image cut (margins)
class ImageCut {
public:
    ImageCut() = default;
    ImageCut(int left, int top, int right, int bottom);
    COMPARABLE(const ImageCut&);

    void clear();
    void setLeft(int);
    void setRight(int);
    void setTop(int);
    void setBottom(int);
    void setLinked(bool);

    int left() const { return _left; }
    int right() const { return _right; }
    int top() const { return _top; }
    int bottom() const { return _bottom; }
    int linked() const { return _linked; }

    size2d marginSize() const;
private:
    void confine(int& m1, int& m2, int maxTogether);
    void setAll(int);

    int _left {0}, _top {0}, _right {0}, _bottom {0};
    bool _linked { false };
};


//! A pair of angles (gamma, 2theta) that designate a scattering direction.
class ScatterDirection {
public:
    ScatterDirection();
    ScatterDirection(deg, deg);

    deg tth;
    deg gma;
};


//! Holds geometry parameters that define a mapping of image coordinates onto (gamma,2theta).

//! Needed for caching such coordinate maps.
class ImageKey {
public:
    ImageKey(const Geometry&, const size2d&, const ImageCut&, const IJ& midPix, deg midTth);

    COMPARABLE(const ImageKey&);
    bool operator<(const ImageKey& that) const { return compare(that) < 0; }

    void computeAngles(Array2D<ScatterDirection>&) const;

    Geometry geometry;
    size2d size;
    ImageCut cut;
    IJ midPix;
    deg midTth;
};

#endif // GEOMETRY_H
