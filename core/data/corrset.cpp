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

Image recomputeNormalizer(const Image& corrImage)
{
    size2d size = corrImage.size();
    if (size.isEmpty())
        return {};

    // only use pixels that are not cut
    size = size - gSession->params.imageCut.marginSize();
    ASSERT(!size.isEmpty());
    int w = size.w;
    int h = size.h;
    int di = gSession->params.imageCut.left.val();
    int dj = gSession->params.imageCut.top .val();

    double sum = 0;
    for (int i=0; i<w; ++i)
        for (int j=0; j<h; ++j)
            sum += corrImage.inten2d(i + di, j + dj);
    double avg = sum / (w * h);

    Image ret(corrImage.size(), 1.);

    for (int i=0; i<w; ++i) {
        for (int j=0; j<h; ++j) {
            const float inten = corrImage.inten2d(i + di, j + dj);
            double fact;
            if (inten > 0) {
                fact = avg / inten;
            } else {
                fact = Q_QNAN;
            }
            ret.setInten2d(i + di, j + dj, float(fact));
        }
    }
    return ret;
}

} // namespace


Corrset::Corrset()
    : normalizer_{[this]()->Image{ return recomputeNormalizer(image()); }}
{}

void Corrset::clear()
{
    removeFile();
    enabled.setVal(true);
}

void Corrset::removeFile()
{
    raw_.release();
    invalidateNormalizer();
    gSession->updateImageSize();
}

void Corrset::loadFile(const QString& filePath)
{
    if (filePath.isEmpty())
        qFatal("Corrset::loadFile called with empty filePath argument");
    raw_.reset(new Rawfile(load::loadRawfile(filePath)));
    if (!raw_.get())
        return;
    gSession->setImageSize(raw_->imageSize());
    corrImage_ = raw_->summedImage();
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
