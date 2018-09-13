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
#include "core/fit/peak_function.h"
#include "core/fit/polynom.h"
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
    if (!bgFit.success)
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

RawOutcome computeRawOutcome(const Dfgram* parent, int jP)
{
    Peak& peak = gSession->peaks.at(jP);
    const Curve& peakCurve = parent->getCurveMinusBg().intersect(peak.range());
    return RawOutcome(peakCurve);
}

Fitted computePeakFit(const Dfgram* parent, int jP)
{
    Peak& peak = gSession->peaks.at(jP);
    return PeakFunction::fromFit(
        peak.functionName(), parent->getCurveMinusBg().intersect(peak.range()),
        parent->getRawOutcome(jP));
}

Curve computePeakAsCurve(const Dfgram* parent, int jP)
{
    Peak& peak = gSession->peaks.at(jP);
    const Curve& curveMinusBg = parent->getCurveMinusBg();
    const Fitted& fun = parent->getPeakFit(jP);
    if (!fun.success)
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
    , bgFit_        {&computeBgFit}
    , bgAsCurve_    {&computeBgAsCurve}
    , curveMinusBg_ {&computeCurveMinusBg}
    , rawOutcomes_ {[]()->int {return gSession->peaks.size();},
              [](const Dfgram* parent, int jP)->RawOutcome{
                  return computeRawOutcome(parent, jP); } }
    , peakFits_ {[]()->int {return gSession->peaks.size();},
              [](const Dfgram* parent, int jP)->Fitted{
                  return computePeakFit(parent, jP); } }
    , peaksAsCurve_ {[]()->int {return gSession->peaks.size();},
              [](const Dfgram* parent, int jP)->Curve{
                  return computePeakAsCurve(parent, jP); } }
{}

Dfgram::~Dfgram()
{
    gSession->allPeaks.invalidateAll();
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
    rawOutcomes_.invalidate();
    peakFits_.invalidate();
    peaksAsCurve_.invalidate();
    gSession->allPeaks.invalidateAll();
}

void Dfgram::invalidatePeakAt(int jP) const // TODO restrict to peak jP
{
    invalidatePeaks();
    gSession->allPeaks.invalidateAt(jP);
}
