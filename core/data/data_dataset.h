// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/data_dataset.h
//! @brief     Defines classes Metadata, [One]Dataset[s]
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATA_DATASET_H
#define DATA_DATASET_H

#include "typ/curve.h"
#include "typ/image.h"
#include "typ/str.h"
#include "typ/variant.h"
#include <QSharedPointer>

namespace core {
class Session;
}

namespace data {

struct Metadata;
class OneDataset;
class Dataset;
class Datasets;

typedef QSharedPointer<Metadata const> shp_Metadata; // no changing these
typedef QSharedPointer<OneDataset const> shp_OneDataset;
typedef QSharedPointer<Dataset> shp_Dataset;


struct Metadata {
    Metadata();

    // attribute list - will be dynamic
    static uint numAttributes(bool onlyNum);

    static rcstr attributeTag(uint, bool out);
    static str_lst attributeTags(bool out);
    static typ::cmp_vec attributeCmps();

    str attributeStrValue(uint) const;
    QVariant attributeValue(uint) const;
    typ::row_t attributeValues() const;

    static typ::row_t attributeNaNs();

    str date, comment;

    typ::deg motorXT, motorYT, motorZT, motorOmg, motorTth, motorPhi, motorChi, motorPST, motorSST,
        motorOMGM;

    // new metadata
    qreal nmT, nmTeload, nmTepos, nmTeext, nmXe, nmYe, nmZe;

    qreal monitorCount, deltaMonitorCount;
    qreal time, deltaTime;
};


//! Metadata + Image, for calculation always accessed through its owning Dataset

class OneDataset final {
    friend class OneDatasets;
    friend class Dataset;

public:
    OneDataset(Metadata const&, typ::inten_arr const&);
    OneDataset(Metadata const&, typ::size2d const&, inten_vec const&);
    OneDataset(OneDataset const&);

    shp_Metadata metadata() const;

    typ::deg midTth() const { return md_->motorTth; }

    qreal monitorCount() const { return md_->monitorCount; }
    qreal deltaMonitorCount() const { return md_->deltaMonitorCount; }
    qreal deltaTime() const { return md_->deltaTime; }

    typ::deg omg() const { return md_->motorOmg; }
    typ::deg phi() const { return md_->motorPhi; }
    typ::deg chi() const { return md_->motorChi; }

    gma_rge rgeGma(core::Session const&) const;
    gma_rge rgeGmaFull(core::Session const&) const;
    tth_rge rgeTth(core::Session const&) const;

    inten_rge rgeInten() const;

    typ::shp_Image image() const { return image_; }
    typ::size2d imageSize() const;

    void collectIntens(
        core::Session const&, typ::Image const* intensCorr, inten_vec&, uint_vec&, gma_rge const&,
        typ::deg minTth, typ::deg deltaTth) const;

private:
    shp_Metadata md_;
    typ::shp_Image image_;
};


//! Collection of (OneDataset)s

class OneDatasets : public typ::vec<shp_OneDataset> {
private:
    using super = typ::vec<shp_OneDataset>;
public:
    typ::size2d imageSize() const;
    typ::shp_Image foldedImage() const;
};

//! One or more OneDataset(s)

class Dataset final : public OneDatasets {
private:
    using super = OneDatasets;friend class Datasets;

public:
    Dataset();

    shp_Metadata metadata() const;
    Datasets const& datasets() const;

    typ::deg omg() const;
    typ::deg phi() const;
    typ::deg chi() const;

    gma_rge rgeGma(core::Session const&) const;
    gma_rge rgeGmaFull(core::Session const&) const;
    tth_rge rgeTth(core::Session const&) const;

    inten_rge rgeInten() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    inten_vec collectIntens(core::Session const&, typ::Image const* intensCorr, gma_rge const&) const;
    void calculateAlphaBeta(typ::deg tth, typ::deg gma, typ::deg& alpha, typ::deg& beta) const;

private:
    // all dataset(s) must have the same image size
    typ::size2d imageSize() const;

    Datasets* datasets_; // here it belongs (or can be nullptr)
    shp_Metadata md_; // on demand, compute once
};

//! Collection of (Dataset)s

class Datasets final : public typ::vec<shp_Dataset> {
private:
    using super = typ::vec<shp_Dataset>;
public:
    Datasets();

    void appendHere(shp_Dataset);

    typ::size2d imageSize() const;

    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    inten_rge const& rgeGma(core::Session const&) const;
    inten_rge const& rgeFixedInten(core::Session const&, bool trans, bool cut) const;

    typ::Curve avgCurve(core::Session const&) const;

    void invalidateAvgMutables() const;

private:
    shp_Dataset combineAll() const;
    qreal calcAvgMutable(qreal (Dataset::*avgMth)() const) const;

    // computed on demand (NaNs or emptiness indicate yet unknown values)
    mutable qreal avgMonitorCount_, avgDeltaMonitorCount_, avgDeltaTime_;
    mutable inten_rge rgeFixedInten_;
    mutable gma_rge rgeGma_;
    mutable typ::Curve avgCurve_;
};

} // namespace data

Q_DECLARE_METATYPE(data::shp_Dataset)

#endif // DATA_DATASET_H
