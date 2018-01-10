// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/lens.h
//! @brief     Defines LensBase, ImageLens, SequenceLens
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

class Suite;
class Image;

//! View the data through a lens

class LensBase {
public:
    LensBase(bool trans, bool cut, ImageTransform const&, ImageCut const&);
    virtual ~LensBase() {}
    virtual size2d size() const = 0;

protected:
    size2d transCutSize(size2d) const;

    void doTrans(uint& i, uint& j) const;
    void doCut(uint& i, uint& j) const;

    bool trans_, cut_;
    ImageTransform imageTransform_;
    ImageCut imageCut_;
    Image const* intensCorr_;
};

class ImageLens final : public LensBase {
public:
    ImageLens(Image const&, bool trans, bool cut);

    size2d size() const;

    inten_t imageInten(uint i, uint j) const;

    Range const& rgeInten(bool fixed) const;

private:
    Image const& image_;

    mutable Range rgeInten_;
};

typedef QSharedPointer<const ImageLens> shp_ImageLens;


class SequenceLens final : public LensBase {
public:
    SequenceLens(Suite const&, eNorm, bool trans,
                 bool cut, ImageTransform const&, ImageCut const&);

    size2d size() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;
    Range rgeInten() const;

    Curve makeCurve() const;
    Curve makeCurve(Range const&) const;

    Suite const& suite() const { return suite_; }

private:
    void setNorm(eNorm);
    inten_t normFactor_;

    Suite const& suite_;
};

#endif // LENS_H
