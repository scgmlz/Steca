// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/lens.h
//! @brief     Defines ImageLens, SequenceLens
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef LENS_H
#define LENS_H

#include "core/typ/curve.h"
#include "core/data/geometry.h"
#include "core/typ/types.h"
#include "core/data/image_transform.h"

class Sequence;
class Image;

//! A lens for a single Image.

class ImageLens {
public:
    ImageLens(const Image&, bool trans, bool cut);

    size2d imgSize() const;
    inten_t imageInten(int i, int j) const;
    const Range& rgeInten(bool fixed) const;

private:
    void doTrans(int& i, int& j) const;
    void doCut(int& i, int& j) const;
    size2d transCutSize(size2d) const;
    bool trans_, cut_;
    const Image& image_;
    mutable Range rgeInten_;
};

typedef QSharedPointer<const ImageLens> shp_ImageLens;


//! A lens for a sequence of Image's.

class SequenceLens {
public:
    SequenceLens(Sequence const&, eNorm);

    Curve makeCurve() const;
    Curve makeCurve(const Range&) const;

private:
    void setNorm(eNorm);
    inten_t normFactor_;

    Sequence const& seq_;
};

typedef QSharedPointer<SequenceLens> shp_SequenceLens;

#endif // LENS_H
