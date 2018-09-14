//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/rawfile.cpp
//! @brief     Implements class Rawfile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/misc/exception.h"
#include "core/raw/rawfile.h"
#include "qcr/base/debug.h"
#include <QStringBuilder> // for ".." % ..

Rawfile::Rawfile(const QString& fileName)
    : fileInfo_(fileName)
{}

//! The loaders use this function to push cluster
void Rawfile::addDataset(Metadata&& md, const size2d& sz, std::vector<float>&& ivec)
{
    if (!measurements_.size())
        imageSize_ = sz;
    else if (sz != imageSize_)
        THROW("Inconsistent image size in " % fileName());
    measurements_.push_back({(int)measurements_.size(), std::move(md), sz, std::move(ivec)});
}

std::vector<const Measurement*> const Rawfile::measurements() const
{
    std::vector<const Measurement*> ret;
    for (const Measurement& one: measurements_)
        ret.push_back(&one);
    return ret;
}

Image Rawfile::summedImage() const
{
    ASSERT(measurements_.size());
    // TODO start from the first given image, not from a zero image
    Image ret(measurements_.front().imageSize(), 0.);
    for (const Measurement& one : measurements_)
        ret.addImage(one.image());
    return ret;
}
