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

#include "core/typ/array2d.h"
#include "core/typ/str.h"
#include "core/typ/types.h"
#include <QFileInfo>
#include <QSharedPointer> // no auto rm

class Measurement;
class Metadata;

//! A file (loaded from a disk file) that contains a data sequence.
class Datafile final {
public:
    Datafile(rcstr fileName);
    void addDataset(Metadata const&, size2d const&, inten_vec const&);

    vec<QSharedPointer<Measurement const>> const& suite() const { return experiment_; }
    size2d imageSize() const { return imageSize_; }

    QFileInfo const& fileInfo() const;
    str fileName() const;
    QSharedPointer<class Image> foldedImage() const;

private:
    QFileInfo fileInfo_;
    vec<QSharedPointer<class Measurement const>> experiment_;
    size2d imageSize_;
};

Q_DECLARE_METATYPE(QSharedPointer<Datafile const>)

#endif // DATAFILE_H
