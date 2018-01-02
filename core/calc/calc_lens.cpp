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

#include "session.h"

namespace calc {

// ************************************************************************** //
//   class LensBase
// ************************************************************************** //

LensBase::LensBase(
    Session const& session, Datasets const& datasets, bool trans, bool cut,
    ImageTransform const& imageTransform, ImageCut const& imageCut)
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
    Session const& session, Image const& image, Datasets const& datasets, bool trans,
    bool cut)
    : LensBase(session, datasets, trans, cut, session.imageTransform(), session.imageCut())
    , image_(image) {}

size2d ImageLens::size() const {
    return LensBase::transCutSize(image_.size());
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

Range const& ImageLens::rgeInten(bool fixed) const {
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
    Session const& session, Dataset const& dataset, Datasets const& datasets,
    eNorm norm, bool trans, bool cut, ImageTransform const& imageTransform,
    ImageCut const& imageCut)
    : LensBase(session, datasets, trans, cut, imageTransform, imageCut)
    , normFactor_(1)
    , dataset_(dataset) {
    setNorm(norm);
}

size2d DatasetLens::size() const {
    return LensBase::transCutSize(datasets_.imageSize());
}

Range DatasetLens::rgeGma() const {
    return dataset_.rgeGma(session_);
}

Range DatasetLens::rgeGmaFull() const {
    return dataset_.rgeGmaFull(session_);
}

Range DatasetLens::rgeTth() const {
    return dataset_.rgeTth(session_);
}

Range DatasetLens::rgeInten() const {
    // fixes the scale
    // TODO consider return datasets_.rgeInten();
    return dataset_.rgeInten();
}

Curve DatasetLens::makeCurve() const {
    return makeCurve(rgeGma());
}

Curve DatasetLens::makeCurve(Range const& rgeGma) const {
    inten_vec intens = dataset_.collectIntens(session_, intensCorr_, rgeGma);
    Curve res;
    uint count = intens.count();
    if (count) {
        Range rgeTth = dataset_.rgeTth(session_);
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
        qWarning() << "Bad normalisation value";
}

} // namespace calc
