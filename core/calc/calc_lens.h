// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_lens.h
//! @brief     Defines LensBase, ImageLens, DatasetLens
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CALC_LENS_H
#define CALC_LENS_H

#include "data/dataset.h"
#include "typ/geometry.h"
#include "typ/image_transform.h"

namespace calc {

//! View the dataset through a lens

class LensBase {
public:
    LensBase(
        Session const&, data::Datasets const&, bool trans, bool cut,
        typ::ImageTransform const&, typ::ImageCut const&);
    virtual ~LensBase() {}
    virtual typ::size2d size() const = 0;

protected:
    typ::size2d transCutSize(typ::size2d) const;

    void doTrans(uint& i, uint& j) const;
    void doCut(uint& i, uint& j) const;

    Session const& session_;
    data::Datasets const& datasets_;
    bool trans_, cut_;
    typ::ImageTransform imageTransform_;
    typ::ImageCut imageCut_;
    typ::Image const* intensCorr_;
};

class ImageLens final : public LensBase {
public:
    ImageLens(Session const&, typ::Image const&, data::Datasets const&, bool trans, bool cut);

    typ::size2d size() const;

    inten_t imageInten(uint i, uint j) const;

    typ::Range const& rgeInten(bool fixed) const;

private:
    typ::Image const& image_;

    mutable typ::Range rgeInten_;
};


typedef QSharedPointer<ImageLens> shp_ImageLens;

class DatasetLens final : public LensBase {
public:
    DatasetLens(
        Session const&, data::Dataset const&, data::Datasets const&, eNorm, bool trans,
        bool cut, typ::ImageTransform const&, typ::ImageCut const&);

    typ::size2d size() const;

    typ::Range rgeGma() const;
    typ::Range rgeGmaFull() const;
    typ::Range rgeTth() const;
    typ::Range rgeInten() const;

    typ::Curve makeCurve() const;
    typ::Curve makeCurve(typ::Range const&) const;

    data::Dataset const& dataset() const { return dataset_; }

private:
    void setNorm(eNorm);
    inten_t normFactor_;

    data::Dataset const& dataset_;
};

typedef QSharedPointer<DatasetLens> shp_DatasetLens;

} // namespace calc

#endif // CALC_LENS_H
