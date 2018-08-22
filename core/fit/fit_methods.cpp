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

#include "core/fit/fit_methods.h"
#include "LevMar/LM/levmar.h"
#include "core/typ/curve.h"
#include "qcr/base/debug.h"
#include <qmath.h>

template <typename T>
T* remove_const(T const* t)
{
    return const_cast<T*>(t);
}

void FitWrapper::execFit(
    ParametricFunction& function, const Curve& curve, std::vector<double> parValue)
{
    int parCount = function.parameterCount();
    if (curve.count()<parCount) {
        //qDebug() << "not enough points for fitting";
        function.setSuccess(false);
        return;
    }

    std::vector<double> parError(parCount);
    std::vector<double> covar(parCount * parCount); // output covariance matrix
    //std::vector<double> parMin(parCount), parMax(parCount);

    // minimizer options mu, epsilon1, epsilon2, epsilon3
    double opts[] = { LM_INIT_MU, 1e-12, 1e-12, 1e-18 };
    int const maxIterations = 1000;
    double info[LM_INFO_SZ];

    function_ = &function;
    xValues_ = &curve.xs();

    DelegateCalculationDbl fitFct(this, &FitWrapper::callbackY);
    DelegateCalculationDbl Jacobian(this, &FitWrapper::callbackJacobianLM);

    dlevmar_bc_der(
        &fitFct, &Jacobian, parValue.data(), remove_const(curve.ys().data()), parCount,
        curve.count(),
        nullptr /* remove_const(parMin.data()) */,
        nullptr /* remove_const(parMax.data()) */,
        nullptr, maxIterations, opts, info, nullptr, covar.data(), nullptr);

    // pass fit results
    function.setSuccess(true);
    for (int ip=0; ip<parCount; ++ip)
        parError[ip] = sqrt(covar[ip * parCount + ip]); // the diagonal
    for (int ip=0; ip<parCount; ++ip)
        function.parameterAt(ip).setValue(parValue[ip], parError[ip]);
}

void FitWrapper::callbackY(double* parValues, double* yValues, int, int, void*)
{
    function_->f->setY(parValues, xValues_->size(), xValues_->data(), yValues);
}

void FitWrapper::callbackJacobianLM(double* parValues, double* jacobian, int, int, void*)
{
    function_->f->setDY(parValues, xValues_->size(), xValues_->data(), jacobian);
}
