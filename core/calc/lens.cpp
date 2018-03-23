// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/lens.cpp
//! @brief     Implements ImageLens
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
    : trans_(trans)
    , cut_(cut)
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
    i += gSession->imageCut().left();
    j += gSession->imageCut().top();
}


inten_t ImageLens::imageInten(int i, int j) const {
    if (trans_)
        doTrans(i, j);
    if (cut_)
        doCut(i, j);
    inten_t inten = image_.inten(i, j);
    if (auto* corr = gSession->corrset().intensCorr())
        inten *= corr->inten(i, j);
    return inten;
}

const Range& ImageLens::rgeInten(bool fixed) const {
    if (fixed)
        return gSession->activeClusters().rgeFixedInten(trans_, cut_);
    if (!rgeInten_.isValid()) {
        size2d sz = imgSize();
        for_ij (sz.w, sz.h)
            rgeInten_.extendBy(qreal(imageInten(i, j)));
    }
    return rgeInten_;
}
