// ************************************************************************** //
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
// ************************************************************************** //

#include "core/fit/fit_methods.h"
#include "core/def/idiomatic_for.h"
#include "core/typ/curve.h"
#include "LM/levmar.h"
#include <qmath.h>

void FitWrapper::fit(Function& function, Curve const& curve) {
    if (curve.isEmpty())
        return;

    function_ = &function;
    xValues_ = curve.xs().data();

    // prepare data in a debug::ensured format
    uint parCount = function_->parameterCount();
    qreal_vec parValue(parCount), parMin(parCount), parMax(parCount), parError(parCount);

    for_i (parCount) {
        auto par = function_->parameterAt(i);
        debug::ensure(qIsFinite(par.value())); // TODO if not so, return false ?
        parValue[i] = par.value();
        parMin[i] = par.valueRange().min;
        parMax[i] = par.valueRange().max;
    }

    fit_exec(
        parValue.data(), parMin.data(), parMax.data(), parError.data(), parCount, curve.ys().data(),
        curve.count());

    // read data
    for_i (parCount)
        function_->parameterAt(i).setValue(parValue[i], parError[i]);
}

template <typename T> T* remove_const(T const* t) {
    return const_cast<T*>(t);
}

void FitWrapper::fit_exec(
    qreal* params, // IO initial parameter estimates -> estimated solution
    qreal const* paramsLimitMin, // I
    qreal const* paramsLimitMax, // I
    qreal* paramsError, // O
    uint paramsCount, // I
    qreal const* yValues, // I
    uint dataPointsCount) // I
{
    DelegateCalculationDbl function(this, &FitWrapper::callbackY);
    DelegateCalculationDbl functionJacobian(this, &FitWrapper::callbackJacobianLM);

    // minim. options mu, epsilon1, epsilon2, epsilon3
    double opts[] = { LM_INIT_MU, 1e-12, 1e-12, 1e-18 };

    // information regarding the minimization
    double info[LM_INFO_SZ];

    // output covariance matrix
    qreal_vec covar(paramsCount * paramsCount);

    uint const maxIterations = 1000;

    dlevmar_bc_der(
        &function, &functionJacobian, params, remove_const(yValues), to_i(paramsCount),
        to_i(dataPointsCount), remove_const(paramsLimitMin), remove_const(paramsLimitMax), NULL,
        maxIterations, opts, info, NULL, covar.data(), NULL);

    for_i (paramsCount)
        paramsError[i] = sqrt(covar[i * paramsCount + i]); // the diagonal
}

void FitWrapper::callbackY(qreal* parValues, qreal* yValues, int /*parCount*/, int xLength, void*) {
    for_i (xLength)
        yValues[i] = function_->y(xValues_[i], parValues);
}

void FitWrapper::callbackJacobianLM(
    qreal* parValues, qreal* jacobian, int parameterLength, int xLength, void*) {
    for_int (ix, xLength) {
        for_int (ip, parameterLength) {
            *jacobian++ = function_->dy(xValues_[ix], to_u(ip), parValues);
        }
    }
}
