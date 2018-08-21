//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/corrset.h
//! @brief     Defines class Corrset
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CORRSET_H
#define CORRSET_H

#include "core/raw/image.h"
#include "core/raw/rawfile.h"
#include "core/typ/cached.h"
#include "qcr/engine/cell.h"
#include <memory>

//! A correction dataset, consisting of one Rawfile and associated settings.

//! Note that "correction", throughout Steca, means "calibration" or "normalization".

class Corrset {
public:
    Corrset();
    Corrset(const Corrset&) = delete;

    void fromJson(const JsonObj& obj);
    void clear();
    void removeFile();
    void loadFile(const QString& filePath);

    QJsonObject toJson() const;
    const Rawfile& raw() const { return *raw_; }
    bool hasFile() const { return raw_.get(); }
    QString fileName() const { return hasFile() ? raw_->fileName() : ""; }
    const Image& image() const { return corrImage_; }
    void invalidateNormalizer() const { normalizer_.invalidate(); }
    const Image& getNormalizer() const { return normalizer_.get(); }

    QcrCell<bool> enabled {true};

private:
    std::unique_ptr<const Rawfile> raw_; //!< owned by this
    Image corrImage_;
    mutable Cached<Image> normalizer_;
};

#endif // CORRSET_H
