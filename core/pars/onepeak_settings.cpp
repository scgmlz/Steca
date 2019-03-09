//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/onepeak_settings.cpp
//! @brief     Implements class PeakFitpar
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/onepeak_settings.h"
#include "core/session.h"
#include "core/base/exception.h"
#include "qcr/base/debug.h"

const QStringList OnePeakSettings::functionNames = { "Raw", "Gaussian", "Lorentzian", "Voigt" };

OnePeakSettings::OnePeakSettings(const Range& r, const QString& functionName)
    : range_{r}
    , functionName_ {functionName}
{
    onFunction();
}

void OnePeakSettings::setRange(const Range& r)
{
    range_ = r;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::setMin(double val)
{
    range_.setMin(val);
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::setMax(double val)
{
    range_.setMax(val);
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::setPeakFunction(const QString& name)
{
    functionName_ = name;
    onFunction();
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::onFunction()
{
    fitParAsciiNames_ = QStringList{ "intensity", "sigma_intensity",
                                     "center", "sigma_center",
                                     "fwhm", "sigma_fwhm" };
    fitParNiceNames_ = QStringList{ "intensity", "σintensity",
                                    "2θ", "σ2θ",
                                    "fwhm", "σfwhm" };
// TODO URGENT OUTCOMMENT FOR PRODUCTION
    if (functionName_=="Voigt") {
        fitParAsciiNames_ << "Gamma/Sigma" << "sigma_Gamma/Sigma";
        fitParNiceNames_  << "Γ/Σ" << "σ(Γ/Σ)";
    }
}

JsonObj OnePeakSettings::toJson() const
{
    QJsonObject ret;
    ret.insert("range", range_.toJson() );
    ret.insert("type", functionName());
    return ret;
}

OnePeakSettings OnePeakSettings::fromJson(const JsonObj& obj)
{
    QString type = obj.loadString("type");
    if (!functionNames.contains(type)) // validate peak fit function, so we dont get any surprises later.
        THROW(QString("'") + type + "' is not a valid fit function!");
    return {obj.loadRange("range"), type};
}
