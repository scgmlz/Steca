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

InterpolParams::InterpolParams()
{
    enabled.    connectAction([](){ emit gSession->sigInterpol();});
    enabled.    connectAction([](){ emit gSession->sigInterpol();});
    stepAlpha.  connectAction([](){ emit gSession->sigInterpol();});
    stepBeta.   connectAction([](){ emit gSession->sigInterpol();});
    idwRadius.  connectAction([](){ emit gSession->sigInterpol();});
    avgAlphaMax.connectAction([](){ emit gSession->sigInterpol();});
    avgRadius.  connectAction([](){ emit gSession->sigInterpol();});
    threshold.  connectAction([](){ emit gSession->sigInterpol();});
}

void InterpolParams::fromSettings()
{
    XSettings s("interpolation parameters");
    { bool   val; s.getBool("enabled", val);    enabled.setParam(val); }
    { double val; s.getReal("step alpha", val); stepAlpha.setParam(val); }
    { double val; s.getReal("step beta", val);  stepBeta.setParam(val); }
    { double val; s.getReal("idw radius", val); idwRadius.setParam(val); }
    { double val; s.getReal("alpha max", val);  avgAlphaMax.setParam(val); }
    { double val; s.getReal("avg radius", val); avgRadius.setParam(val); }
    { int    val; s.getInt ("threshold", val);  threshold.setParam(val); }
}

void InterpolParams::toSettings() const
{
    XSettings s("interpolation parameters");
    s.setValue("enabled",    enabled.getParam());
    s.setValue("step alpha", stepAlpha.getParam());
    s.setValue("step beta",  stepBeta.getParam());
    s.setValue("idw radius", idwRadius.getParam());
    s.setValue("alpha max",  avgAlphaMax.getParam());
    s.setValue("avg radius", avgRadius.getParam());
    s.setValue("threshold",  threshold.getParam());
}

QJsonObject InterpolParams::toJson() const
{
    return {
        { "enabled",    QJsonValue(enabled.getParam()) },
        { "step alpha", QJsonValue(stepAlpha.getParam()) },
        { "step beta",  QJsonValue(stepBeta.getParam()) },
        { "idw radius", QJsonValue(idwRadius.getParam()) },
        { "alpha max",  QJsonValue(avgAlphaMax.getParam()) },
        { "avg radius", QJsonValue(avgRadius.getParam()) },
        { "threshold",  QJsonValue(threshold.getParam()) },
    };
}

void InterpolParams::fromJson(const JsonObj& obj)
{
    enabled    .setParam(obj.loadBool("enabled"));
    stepAlpha  .setParam(obj.loadInt("step alpha"));
    stepBeta   .setParam(obj.loadInt("step beta"));
    idwRadius  .setParam(obj.loadInt("idw radius"));
    avgAlphaMax.setParam(obj.loadInt("alpha max"));
    avgRadius  .setParam(obj.loadInt("avg radius"));
    threshold  .setParam(obj.loadInt("threshold"));
}
/*
void InterpolParams::setStepAlpha(double val)
{
    stepAlpha_ = qMax(0., qMin(val, 90.));
    EMITS("InterpolParams::setStepAlpha", gSession->sigInterpol());
}

void InterpolParams::setStepBeta(double val)
{
    stepBeta_ = qMax(0., qMin(val, 360.));
    EMITS("InterpolParams::setStepBeta", gSession->sigInterpol());
}

void InterpolParams::setIdwRadius(double val)
{
    if (!qIsNaN(val))
        val = qMax(0., val);
    idwRadius_ = val;
    EMITS("InterpolParams::setIdwRadiu", gSession->sigInterpol());
}

void InterpolParams::setAvgAlphaMax(double val)
{
    avgAlphaMax_ = qMax(0., qMin(val, 90.));
    EMITS("InterpolParams::setAvgAlphaMax", gSession->sigInterpol());
}

void InterpolParams::setAvgRadius(double val)
{
    avgRadius_ = qMax(0., val);
    EMITS("InterpolParams::setAvgRadius", gSession->sigInterpol());
}

void InterpolParams::setThreshold(int val)
{
    threshold_ = qMax(0, qMin(val, 1)); // TODO check
    EMITS("InterpolParams::setThreshold", gSession->sigInterpol());
}
*/
