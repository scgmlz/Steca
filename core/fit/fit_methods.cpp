//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_methods.cpp
//! @brief     Implements class FitWrapper
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fit_methods.h"
#include "LevMar/LM/levmar.h"
#include "core/typ/curve.h"
#include "qcr/base/debug.h"
#include <qmath.h>

void FitWrapper::execFit(ParametricFunction& function, const Curve& curve)
{
    int parCount = function.parameterCount();
    if (curve.count()<parCount) {
        //qDebug() << "not enough points for fitting";
        function.setSuccess(false);
        return;
    }

    std::vector<double> parValue(parCount), parMin(parCount), parMax(parCount), parError(parCount);
    for (int ip=0; ip<parCount; ++ip) {
        const FitParameter& par = function.parameterAt(ip);
        parValue[ip] = qIsFinite(par.value()) ? par.value() : 0.;
        parMin[ip] = par.range().min;
        parMax[ip] = par.range().max;
    }

    function_ = &function;
    xValues_ = &curve.xs();
    callFit(parValue.data(), parMin.data(), parMax.data(), parError.data(), parCount,
            curve.ys().data(), curve.count());

    // pass fit results
    function.setSuccess(true);
    for (int ip=0; ip<parCount; ++ip)
        function.parameterAt(ip).setValue(parValue[ip], parError[ip]);
}

template <typename T>
T* remove_const(T const* t)
{
    return const_cast<T*>(t);
}

void FitWrapper::callFit(
    double* params, // IO initial parameter estimates -> estimated solution
    double const* paramsLimitMin, // I
    double const* paramsLimitMax, // I
    double* paramsError, // O
    int paramsCount, // I
    double const* yValues, // I
    int dataPointsCount) // I
{
    DelegateCalculationDbl fitFct(this, &FitWrapper::callbackY);
    DelegateCalculationDbl Jacobian(this, &FitWrapper::callbackJacobianLM);

    // minim. options mu, epsilon1, epsilon2, epsilon3
    double opts[] = { LM_INIT_MU, 1e-12, 1e-12, 1e-18 };

    // information regarding the minimization
    double info[LM_INFO_SZ];

    // output covariance matrix
    std::vector<double> covar(paramsCount * paramsCount);

    int const maxIterations = 1000;

    dlevmar_bc_der(
        &fitFct, &Jacobian, params, remove_const(yValues), paramsCount,
        dataPointsCount, remove_const(paramsLimitMin), remove_const(paramsLimitMax), NULL,
        maxIterations, opts, info, NULL, covar.data(), NULL);

    for (int ip=0; ip<paramsCount; ++ip)
        paramsError[ip] = sqrt(covar[ip * paramsCount + ip]); // the diagonal
}

void FitWrapper::callbackY(double* parValues, double* yValues, int, int, void*)
{
    for (int i=0 ; i<xValues_->size(); ++i)
        yValues[i] = function_->y((*xValues_)[i], parValues);
}

void FitWrapper::callbackJacobianLM(double* parValues, double* jacobian, int, int, void*)
{
    for (int i=0; i<xValues_->size(); ++i)
        for (int ip=0; ip<function_->parameterCount(); ++ip)
            *jacobian++ = function_->dy((*xValues_)[i], ip, parValues);
}
