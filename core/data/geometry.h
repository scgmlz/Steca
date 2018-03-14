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
    static qreal const DEF_DETECTOR_DISTANCE;
    static qreal const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();
    COMPARABLE(const Geometry&);

    void setDetectorDistance(qreal);
    void setPixSize(qreal);
    void setOffset(const IJ& midPixOffset);

    qreal detectorDistance() const { return detectorDistance_; }
    qreal pixSize() const { return pixSize_; }
    IJ& midPixOffset() { return midPixOffset_; }
    const IJ& midPixOffset() const { return midPixOffset_; }

private:
    qreal detectorDistance_; // the distance from the sample to the detector
    qreal pixSize_; // size of the detector pixel
    IJ midPixOffset_;
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

    int left() const { return left_; }
    int right() const { return right_; }
    int top() const { return top_; }
    int bottom() const { return bottom_; }
    int linked() const { return linked_; }

    size2d marginSize() const;
private:
    void confine(int& m1, int& m2, int maxTogether);
    void setAll(int);

    int left_ {0}, top_ {0}, right_ {0}, bottom_ {0};
    bool linked_ { false };
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
