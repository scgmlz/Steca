//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/peak_fitpar.h
//! @brief     Defines class PeakFitpar
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_FITPAR_H
#define PEAK_FITPAR_H

#include "core/typ/range.h"

//! Fit parameters (range, function type) for one Bragg peak.

//! These are the same for all diffractograms.

class PeakFitpar {
public:
    PeakFitpar(const Range& range, const QString& functionName);

    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void setOne(double val, bool namelyMax); //!< sets either min or max

    const Range& range() const { return range_; }
    QString functionName() const { return functionName_; }
    bool isRaw() const { return functionName_=="Raw"; }
    JsonObj toJson() const;

    static PeakFitpar fromJson(const JsonObj&);
    static const QStringList keys;

private:
    Range range_;
    QString functionName_;
};

#endif // PEAK_FITPAR_H
