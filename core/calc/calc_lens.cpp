// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_lens.cpp
//! @brief     Implements LensBase, ImageLens, DatasetLens
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "def/idiomatic_for.h"
#include "session.h"
#include "typ/typ_log.h"

namespace calc {

using typ::Curve;
using typ::Image;
using typ::ImageTransform;
using typ::size2d;

// ************************************************************************** //
//   class LensBase
// ************************************************************************** //

LensBase::LensBase(
    core::Session const& session, data::Datasets const& datasets, bool trans, bool cut,
    typ::ImageTransform const& imageTransform, typ::ImageCut const& imageCut)
    : session_(session)
    , datasets_(datasets)
    , trans_(trans)
    , cut_(cut)
    , imageTransform_(imageTransform)
    , imageCut_(imageCut)
    , intensCorr_(session.intensCorr()) {}

size2d LensBase::transCutSize(size2d size) const {
    if (trans_ && imageTransform_.isTransposed())
        size = size.transposed();

    if (cut_)
        size = size - imageCut_.marginSize();

    return size;
}

void LensBase::doTrans(uint& x, uint& y) const {
    auto s = size();
    uint w = s.w;
    uint h = s.h;

    switch (imageTransform_.val) {
    case ImageTransform::ROTATE_0: break;
    case ImageTransform::ROTATE_1:
        qSwap(x, y);
        y = w - y - 1;
        break;
    case ImageTransform::ROTATE_2:
        x = w - x - 1;
        y = h - y - 1;
        break;
    case ImageTransform::ROTATE_3:
        qSwap(x, y);
        x = h - x - 1;
        break;
    case ImageTransform::MIRROR_ROTATE_0: x = w - x - 1; break;
    case ImageTransform::MIRROR_ROTATE_1:
        y = h - y - 1;
        qSwap(x, y);
        y = w - y - 1;
        break;
    case ImageTransform::MIRROR_ROTATE_2: y = h - y - 1; break;
    case ImageTransform::MIRROR_ROTATE_3: qSwap(x, y); break;
    }
}

void LensBase::doCut(uint& i, uint& j) const {
    i += imageCut_.left;
    j += imageCut_.top;
}


// ************************************************************************** //
//   class ImageLens
// ************************************************************************** //

ImageLens::ImageLens(
    core::Session const& session, Image const& image, data::Datasets const& datasets, bool trans,
    bool cut)
    : super(session, datasets, trans, cut, session.imageTransform(), session.imageCut())
    , image_(image) {}

size2d ImageLens::size() const {
    return super::transCutSize(image_.size());
}

inten_t ImageLens::imageInten(uint i, uint j) const {
    if (trans_)
        doTrans(i, j);
    if (cut_)
        doCut(i, j);

    inten_t inten = image_.inten(i, j);
    if (intensCorr_)
        inten *= intensCorr_->inten(i, j);

    return inten;
}

inten_rge const& ImageLens::rgeInten(bool fixed) const {
    if (fixed)
        return datasets_.rgeFixedInten(session_, trans_, cut_);

    if (!rgeInten_.isValid()) {
        auto sz = size();
        for_ij (sz.w, sz.h)
            rgeInten_.extendBy(qreal(imageInten(i, j)));
    }

    return rgeInten_;
}


// ************************************************************************** //
//   class DatasetLens
// ************************************************************************** //

DatasetLens::DatasetLens(
    core::Session const& session, data::Dataset const& dataset, data::Datasets const& datasets,
    eNorm norm, bool trans, bool cut, typ::ImageTransform const& imageTransform,
    typ::ImageCut const& imageCut)
    : super(session, datasets, trans, cut, imageTransform, imageCut)
    , normFactor_(1)
    , dataset_(dataset) {
    setNorm(norm);
}

size2d DatasetLens::size() const {
    return super::transCutSize(datasets_.imageSize());
}

gma_rge DatasetLens::rgeGma() const {
    return dataset_.rgeGma(session_);
}

gma_rge DatasetLens::rgeGmaFull() const {
    return dataset_.rgeGmaFull(session_);
}

tth_rge DatasetLens::rgeTth() const {
    return dataset_.rgeTth(session_);
}

inten_rge DatasetLens::rgeInten() const {
    // fixes the scale
    // TODO consider return datasets_.rgeInten();
    return dataset_.rgeInten();
}

Curve DatasetLens::makeCurve() const {
    return makeCurve(rgeGma());
}

Curve DatasetLens::makeCurve(gma_rge const& rgeGma) const {
    inten_vec intens = dataset_.collectIntens(session_, intensCorr_, rgeGma);

    Curve res;
    uint count = intens.count();

    if (count) {
        tth_rge rgeTth = dataset_.rgeTth(session_);
        deg minTth = rgeTth.min, deltaTth = rgeTth.width() / count;
        for_i (count)
            res.append(minTth + deltaTth * i, qreal(intens.at(i) * normFactor_));
    }

    return res;
}

void DatasetLens::setNorm(eNorm norm) {
    qreal num = 1, den = 1;

    switch (norm) {
    case eNorm::MONITOR:
        num = datasets_.avgMonitorCount();
        den = dataset_.avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = datasets_.avgDeltaMonitorCount();
        den = dataset_.avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = datasets_.avgDeltaTime();
        den = dataset_.avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = session_.calcAvgBackground(datasets_);
        den = session_.calcAvgBackground(dataset_);
        break;
    case eNorm::NONE: break;
    }

    normFactor_ = inten_t((num > 0 && den > 0) ? num / den : NAN);
    if (qIsNaN(normFactor_))
        MessageLogger::warn("Bad normalisation value");
}

} // namespace calc
