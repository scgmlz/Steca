//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/peak_fitpar.cpp
//! @brief     Implements classes Peak and Peaks
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/peak_fitpar.h"
#include "core/session.h"
#include "core/base/exception.h"
#include "qcr/base/debug.h"

const QStringList PeakFitpar::keys = { "Raw", "Gaussian", "Lorentzian", "Voigt" };

PeakFitpar::PeakFitpar(const Range& r, const QString& functionName)
    : range_(r)
    , functionName_(functionName)
{}

void PeakFitpar::setRange(const Range& r)
{
    range_ = r;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void PeakFitpar::setOne(double val, bool namelyMax)
{
    range_.setOne(val, namelyMax);
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

void PeakFitpar::setPeakFunction(const QString& name)
{
    functionName_ = name;
    gSession->onPeaks(); // TODO restrict to PeakAt(index())
}

JsonObj PeakFitpar::toJson() const
{
    QJsonObject ret;
    ret.insert("range", range_.toJson() );
    ret.insert("type", functionName());
    return ret;
}

PeakFitpar PeakFitpar::fromJson(const JsonObj& obj)
{
    QString type = obj.loadString("type");
    if (!keys.contains(type)) // validate peak fit function, so we dont get any surprises later.
        THROW(QString("'") + type + "' is not a valid fit function!");
    return {obj.loadRange("range"), type};
}
