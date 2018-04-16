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

#include "core/session.h"
#include "core/loaders/loaders.h"
#include "core/def/idiomatic_for.h"

void Corrset::clear()
{
    removeFile();
    enabled_ = true;
    hasNANs_ = false;
}

void Corrset::removeFile()
{
    raw_.release();
    // TODO empty image? was corrImage_.clear();
    normalizer_.clear();
    gSession->updateImageSize();
    EMITS("Corrset::removeFile", gSession->sigCorr());
}

void Corrset::loadFile(const QString& filePath)
{
    if (filePath.isEmpty())
        THROW("invalid call of Corrset::loadFile with empty filePath argument");
    raw_.reset(new Rawfile(load::loadRawfile(filePath)));
    if (!raw_.get())
        return;
    gSession->setImageSize(raw_->imageSize());
    corrImage_.reset(new Image{raw_->summedImage()});
    normalizer_.clear(); // will be calculated lazily
    // all ok
    enabled_ = true;
    EMITS("Corrset::loadFile", gSession->sigCorr());
}

void Corrset::tryEnable(bool on)
{
    if ((on && !hasFile()) || on==enabled_)
        return;
    enabled_ = on;
    EMITS("Corrset::tryEnable", gSession->sigCorr());
}

const Image* Corrset::normalizer() const
{
    if (!hasFile() || !enabled_)
        return nullptr;
    if (normalizer_.isEmpty())
        calcNormalizer();
    return &normalizer_;
}

void Corrset::calcNormalizer() const
{
    hasNANs_ = false;

    ASSERT(corrImage_);
    size2d size = corrImage_->size() - gSession->imageCut().marginSize();
    ASSERT(!size.isEmpty());

    int w = size.w, h = size.h;
    int di = gSession->imageCut().left(), dj = gSession->imageCut().top();

    double sum = 0;
    for_ij (w, h)
        sum += corrImage_->inten2d(i + di, j + dj);
    double avg = sum / (w * h);

    normalizer_.fill(1, corrImage_->size());

    for_ij (w, h) {
        const float inten = corrImage_->inten2d(i + di, j + dj);
        double fact;
        if (inten > 0) {
            fact = avg / inten;
        } else {
            fact = Q_QNAN;
            hasNANs_ = true;
        }
        normalizer_.setInten2d(i + di, j + dj, float(fact));
    }
}

QJsonObject Corrset::toJson() const
{
    QJsonObject ret;
    if (hasFile())
        ret.insert("file", raw_->fileInfo().absoluteFilePath());
    ret.insert("enabled", enabled_);
    return ret;
}

void Corrset::fromJson(const JsonObj& obj)
{
    if (obj.find("file") != obj.end())
        loadFile(obj.loadString("file"));
    tryEnable(obj.loadBool("enabled", true));
}
