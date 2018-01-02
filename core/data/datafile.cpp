// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/datafile.cpp
//! @brief     Implements class Datafile
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "datafile.h"
#include <QStringBuilder> // for ".." % ..

Datafile::Datafile(rcstr fileName) : fileInfo_(fileName) {}

//! The loaders use this function to push datasets
void Datafile::addDataset(Metadata const& md, typ::size2d const& sz, inten_vec const& ivec) {
    if (datasets_.isEmpty())
        imageSize_ = sz;
    else if (sz != imageSize_)
        THROW("Inconsistent image size in " % fileName());
    datasets_.append(shp_OneDataset(new OneDataset(md, sz, ivec)));
}

QFileInfo const& Datafile::fileInfo() const {
    return fileInfo_;
}

str Datafile::fileName() const {
    return fileInfo_.fileName();
}
