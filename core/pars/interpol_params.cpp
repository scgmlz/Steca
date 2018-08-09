//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/pars/interpol_params.cpp
//! @brief     Implements InterpolParams
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/pars/interpol_params.h"
#include "core/aux/settings.h"
#include "core/session.h"

InterpolParams::InterpolParams()
{
    // from settings:
    XSettings s("interpolation parameters");
    { bool   val; s.getBool("enabled", val);    enabled.setVal(val); }
    { double val; s.getReal("step alpha", val); stepAlpha.setVal(val); }
    { double val; s.getReal("step beta", val);  stepBeta.setVal(val); }
    { double val; s.getReal("idw radius", val); idwRadius.setVal(val); }
    { double val; s.getReal("alpha max", val);  avgAlphaMax.setVal(val); }
    { double val; s.getReal("avg radius", val); avgRadius.setVal(val); }
    { int    val; s.getInt ("threshold", val);  threshold.setVal(val); }

    enabled.setHook    ([](bool  ){gSession->onInterpol(); });
    stepAlpha.setHook  ([](double){gSession->onInterpol(); });
    stepBeta.setHook   ([](double){gSession->onInterpol(); });
    idwRadius.setHook  ([](double){gSession->onInterpol(); });
    avgAlphaMax.setHook([](double){gSession->onInterpol(); });
    avgRadius.setHook  ([](double){gSession->onInterpol(); });
    threshold.setHook  ([](int   ){gSession->onInterpol(); });
/* TODO do we want to duplicate these constraints (the are also imposed in the GUI)
void InterpolParams::setStepAlpha(double val) {
    stepAlpha_ = qMax(0., qMin(val, 90.)); }
void InterpolParams::setStepBeta(double val) {
    stepBeta_ = qMax(0., qMin(val, 360.)); }
void InterpolParams::setIdwRadius(double val) {
    if (!qIsNaN(val))
        val = qMax(0., val);
    idwRadius_ = val; }
void InterpolParams::setAvgAlphaMax(double val) {
    avgAlphaMax_ = qMax(0., qMin(val, 90.)); }
void InterpolParams::setAvgRadius(double val) {
    avgRadius_ = qMax(0., val); }
void InterpolParams::setThreshold(int val) {
    threshold_ = qMax(0, qMin(val, 1)); // TODO check }
*/
}

InterpolParams::~InterpolParams()
{
    // to settings:
    XSettings s("interpolation parameters");
    s.setValue("enabled",    enabled.val());
    s.setValue("step alpha", stepAlpha.val());
    s.setValue("step beta",  stepBeta.val());
    s.setValue("idw radius", idwRadius.val());
    s.setValue("alpha max",  avgAlphaMax.val());
    s.setValue("avg radius", avgRadius.val());
    s.setValue("threshold",  threshold.val());
}

QJsonObject InterpolParams::toJson() const
{
    return {
        { "enabled",    QJsonValue(enabled.val()) },
        { "step alpha", QJsonValue(stepAlpha.val()) },
        { "step beta",  QJsonValue(stepBeta.val()) },
        { "idw radius", QJsonValue(idwRadius.val()) },
        { "alpha max",  QJsonValue(avgAlphaMax.val()) },
        { "avg radius", QJsonValue(avgRadius.val()) },
        { "threshold",  QJsonValue(threshold.val()) },
    };
}

void InterpolParams::fromJson(const JsonObj& obj)
{
    enabled    .setVal(obj.loadBool("enabled"));
    stepAlpha  .setVal(obj.loadInt("step alpha"));
    stepBeta   .setVal(obj.loadInt("step beta"));
    idwRadius  .setVal(obj.loadInt("idw radius"));
    avgAlphaMax.setVal(obj.loadInt("alpha max"));
    avgRadius  .setVal(obj.loadInt("avg radius"));
    threshold  .setVal(obj.loadInt("threshold"));
}
