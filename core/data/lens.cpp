//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/lens.cpp
//! @brief     Implements ImageLens
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/lens.h"
#include "core/session.h"

//  ***********************************************************************************************
//! @class ImageLens

ImageLens::ImageLens(const Image& image, bool trans, bool cut)
    : trans_(trans)
    , cut_(cut) // if true then remove borders for good; this would rescale the rendered image
    , image_(image)
{}

size2d ImageLens::imgSize() const
{
    size2d ret = image_.size();
    if (trans_ && gSession->params.imageTransform.isTransposed())
        ret = ret.transposed();
    if (cut_)
        ret = ret - gSession->params.imageCut.marginSize();
    return ret;
}

void ImageLens::doTrans(int& x, int& y) const
{
    size2d s = imgSize();
    int w = s.w;
    int h = s.h;

    const ImageTransform& trafo = gSession->params.imageTransform;
    if        (trafo.rotation==0 && !trafo.mirror) {
        ; // do nothing
    } else if (trafo.rotation==1 && !trafo.mirror) {
        qSwap(x, y);
        y = w - y - 1;
    } else if (trafo.rotation==2 && !trafo.mirror) {
        x = w - x - 1;
        y = h - y - 1;
    } else if (trafo.rotation==3 && !trafo.mirror) {
        qSwap(x, y);
        x = h - x - 1;
    } else if (trafo.rotation==0 &&  trafo.mirror) {
        x = w - x - 1;
    } else if (trafo.rotation==1 &&  trafo.mirror) {
        y = h - y - 1;
        qSwap(x, y);
        y = w - y - 1;
    } else if (trafo.rotation==2 &&  trafo.mirror) {
        y = h - y - 1;
    } else if (trafo.rotation==3 &&  trafo.mirror) {
        qSwap(x, y);
    }
}

void ImageLens::doCut(int& i, int& j) const
{
    i += gSession->params.imageCut.left.val();
    j += gSession->params.imageCut.top.val();
}


float ImageLens::imageInten(int i, int j) const
{
    if (trans_)
        doTrans(i, j);
    if (cut_)
        doCut(i, j);
    float ret = image_.inten2d(i, j);
    const Image& normalizer = gSession->corrset.getNormalizer();
    if (!normalizer.isEmpty())
        ret *= normalizer.inten2d(i, j);
    return ret;
}

const Range& ImageLens::rgeInten(bool fixed) const
{
    if (fixed)
        return gSession->activeClusters.rgeFixedInten.get(); // TODO restore from pre d9714895: (trans_, cut_);
    if (!rgeInten_.isValid()) {
        size2d sz = imgSize();
        for (int j=0; j<sz.h; ++j)
            for (int i=0; i<sz.w; ++i)
                rgeInten_.extendBy(double(imageInten(i,j)));
    }
    return rgeInten_;
}
