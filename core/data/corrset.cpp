//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/corrset.cpp
//! @brief     Implements class Corrset
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/corrset.h"
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "qcr/engine/mixin.h" // remakeAll
#include "qcr/base/debug.h"

namespace {

Image recomputeNormalizer(const Corrset* parent)
{
    /*
    ASSERT(corrImage_.get());
    size2d size = corrImage_->size() - gSession->params.imageCut.marginSize();
    ASSERT(!size.isEmpty());

    int w = size.w, h = size.h;
    int di = gSession->params.imageCut.left.val(), dj = gSession->params.imageCut.top.val();

    double sum = 0;
    for (int i=0; i<w; ++i)
        for (int j=0; j<h; ++j)
            sum += corrImage_->inten2d(i + di, j + dj);
    double avg = sum / (w * h);

    normalizer_.reset(new Image(corrImage_->size(), 1.));

    for (int i=0; i<w; ++i) {
        for (int j=0; j<h; ++j) {
            const float inten = corrImage_->inten2d(i + di, j + dj);
            double fact;
            if (inten > 0) {
                fact = avg / inten;
            } else {
                fact = Q_QNAN;
            }
            normalizer_->setInten2d(i + di, j + dj, float(fact));
        }
    }
    */
    return {};
}

} // namespace


Corrset::Corrset()
    : normalizer_{[this]()->Image{ return recomputeNormalizer(this); }}
{}

void Corrset::clear()
{
    removeFile();
    enabled.setVal(true);
}

void Corrset::removeFile()
{
    raw_.release();
    // TODO empty image? was corrImage_.clear();
    invalidateNormalizer();
    gSession->updateImageSize(); // TODO check, was marked "UNDER CONSTRUCTION"
}

void Corrset::loadFile(const QString& filePath)
{
    if (filePath.isEmpty())
        qFatal("Corrset::loadFile called with empty filePath argument");
    raw_.reset(new Rawfile(load::loadRawfile(filePath)));
    if (!raw_.get())
        return;
    gSession->setImageSize(raw_->imageSize());
    corrImage_.reset(new Image{raw_->summedImage()});
    invalidateNormalizer();
    // all ok
    enabled.setVal(true);
    gRoot->remakeAll();
}

QJsonObject Corrset::toJson() const
{
    QJsonObject ret;
    if (hasFile())
        ret.insert("file", raw_->fileInfo().absoluteFilePath());
    ret.insert("enabled", enabled.val());
    return ret;
}

void Corrset::fromJson(const JsonObj& obj)
{
    if (obj.find("file") != obj.end())
        loadFile(obj.loadString("file"));
    enabled.setVal(obj.loadBool("enabled", true));
}
