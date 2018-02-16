// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/lens.cpp
//! @brief     Implements ImageLens, SequenceLens
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"

// ************************************************************************** //
//   class ImageLens
// ************************************************************************** //

ImageLens::ImageLens(const Image& image, bool trans, bool cut)
    : cut_(cut)
    , trans_(trans)
    , image_(image)
{}

size2d ImageLens::imgSize() const {
    size2d ret = image_.size();
    if (trans_ && gSession->imageTransform().isTransposed())
        ret = ret.transposed();
    if (cut_)
        ret = ret - gSession->imageCut().marginSize();
    return ret;
}

void ImageLens::doTrans(int& x, int& y) const {
    size2d s = imgSize();
    int w = s.w;
    int h = s.h;

    switch (gSession->imageTransform().val) {
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

void ImageLens::doCut(int& i, int& j) const {
    i += gSession->imageCut().left;
    j += gSession->imageCut().top;
}


inten_t ImageLens::imageInten(int i, int j) const {
    if (trans_)
        doTrans(i, j);
    if (cut_)
        doCut(i, j);
    inten_t inten = image_.inten(i, j);
    if (gSession->intensCorr())
        inten *= gSession->intensCorr()->inten(i, j);
    return inten;
}

const Range& ImageLens::rgeInten(bool fixed) const {
    if (fixed)
        return gSession->experiment().rgeFixedInten(trans_, cut_);
    if (!rgeInten_.isValid()) {
        size2d sz = imgSize();
        for_ij (sz.w, sz.h)
            rgeInten_.extendBy(qreal(imageInten(i, j)));
    }
    return rgeInten_;
}


// ************************************************************************** //
//   class SequenceLens
// ************************************************************************** //

SequenceLens::SequenceLens(Sequence const& seq, eNorm norm, bool trans, bool cut)
    : normFactor_(1)
    , seq_(seq)
{
    setNorm(norm);
}

Curve SequenceLens::makeCurve() const {
    return makeCurve(seq_.rgeGma());
}

Curve SequenceLens::makeCurve(const Range& rgeGma) const {
    inten_vec intens = seq_.collectIntens(gSession->intensCorr(), rgeGma);
    int count = intens.count();
    if (!count)
        return {};
    Curve res;
    Range rgeTth = seq_.rgeTth();
    deg minTth = rgeTth.min;
    deg deltaTth = rgeTth.width() / count;
    for_i (count)
        res.append(minTth + deltaTth * i, qreal(intens.at(i) * normFactor_));
    return res;
}

void SequenceLens::setNorm(eNorm norm) {
    qreal num = 1, den = 1;

    switch (norm) {
    case eNorm::MONITOR:
        num = gSession->experiment().avgMonitorCount();
        den = seq_.avgMonitorCount();
        break;
    case eNorm::DELTA_MONITOR:
        num = gSession->experiment().avgDeltaMonitorCount();
        den = seq_.avgDeltaMonitorCount();
        break;
    case eNorm::DELTA_TIME:
        num = gSession->experiment().avgDeltaTime();
        den = seq_.avgDeltaTime();
        break;
    case eNorm::BACKGROUND:
        num = gSession->calcAvgBackground();
        den = gSession->calcAvgBackground(seq_);
        break;
    case eNorm::NONE:
        break;
    }

    normFactor_ = inten_t((num > 0 && den > 0) ? num / den : NAN);
    if (qIsNaN(normFactor_))
        qWarning() << "Bad normalisation value";
}
