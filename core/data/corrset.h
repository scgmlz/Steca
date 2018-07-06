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
#include <memory>

//! A correction dataset, consisting of one Rawfile and associated settings.

//! Note that "correction", as used throughout Steca, rather means "calibration" or "normalization".

class Corrset {
public:
    // Modifying methods:
    void clear();
    void fromJson(const JsonObj& obj);
    void removeFile();
    void loadFile(const QString& filePath);
    ParamWrapper<bool> enabled {true};
    void clearIntens() { normalizer_.release(); }

    // Lookup methods
    const Rawfile& raw() const { return *raw_; }
    bool hasFile() const { return raw_.get(); }
    QString fileName() const { return hasFile() ? raw_->fileName() : ""; }
    bool hasNANs() const { return hasNANs_; }
    const Image& image() const { return *corrImage_; }
    const Image* normalizer() const;
    QJsonObject toJson() const;

private:
    void onCorr();
    void calcNormalizer() const;

    std::unique_ptr<const Rawfile> raw_; //!< owned by this
    mutable bool hasNANs_ {false};
    std::unique_ptr<Image> corrImage_;
    mutable std::unique_ptr<Image> normalizer_;
};

#endif // CORRSET_H
