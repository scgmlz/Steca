// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/angle_map.h
//! @brief     Defines classes Angles, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef ANGLE_MAP_H
#define ANGLE_MAP_H

#include "typ/angles.h"
#include "typ/array2d.h"
#include "typ/geometry.h"
#include "typ/ij.h"
#include "typ/range.h"

class AnglePair {
public:
    deg tth;
    deg gma;

    AnglePair();
    AnglePair(deg, deg);
};

class AngleMap {
public:
    struct Key {
        Key(Geometry const&, size2d const&, ImageCut const&, IJ const& midPix, deg midTth);

        COMPARABLE(AngleMap::Key const&);

        bool operator<(AngleMap::Key const& that) const {
            return compare(that) < 0; }

        Geometry geometry;
        size2d size;
        ImageCut cut;
        IJ midPix;
        deg midTth;
    };

    AngleMap(Key const&);

    AnglePair const& at(uint i) const { return arrAngles_.at(i); }

    AnglePair const& at(uint i, uint j) const { return arrAngles_.at(i, j); }

    Range rgeTth() const { return rgeTth_; }
    Range rgeGma() const { return rgeGma_; }
    Range rgeGmaFull() const { return rgeGmaFull_; }

    // TODO remove  IJ gmaPixel(gma_t);

    void getGmaIndexes(Range const&, uint_vec const*&, uint&, uint&) const;

private:
    void calculate();

    Key key_;

    Array2D<AnglePair> arrAngles_;

    Range rgeTth_;
    Range rgeGma_, rgeGmaFull_;

    // sorted
    vec<deg> gmas;
    uint_vec gmaIndexes;
};

typedef QSharedPointer<AngleMap> shp_AngleMap;

#endif // ANGLE_MAP_H
