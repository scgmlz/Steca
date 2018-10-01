//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dfgram.h
//! @brief     Defines class Dfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DFGRAM_H
#define DFGRAM_H

#include "core/typ/cached.h"
#include "core/typ/curve.h"
#include "core/fit/raw_outcome.h"
#include "core/fit/parametric_function.h"

//! A curve, and associated fit parameters.

class Dfgram {
public:
    Dfgram(Curve&& c);
    ~Dfgram();
    Dfgram(const Dfgram&) = delete;
    Dfgram(Dfgram&&) = default;

    void invalidateBg() const;
    void invalidatePeaks() const;
    void invalidatePeakAt(int) const;

    const Curve curve; // not cached here because recompute depends on context (sector vs avg)
    const Fitted& getBgFit() const { return bgFit_.get(this); }
    const Curve& getBgAsCurve() const { return bgAsCurve_.get(this); }
    const Curve& getCurveMinusBg() const { return curveMinusBg_.get(this); }
    const RawOutcome& getRawOutcome(int jP) const { return rawOutcomes_.get(this,jP).get(this); }
    const Fitted& getPeakFit(int jP) const { return peakFits_.get(this,jP).get(this); }
    const Curve& getPeakAsCurve(int jP) const { return peaksAsCurve_.get(this,jP).get(this); }

private:
    mutable Kached<Dfgram,Fitted> bgFit_;
    mutable Kached<Dfgram,Curve>   bgAsCurve_;
    mutable Kached<Dfgram,Curve>   curveMinusBg_;
    mutable SelfKachingVector<Dfgram,RawOutcome> rawOutcomes_;
    mutable SelfKachingVector<Dfgram,Fitted> peakFits_;
    mutable SelfKachingVector<Dfgram,Curve> peaksAsCurve_;
};

#endif // DFGRAM_H
