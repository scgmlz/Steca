// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/datafile.h
//! @brief     Defines class Datafile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATAFILE_H
#define DATAFILE_H

#include "core/data/image.h"
#include "core/data/measurement.h"
#include "core/typ/array2d.h"
#include "core/typ/str.h"
#include "core/typ/types.h"
#include <QFileInfo>
#include <QSharedPointer> // no auto rm

class Metadata;

//! A file (loaded from a disk file) that contains a data sequence.
class Datafile final {
public:
    Datafile(rcstr fileName);
    void addDataset(const Metadata&, size2d const&, inten_vec const&);

    vec<shp_Measurement> const& suite() const { return measurements_; }
    size2d imageSize() const { return imageSize_; }

    QFileInfo const& fileInfo() const;
    str fileName() const;
    shp_Image foldedImage() const;

private:
    QFileInfo fileInfo_;
    vec<shp_Measurement> measurements_;
    size2d imageSize_;
};

typedef QSharedPointer<Datafile const> shp_Datafile;

Q_DECLARE_METATYPE(shp_Datafile)

#endif // DATAFILE_H
