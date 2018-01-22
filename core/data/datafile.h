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

class Metadata;

//! A file (loaded from a disk file) that contains a data sequence.
class Datafile final {
public:
    Datafile() = delete;
    Datafile(const Datafile&) = delete;
    // allow move so that the low-level loaders must not bother about shared pointers:
    Datafile(Datafile&&) = default;
    Datafile(rcstr fileName);

    void addDataset(const Metadata&, size2d const&, inten_vec const&);

    vec<shp_Measurement> const& measurements() const { return measurements_; }
    int count() const { return measurements_.count(); }
    size2d imageSize() const { return imageSize_; }

    QFileInfo const& fileInfo() const;
    str fileName() const;
    shp_Image foldedImage() const;

private:
    QFileInfo fileInfo_;
    vec<shp_Measurement> measurements_;
    size2d imageSize_;
};

Q_DECLARE_METATYPE(const Datafile*)

#endif // DATAFILE_H
