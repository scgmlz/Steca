//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_function.cpp
//! @brief     Implements class PeakFunction; defines and implements its subclasses
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fit/peak_function.h"
#include "core/typ/curve.h"
#include "core/fit/fit_methods.h"
#include "core/fit/fit_models.h"
#include "core/fit/outcome.h"
#include "core/fit/raw_outcome.h"
#include "qcr/base/debug.h" // ASSERT
#include "LevMar/LM/levmar.h"
// waiting for activation #include "lmfit/lib/lmmin.h"
#include <iostream>

PeakOutcome PeakFunction::outcome(const Fitted& F) const
{
    return {
        {F.parValAt(0), F.parErrAt(0)},
        {F.parValAt(1), F.parErrAt(1)},
        {F.parValAt(2), F.parErrAt(2)} };
}

Fitted PeakFunction::fromFit(const QString& name, const Curve& curve, const RawOutcome& rawOutcome)
{
    const PeakFunction* f;
    bool onlyPositiveParams = false;
    if        (name=="Raw") {
        return {};
    } else if (name=="Gaussian") {
        f = new Gaussian;
    } else if (name=="Lorentzian") {
        f = new Lorentzian;
    } else if (name=="Voigt") {
        f = new Voigt;
        onlyPositiveParams = true;
    } else
        qFatal("Impossible case");
    std::vector<double> startParams(f->nPar(), 1.);
    startParams[0] = rawOutcome.getCenter();
    startParams[1] = rawOutcome.getFwhm();
    startParams[2] = rawOutcome.getIntensity();
    return FitWrapper().execFit(f, curve, startParams, onlyPositiveParams);
}
