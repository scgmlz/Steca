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
#include "core/session.h"
#include "core/fit/fit_fun.h"

Polynom computeBgFit(const Dfgram* parent)
{
    return Polynom::fromFit(
        gSession->baseline.polynomDegree.val(), parent->curve, gSession->baseline.ranges);
}

Curve computeBgAsCurve(const Dfgram* parent)
{
    if (!gSession->baseline.ranges.count())
        return {};
    Curve ret;
    const Polynom& bgFit = parent->getBgFit();
    for (int i=0; i<parent->curve.count(); ++i) {
        double x = parent->curve.x(i);
        ret.append(x, bgFit.y(x));
    }
    return ret;
}

Curve computeCurveMinusBg(const Dfgram* parent)
{
    if (!gSession->baseline.ranges.count())
        return parent->curve; // no bg defined
    Curve ret;
    const Curve& bg = parent->getBgAsCurve();
    for (int i=0; i<parent->curve.count(); ++i)
        ret.append(parent->curve.x(i), parent->curve.y(i)-bg.y(i));
    return ret;
}

Dfgram::Dfgram(Curve&& c)
    : curve(std::move(c))
{
    QObject::connect(gSession, &Session::sigBaseline, [this](){bgFit.invalidate();});
}
