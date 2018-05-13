//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/geometry.h
//! @brief     Defines classes Geometry, ImageCut, ScatterDirection, ImageKey
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "qcr/engine/cell.h"
#include "core/typ/angles.h"
#include "core/typ/size2d.h"
#include "core/typ/range.h"

//! Detector geometry.
class Geometry {
public:
    static double const DEF_DETECTOR_DISTANCE;
    static double const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();
    COMPARABLE(const Geometry&);

    void fromSettings();
    void fromJson(const JsonObj& obj);

    ParamCell<double> detectorDistance {DEF_DETECTOR_DISTANCE};   // from the sample to the detector
    ParamCell<double> pixSize          {DEF_DETECTOR_PIXEL_SIZE}; // size of the detector pixel
    ParamCell<int>    pixOffset[2]     {0, 0};
    void toSettings() const;
    QJsonObject toJson() const;
};


//! Image cut (margins)
class ImageCut {
public:
    ImageCut() {}
    ImageCut(const ImageCut&) = default; // TODO delete after removal of ImageCut
    COMPARABLE(const ImageCut&);

    void clear();
    void setLeft(int);
    void setRight(int);
    void setTop(int);
    void setBottom(int);

    ParamCell<int> left {0}, right {0}, top {0}, bottom {0};
    ParamCell<bool> linked {false};
    void fromJson(const JsonObj& obj);

    int horiz() const { return left.val() + right.val(); }
    int vertical() const { return top.val() + bottom.val(); }
    size2d marginSize() const;
    QJsonObject toJson() const;

private:
    static void confine(ParamCell<int>& m1, ParamCell<int>& m2, int maxTogether);
    void setAll(int);
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
    ImageKey(deg midTth);

    COMPARABLE(const ImageKey&);

    const Geometry geometry;
    const size2d size;
    const ImageCut cut;
    const int midPixX, midPixY;
    const deg midTth;
};

#endif // GEOMETRY_H
