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

#include "core/typ/angles.h"
#include "core/typ/range.h"
#include "core/typ/size2d.h"
#include "qcr/engine/param_wrapper.h"
#include <array>

// TODO after removal of ImageKey: delete copy c'tors

//! Detector geometry.
class Geometry {
public:
    static double const DEF_DETECTOR_DISTANCE;
    static double const DEF_DETECTOR_PIXEL_SIZE;

    Geometry();

    void fromJson(const JsonObj& obj);
    void fromSettings();

    QJsonObject toJson() const;
    void toSettings() const;

    ParamWrapper<double> detectorDistance {DEF_DETECTOR_DISTANCE};
    ParamWrapper<double> pixSize          {DEF_DETECTOR_PIXEL_SIZE};
    std::array<ParamWrapper<int>,2>    pixOffset {0, 0};
};


//! Image cut (margins)
class ImageCut {
public:
    ImageCut();

    void fromJson(const JsonObj& obj);
    void clear();

    int horiz() const { return left.val() + right.val(); }
    int vertical() const { return top.val() + bottom.val(); }
    size2d marginSize() const;
    QJsonObject toJson() const;

    ParamWrapper<int> left {0};
    ParamWrapper<int> right {0};
    ParamWrapper<int> top {0};
    ParamWrapper<int> bottom {0};
};


//! A pair of angles (gamma, 2theta) that designate a scattering direction.
class ScatterDirection {
public:
    ScatterDirection();
    ScatterDirection(deg, deg);

    deg tth;
    deg gma;
};

#endif // GEOMETRY_H
