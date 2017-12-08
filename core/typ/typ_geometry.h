// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_geometry.h
//! @brief     Defines classes Geometry, ImageCut, Angles, AngleMap
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_GEOMETRY_H
#define TYP_GEOMETRY_H

#include "typ/typ_array2d.h"
#include "typ/typ_ij.h"
#include "typ/typ_types.h"
#include <QSharedPointer>

namespace typ {

// detector geometry

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

// image cut (margins)

class ImageCut {
    public:

    uint left, top, right, bottom;

    ImageCut();
    ImageCut(uint left, uint top, uint right, uint bottom);
    COMPARABLE(ImageCut const&);
    void update(bool topLeftFirst, bool linked, typ::size2d size);

    size2d marginSize() const;
};

class Angles {
public:
    deg tth;
    deg gma;

    Angles();
    Angles(deg, deg);
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

    Angles const& at(uint i) const { return arrAngles_.at(i); }

    Angles const& at(uint i, uint j) const { return arrAngles_.at(i, j); }

    tth_rge rgeTth() const { return rgeTth_; }
    gma_rge rgeGma() const { return rgeGma_; }
    gma_rge rgeGmaFull() const { return rgeGmaFull_; }

    // TODO remove  IJ gmaPixel(gma_t);

    void getGmaIndexes(gma_rge const&, uint_vec const*&, uint&, uint&) const;

private:
    void calculate();

    Key key_;

    Array2D<Angles> arrAngles_;

    tth_rge rgeTth_;
    gma_rge rgeGma_, rgeGmaFull_;

    // sorted
    vec<deg> gmas;
    uint_vec gmaIndexes;
};

typedef QSharedPointer<AngleMap> shp_AngleMap;

} // namespace typ

#endif // TYP_GEOMETRY_H
