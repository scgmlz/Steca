//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/interpol_params.cpp
//! @brief     Implements InterpolParams
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/session.h"
#include "core/def/settings.h"

void InterpolParams::fromSettings()
{
    XSettings s("interpolation parameters");
    s.getBool("enabled", enabled_);
    s.getReal("step alpha", stepAlpha_);
    s.getReal("step beta", stepBeta_);
    s.getReal("idw radius", idwRadius_);
    s.getReal("alpha max", avgAlphaMax_);
    s.getReal("avg radius", avgRadius_);
    s.getInt ("threshold", threshold_);
}

void InterpolParams::toSettings() const
{
    XSettings s("interpolation parameters");
    s.setValue("enabled", enabled_);
    s.setValue("step alpha", stepAlpha_);
    s.setValue("step beta", stepBeta_);
    s.setValue("idw radius", idwRadius_);
    s.setValue("alpha max", avgAlphaMax_);
    s.setValue("avg radius", avgRadius_);
    s.setValue("threshold", threshold_);
}

QJsonObject InterpolParams::toJson() const
{
    return {
        { "enabled", QJsonValue(enabled_) },
        { "step alpha", QJsonValue(stepAlpha_) },
        { "step beta", QJsonValue(stepBeta_) },
        { "idw radius", QJsonValue(idwRadius_) },
        { "alpha max", QJsonValue(avgAlphaMax_) },
        { "avg radius", QJsonValue(avgRadius_) },
        { "threshold", QJsonValue(threshold_) },
    };
}

void InterpolParams::fromJson(const JsonObj& obj)
{
    setEnabled(obj.loadBool("enabled"));
    setStepAlpha(obj.loadInt("step alpha"));
    setStepBeta(obj.loadInt("step beta"));
    setIdwRadius(obj.loadInt("idw radius"));
    setAvgAlphaMax(obj.loadInt("alpha max"));
    setAvgRadius(obj.loadInt("avg radius"));
    setThreshold(obj.loadInt("threshold"));
}

void InterpolParams::setEnabled(bool val)
{
    enabled_ = val;
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setStepAlpha(double val)
{
    stepAlpha_ = qMax(0., qMin(val, 90.));
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setStepBeta(double val)
{
    stepBeta_ = qMax(0., qMin(val, 360.));
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setIdwRadius(double val)
{
    if (!qIsNaN(val))
        val = qMax(0., val);
    idwRadius_ = val;
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setAvgAlphaMax(double val)
{
    avgAlphaMax_ = qMax(0., qMin(val, 90.));
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setAvgRadius(double val)
{
    avgRadius_ = qMax(0., val);
    EMIT(gSession->sigInterpol());
}

void InterpolParams::setThreshold(int val)
{
    threshold_ = qMax(0, qMin(val, 1)); // TODO check
    EMIT(gSession->sigInterpol());
}
