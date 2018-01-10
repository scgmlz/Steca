// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/lens.cpp
//! @brief     Implements LensBase, ImageLens, SequenceLens
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"

// ************************************************************************** //
//   class LensBase
// ************************************************************************** //

LensBase::LensBase(bool trans, bool cut,
    ImageTransform const& imageTransform, ImageCut const& imageCut)
    : trans_(trans)
    , cut_(cut)
    , imageTransform_(imageTransform)
    , imageCut_(imageCut)
    , intensCorr_(gSession->intensCorr()) {}

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

ImageLens::ImageLens(Image const& image, bool trans, bool cut)
    : LensBase(trans, cut, gSession->imageTransform(), gSession->imageCut())
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
        return gSession->experiment().rgeFixedInten(trans_, cut_);
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
    Suite const& suite, eNorm norm, bool trans, bool cut,
    ImageTransform const& imageTransform, ImageCut const& imageCut)
    : LensBase(trans, cut, imageTransform, imageCut)
    , normFactor_(1)
    , suite_(suite) {
    setNorm(norm);
}

size2d SequenceLens::size() const {
    return LensBase::transCutSize(gSession->experiment().imageSize());
}

Range SequenceLens::rgeGma() const {
    return suite_.rgeGma();
}

Range SequenceLens::rgeGmaFull() const {
    return suite_.rgeGmaFull();
}

Range SequenceLens::rgeTth() const {
    return suite_.rgeTth();
}

Range SequenceLens::rgeInten() const {
    // fixes the scale
    // TODO consider return gSession->experiment().rgeInten();
    return suite_.rgeInten();
}

Curve SequenceLens::makeCurve() const {
    return makeCurve(rgeGma());
}

Curve SequenceLens::makeCurve(Range const& rgeGma) const {
    inten_vec intens = suite_.collectIntens(intensCorr_, rgeGma);
    Curve res;
    uint count = intens.count();
    if (count) {
        Range rgeTth = suite_.rgeTth();
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
        num = gSession->experiment().avgMonitorCount();
        den = suite_.avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = gSession->experiment().avgDeltaMonitorCount();
        den = suite_.avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = gSession->experiment().avgDeltaTime();
        den = suite_.avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = gSession->calcAvgBackground();
        den = gSession->calcAvgBackground(suite_);
        break;
    case eNorm::NONE:
        break;
    }

    normFactor_ = inten_t((num > 0 && den > 0) ? num / den : NAN);
    if (qIsNaN(normFactor_))
        qWarning() << "Bad normalisation value";
}
