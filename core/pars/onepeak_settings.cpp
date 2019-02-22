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
    : range_(r)
    , functionName_(functionName)
{}

void OnePeakSettings::setRange(const Range& r)
{
    range_ = r;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::setOne(double val, bool namelyMax)
{
    range_.setOne(val, namelyMax);
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void OnePeakSettings::setPeakFunction(const QString& name)
{
    functionName_ = name;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
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
