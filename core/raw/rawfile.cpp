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

#include "core/raw/rawfile.h"
#include <QStringBuilder> // for ".." % ..

Rawfile::Rawfile(const QString& fileName)
    : fileInfo_(fileName)
{}

//! The loaders use this function to push cluster
void Rawfile::addDataset(const Metadata& md, const size2d& sz, const QVector<float>& ivec)
{
    if (measurements_.isEmpty())
        imageSize_ = sz;
    else if (sz != imageSize_)
        THROW("Inconsistent image size in " % fileName());
    measurements_.append(shp_Measurement(new Measurement(measurements_.count(), md, sz, ivec)));
}

QVector<const Measurement*> const Rawfile::measurements() const
{
    QVector<const Measurement*> ret;
    for (const shp_Measurement& one: measurements_)
        ret.append(one.data());
    return ret;
}

Image* Rawfile::foldedImage() const
{
    ASSERT(!measurements_.isEmpty());
    auto* ret = new Image(measurements_.first()->imageSize());
    for (shp_Measurement one : measurements_)
        ret->addIntens(one->image());
    return ret;
}
