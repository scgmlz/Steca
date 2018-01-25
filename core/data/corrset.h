// ************************************************************************** //
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
// ************************************************************************** //

#ifndef CORRSET_H
#define CORRSET_H

#include "core/data/rawfile.h"
#include <QSharedPointer> // no auto rm

//! A correction dataset, consisting of one RawFile and associated settings.

//! Note that "correction", as used throughout Steca, rather means "calibration" or "normalization".

class Corrset final {
public:

    // Modifying methods:
    void clear();
    void removeFile();
    void loadFile(rcstr filePath) THROWS;
    void tryEnable(bool on);
    void clearIntens() { intensCorr_.clear(); } // lazy

    // Lookup methods
    const Rawfile& raw() const { return *raw_; }
    bool hasFile() const { return !raw_.isNull(); }
    bool isEnabled() const { return enabled_; }
    bool hasNANs() const { return hasNANs_; }
    shp_Image corrImage() const { return corrImage_; }
    const Image* intensCorr() const;

private:
    void onCorr();
    void calcIntensCorr() const;

    QSharedPointer<const Rawfile> raw_ {nullptr}; //!< owned by this
    bool enabled_ {true};
    mutable bool hasNANs_ {false};
    shp_Image corrImage_;
    mutable Image intensCorr_;
};

#endif // CORRSET_H
