// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/calc/calc_lens.cpp
//! @brief     Implements LensBase, ImageLens, SequenceLens
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "session.h"
#include "data/suite.h"

namespace calc {

// ************************************************************************** //
//   class LensBase
// ************************************************************************** //

LensBase::LensBase(
    Session const& session, Experiment const& datasequence, bool trans, bool cut,
    ImageTransform const& imageTransform, ImageCut const& imageCut)
    : session_(session)
    , experiment_(datasequence)
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
    Session const& session, Image const& image, Experiment const& datasequence, bool trans,
    bool cut)
    : LensBase(session, datasequence, trans, cut, session.imageTransform(), session.imageCut())
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
        return experiment_.rgeFixedInten(session_, trans_, cut_);
    if (!rgeInten_.isValid()) {
        auto sz = size();
        for_ij (sz.w, sz.h)
            rgeInten_.extendBy(qreal(imageInten(i, j)));
    }
    return rgeInten_;
}


// ************************************************************************** //
//   class SequenceLens
// ************************************************************************** //

SequenceLens::SequenceLens(
    Session const& session, DataSequence const& dataseq, Experiment const& datasequence,
    eNorm norm, bool trans, bool cut, ImageTransform const& imageTransform,
    ImageCut const& imageCut)
    : LensBase(session, datasequence, trans, cut, imageTransform, imageCut)
    , normFactor_(1)
    , dataseq_(dataseq) {
    setNorm(norm);
}

size2d SequenceLens::size() const {
    return LensBase::transCutSize(experiment_.imageSize());
}

Range SequenceLens::rgeGma() const {
    return dataseq_.rgeGma(session_);
}

Range SequenceLens::rgeGmaFull() const {
    return dataseq_.rgeGmaFull(session_);
}

Range SequenceLens::rgeTth() const {
    return dataseq_.rgeTth(session_);
}

Range SequenceLens::rgeInten() const {
    // fixes the scale
    // TODO consider return experiment_.rgeInten();
    return dataseq_.rgeInten();
}

Curve SequenceLens::makeCurve() const {
    return makeCurve(rgeGma());
}

Curve SequenceLens::makeCurve(Range const& rgeGma) const {
    inten_vec intens = dataseq_.collectIntens(session_, intensCorr_, rgeGma);
    Curve res;
    uint count = intens.count();
    if (count) {
        Range rgeTth = dataseq_.rgeTth(session_);
        deg minTth = rgeTth.min, deltaTth = rgeTth.width() / count;
        for_i (count)
            res.append(minTth + deltaTth * i, qreal(intens.at(i) * normFactor_));
    }
    return res;
}

void SequenceLens::setNorm(eNorm norm) {
    qreal num = 1, den = 1;

    switch (norm) {
    case eNorm::MONITOR:
        num = experiment_.avgMonitorCount();
        den = dataseq_.avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = experiment_.avgDeltaMonitorCount();
        den = dataseq_.avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = experiment_.avgDeltaTime();
        den = dataseq_.avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = session_.calcAvgBackground(experiment_);
        den = session_.calcAvgBackground(dataseq_);
        break;
    case eNorm::NONE: break;
    }

    normFactor_ = inten_t((num > 0 && den > 0) ? num / den : NAN);
    if (qIsNaN(normFactor_))
        qWarning() << "Bad normalisation value";
}

} // namespace calc
