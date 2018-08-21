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
    Rawfile(const QString& fileName);
    Rawfile(const Rawfile&) = delete;
    Rawfile(Rawfile&&) = default; // needed by loaders
    Rawfile& operator=(Rawfile&&) = default;

    void addDataset(Metadata&&, const size2d&, std::vector<float> &&);

    std::vector<const Measurement*> const measurements() const;
    int numMeasurements() const { return measurements_.size(); }
    size2d imageSize() const { return imageSize_; }

    const QFileInfo& fileInfo() const { return fileInfo_; }
    QString fileName() const { return fileInfo_.fileName(); }

    Image summedImage() const;

private:
    QFileInfo fileInfo_;
    std::vector<Measurement> measurements_;
    size2d imageSize_;
};

#endif // RAWFILE_H
