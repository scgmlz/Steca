//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/raw_outcome.h
//! @brief     Defines class RawOutcome
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef RAW_OUTCOME_H
#define RAW_OUTCOME_H

#include <QtNumeric>

class Curve;

// TODO rather use PeakOutcome to report result of raw analysis;
//      this will simplify getPeak in all_infos.cpp

//! Minimal outcome of peak analysis: center, width, intensity.

class RawOutcome {
public:
    // Can only be constructed by computing it from a Curve.
    RawOutcome() {}
    RawOutcome(const RawOutcome&) = delete;
    RawOutcome(RawOutcome&&) = default;
    double getCenter() const { return center_; }
    double getFwhm() const { return fwhm_; }
    double getIntensity() const { return intensity_; }
    double center_ = Q_QNAN;
    double fwhm_ = Q_QNAN;
    double intensity_ = Q_QNAN;
};

RawOutcome analyseRawPeak(const Curve&);

#endif // RAW_OUTCOME_H
