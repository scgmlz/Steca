//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/rawfile.h
//! @brief     Defines class Rawfile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RAWFILE_H
#define RAWFILE_H

#include "core/raw/measurement.h"
#include <QFileInfo>

//! A file (loaded from a disk file) that contains a data sequence.
class Rawfile final {
public:
    Rawfile() = delete;
    Rawfile(const Rawfile&) = delete;
    // allow move so that the low-level loaders must not bother about shared pointers:
    Rawfile(Rawfile&&) = default;
    Rawfile(const QString& fileName);

    void addDataset(const Metadata&, const size2d&, const QVector<float>&);

    QVector<const Measurement*> const measurements() const;
    int numMeasurements() const { return measurements_.count(); }
    size2d imageSize() const { return imageSize_; }

    const QFileInfo& fileInfo() const { return fileInfo_; }
    QString fileName() const { return fileInfo_.fileName(); }

    Image* foldedImage() const;

private:
    QFileInfo fileInfo_;
    QVector<shp_Measurement> measurements_; // TODO EASY -> unique_ptr
    size2d imageSize_;
};

Q_DECLARE_METATYPE(const Rawfile*)

#endif // RAWFILE_H
