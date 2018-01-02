// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/datafile.h
//! @brief     Defines class File
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DATAFILE_H
#define DATAFILE_H

#include "dataset.h"
#include <QFileInfo>

//! A file (loaded from a disk file) that contains a number of datasets.
class Datafile final {
public:
    Datafile(rcstr fileName);

    QFileInfo const& fileInfo() const;
    str fileName() const;

    OneDatasets& datasets() { return datasets_; }
    OneDatasets const& datasets() const { return datasets_; }

private:
    QFileInfo fileInfo_;
    OneDatasets datasets_;
};

typedef QSharedPointer<class Datafile> shp_Datafile;

Q_DECLARE_METATYPE(shp_Datafile)

#endif // DATAFILE_H
