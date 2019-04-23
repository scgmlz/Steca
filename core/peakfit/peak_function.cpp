//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/peakfit/peak_function.cpp
//! @brief     Implements class PeakFunction; defines and implements its subclasses
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/base/angles.h"
#include "core/peakfit/peak_function.h"
#include "core/fitengine/fit_wrapper.h"
#include "core/peakfit/fit_models.h"
#include "core/typ/mapped.h"
// #include "qcr/base/debug.h"

Mapped PeakFunction::outcome(const Fitted& F) const
{
    if (!F.success())
        return {};
    Mapped ret;
    ret.set("center", deg{F.parValAt(0)});
    ret.set("sigma_center", deg{F.parErrAt(0)});
    ret.set("fwhm", F.parValAt(1));
    ret.set("sigma_fwhm", F.parErrAt(1));
    ret.set("intensity", F.parValAt(2));
    ret.set("sigma_intensity", F.parErrAt(2));
    return ret;
}

//! Fits given `curve` with model given by `name` and with starting values `rawOutcome`.

Fitted PeakFunction::fromFit(const QString& name, const Curve& curve, const Mapped& rawOutcome)
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
    startParams[0] = double(rawOutcome.get<deg>("center"));
    startParams[1] = rawOutcome.get<double>("fwhm");
    startParams[2] = rawOutcome.get<double>("intensity");
    return FitWrapper().execFit(f, curve, startParams, onlyPositiveParams);
}
