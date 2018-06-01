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
#include <array>

// TODO after removal of ImageKey: delete copy c'tors

//! Detector geometry.
class Geometry {
public:
    static double const DEF_DETECTOR_DISTANCE;
    static double const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();
    COMPARABLE(const Geometry&);

    void fromSettings();
    void fromJson(const JsonObj& obj);

    ParamWrapper<double> detectorDistance {"detectorDistance", DEF_DETECTOR_DISTANCE};
    ParamWrapper<double> pixSize          {"pixSize", DEF_DETECTOR_PIXEL_SIZE};
    std::array<ParamWrapper<int>,2>    pixOffset {{{"pixOffsetX", 0}, {"pixOffsetY", 0}}};
    void toSettings() const;
    QJsonObject toJson() const;
};


//! Image cut (margins)
class ImageCut {
public:
    ImageCut();
    COMPARABLE(const ImageCut&);

    ParamWrapper<int> left {"cutLeft", 0};
    ParamWrapper<int> right {"cutRight", 0};
    ParamWrapper<int> top {"cutTop", 0};
    ParamWrapper<int> bottom {"cutBottom", 0};
    ParamWrapper<bool> linked {"linkCuts", false};

    void clear();
    void fromJson(const JsonObj& obj);

    int horiz() const { return left.val() + right.val(); }
    int vertical() const { return top.val() + bottom.val(); }
    size2d marginSize() const;
    QJsonObject toJson() const;

private:
    void postHook(int);
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
