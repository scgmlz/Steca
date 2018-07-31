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

#include "core/def/idiomatic_for.h"
#include "core/loaders/loaders.h"
#include "core/session.h"
#include "qcr/base/debug.h"

void Corrset::clear()
{
    removeFile();
    enabled.setVal(true);
    hasNANs_ = false;
}

void Corrset::removeFile()
{
    raw_.release();
    // TODO empty image? was corrImage_.clear();
    normalizer_.release();
    gSession->updateImageSize(); /* UNDER CONSTRUCTION */
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
    normalizer_.release(); // will be calculated when needed
    // all ok
    enabled.setVal(true);
}

const Image* Corrset::normalizer() const
{
    if (!hasFile() || !enabled.val())
        return nullptr;
    if (!normalizer_.get())
        calcNormalizer();
    return normalizer_.get();
}

void Corrset::calcNormalizer() const
{
    hasNANs_ = false;

    ASSERT(corrImage_.get());
    size2d size = corrImage_->size() - gSession->imageCut.marginSize();
    ASSERT(!size.isEmpty());

    int w = size.w, h = size.h;
    int di = gSession->imageCut.left.val(), dj = gSession->imageCut.top.val();

    double sum = 0;
    for_ij (w, h)
        sum += corrImage_->inten2d(i + di, j + dj);
    double avg = sum / (w * h);

    normalizer_.reset(new Image(corrImage_->size(), 1.));

    for_ij (w, h) {
        const float inten = corrImage_->inten2d(i + di, j + dj);
        double fact;
        if (inten > 0) {
            fact = avg / inten;
        } else {
            fact = Q_QNAN;
            hasNANs_ = true;
        }
        normalizer_->setInten2d(i + di, j + dj, float(fact));
    }
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
