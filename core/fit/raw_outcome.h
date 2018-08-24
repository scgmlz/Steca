//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/raw_outcome.h
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

class Curve;

// TODO rather use PeakOutcome to report result of raw analysis;
//      this will simplify getPeak in all_infos.cpp

//! Minimal outcome of peak analysis: center, width, intensity.

class RawOutcome {
public:
    RawOutcome(const Curve&);
    const double getCenter() const { return center_; }
    const double getFwhm() const { return fwhm_; }
    const double getIntensity() const { return intensity_; }
private:
    double center_;
    double fwhm_;
    double intensity_;
};

#endif // RAW_OUTCOME_H
