//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dfgram.cpp
//! @brief     Implements class Dfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/dfgram.h"
#include "core/peakfit/peak_function.h"
#include "core/peakfit/polynom.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

namespace {

Fitted computeBgFit(const Dfgram* parent)
{
    return Polynom::fromFit(
        gSession->baseline.polynomDegree.val(), parent->curve, gSession->baseline.ranges);
}

Curve computeBgAsCurve(const Dfgram* parent)
{
    const Fitted& bgFit = parent->getBgFit();
    if (!bgFit.success())
        return {};
    Curve ret;
    for (int i=0; i<parent->curve.size(); ++i) {
        double x = parent->curve.x(i);
        ret.append(x, bgFit.y(x));
    }
    return ret;
}

Curve computeCurveMinusBg(const Dfgram* parent)
{
    const Curve& bg = parent->getBgAsCurve();
    if (!bg.size())
        return parent->curve; // no bg defined
    Curve ret;
    for (int i=0; i<parent->curve.size(); ++i)
        ret.append(parent->curve.x(i), parent->curve.y(i)-bg.y(i));
    return ret;
}

RawOutcome computeRawOutcome(int jP, const Dfgram* parent)
{
    OnePeakSettings& peak = gSession->peaksSettings.at(jP);
    const Curve peakCurve = parent->getCurveMinusBg().intersect(peak.range());
    return RawOutcome(peakCurve);
}

Fitted computePeakFit(int jP, const Dfgram* parent)
{
    OnePeakSettings& peak = gSession->peaksSettings.at(jP);
    return PeakFunction::fromFit(
        peak.functionName(), parent->getCurveMinusBg().intersect(peak.range()),
        parent->getRawOutcome(jP));
}

Curve computePeakAsCurve(int jP, const Dfgram* parent)
{
    OnePeakSettings& peak = gSession->peaksSettings.at(jP);
    const Curve& curveMinusBg = parent->getCurveMinusBg();
    const Fitted& fun = parent->getPeakFit(jP);
    if (!fun.success())
        return {};
    const Range& rge = peak.range();
    Curve ret;
    for (int i=0; i<curveMinusBg.size(); ++i) {
        double x = curveMinusBg.x(i);
        if (rge.contains(x))
            ret.append(x, fun.y(x));
    }
    return ret;
}

} // namespace

Dfgram::Dfgram(Curve&& c)
    : curve         {std::move(c)}
    , bgFit_{&computeBgFit}
    , bgAsCurve_    {&computeBgAsCurve}
    , curveMinusBg_ {&computeCurveMinusBg}
    , rawOutcomes_ {[]()->int {return gSession->peaksSettings.size();},
              [](int jP, const Dfgram* parent)->RawOutcome{
                  return computeRawOutcome(jP, parent); } }
    , peakFits_ {[]()->int {return gSession->peaksSettings.size();},
              [](int jP, const Dfgram* parent)->Fitted{
                  return computePeakFit(jP, parent); } }
    , peaksAsCurve_ {[]()->int {return gSession->peaksSettings.size();},
              [](int jP, const Dfgram* parent)->Curve{
                  return computePeakAsCurve(jP, parent); } }
{}

Dfgram::~Dfgram()
{
    gSession->peaksOutcome.invalidateAll();
}

void Dfgram::invalidateBg() const
{
    bgFit_.invalidate();
    bgAsCurve_.invalidate();
    curveMinusBg_.invalidate();
    invalidatePeaks();
}

void Dfgram::invalidatePeaks() const
{
    rawOutcomes_.clear_vector();
    peakFits_.clear_vector();
    peaksAsCurve_.clear_vector();
    gSession->peaksOutcome.invalidateAll();
}

void Dfgram::invalidatePeakAt(int jP) const // TODO restrict to peak jP
{
    invalidatePeaks();
    gSession->peaksOutcome.invalidateAt(jP);
}
