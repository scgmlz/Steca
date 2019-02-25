//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/onepeak_settings.h
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
#include <QStringList>

//! Settings (range, function type) for analysing one Bragg peak.

//! These are the same for all diffractograms.

class OnePeakSettings {
public:
    OnePeakSettings() = delete;
    OnePeakSettings(const OnePeakSettings&) = default; // needed by sort
    OnePeakSettings(const Range& range, const QString& functionName);

    void setPeakFunction(const QString&);
    void setRange(const Range&);
    void setMin(double val);
    void setMax(double val);

    const Range& range() const { return range_; }
    const QString& functionName() const { return functionName_; }
    const QStringList& fitParAsciiNames() const { return fitParAsciiNames_; }
    const QStringList& fitParNiceNames() const { return fitParNiceNames_; }
    bool isRaw() const { return functionName_=="Raw"; }
    JsonObj toJson() const;

    static OnePeakSettings fromJson(const JsonObj&);
    static const QStringList functionNames;

private:
    Range range_;
    QString functionName_;
    QStringList fitParAsciiNames_;
    QStringList fitParNiceNames_;
};

#endif // PEAK_FITPAR_H
