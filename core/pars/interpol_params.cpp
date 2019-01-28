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
#include "core/session.h"

InterpolParams::InterpolParams()
{
    enabled.setHook    ([](bool  ){gSession->onInterpol(); });
    stepAlpha.setHook  ([](double){gSession->onInterpol(); });
    stepBeta.setHook   ([](double){gSession->onInterpol(); });
    idwRadius.setHook  ([](double){gSession->onInterpol(); });
    avgAlphaMax.setHook([](double){gSession->onInterpol(); });
    avgRadius.setHook  ([](double){gSession->onInterpol(); });
    threshold.setHook  ([](int   ){gSession->onInterpol(); });
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

bool InterpolParams::isStandardInterpolation() const
{
    if (!enabled.val())
        return false;
    for (double step: {1.0, 2.0, 5.0})
        if (stepAlpha.val()==step && stepBeta.val()==step)
            return true;
    return false;
}
