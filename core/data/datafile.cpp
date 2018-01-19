// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/datafile.cpp
//! @brief     Implements class Datafile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/data/datafile.h"
#include <QStringBuilder> // for ".." % ..

Datafile::Datafile(rcstr fileName) : fileInfo_(fileName) {}

//! The loaders use this function to push cluster
void Datafile::addDataset(const Metadata& md, size2d const& sz, inten_vec const& ivec) {
    if (measurements_.isEmpty())
        imageSize_ = sz;
    else if (sz != imageSize_)
        THROW("Inconsistent image size in " % fileName());
    measurements_.append(shp_Measurement(
                             new Measurement(this, measurements_.count(), md, sz, ivec)));
}

QFileInfo const& Datafile::fileInfo() const {
    return fileInfo_;
}

str Datafile::fileName() const {
    return fileInfo_.fileName();
}

shp_Image Datafile::foldedImage() const {
    debug::ensure(!measurements_.isEmpty());
    shp_Image ret(new Image(measurements_.first()->imageSize()));
    for (shp_Measurement one : measurements_)
        ret->addIntens(*one->image());
    return ret;
}
