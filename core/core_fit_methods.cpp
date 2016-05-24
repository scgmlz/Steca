// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_methods.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_fit_methods.h"

#include "LevMar/levmar.h"
#include "types/core_type_curve.h"
#include <qmath.h>

namespace core {
namespace fit {
//------------------------------------------------------------------------------

FittingMethod::FittingMethod() {
}

void FittingMethod::fit(Function& function, rcCurve curve) {
  if (curve.isEmpty()) return;

  function_ = &function;
  xValues_  = curve.xs().data();

  // prepare data in a required format
  uint      parCount = function_->parameterCount();
  qreal_vec parValue(parCount), parMin(parCount), parMax(parCount),
            parError(parCount);

  for_i (parCount) {
    auto par = function_->parameterAt(i);
    auto rge = par.valueRange();

    EXPECT(qIsFinite(par.value())) // TODO if not so, return false
    parValue[i] = par.value();
    parMin[i]   = rge.min;
    parMax[i]   = rge.max;
  }

  approximate(parValue.data(), parMin.data(), parMax.data(),
              parError.data(), parCount, curve.ys().data(), curve.count());

  // read data
  for_i (parCount)
    function_->parameterAt(i).setValue(parValue[i], parError[i]);
}

void FittingMethod::callbackY(qreal* parValues, qreal* yValues,
                              int /*parCount*/, int    xLength, void*) {
  for_i (xLength)
    yValues[i] = function_->y(xValues_[i], parValues);
}

//------------------------------------------------------------------------------

FittingLinearLeastSquare::FittingLinearLeastSquare() {}

void FittingLinearLeastSquare::approximate(
    qreal* params,  // IO initial parameter estimates -> estimated solution
    qreal const* paramsLimitMin,   // I
    qreal const* paramsLimitMax,   // I
    qreal*       paramsError,      // O
    uint         paramsCount,      // I
    qreal const* yValues,          // I
    uint         dataPointsCount)  // I
{
  DelegateCalculationDbl function(this, &thisClass::callbackY);

  // information regarding the minimization
  double info[LM_INFO_SZ];

  // output covariance matrix
  qreal_vec covar(paramsCount * paramsCount);

  uint const maxIterations = 1000;

  dlevmar_bc_dif(&function, params, (qreal*)yValues, paramsCount,
                 dataPointsCount, (qreal*)paramsLimitMin,
                 (qreal*)paramsLimitMax, NULL, maxIterations, NULL, info, NULL,
                 covar.data(), NULL);

  for_i (paramsCount)
    paramsError[i] = sqrt(covar[i * paramsCount + i]);  // the diagonal
}

//------------------------------------------------------------------------------

FittingLevenbergMarquardt::FittingLevenbergMarquardt() {}

void FittingLevenbergMarquardt::approximate(
    qreal* params,  // IO initial parameter estimates -> estimated solution
    qreal const* paramsLimitMin,   // I
    qreal const* paramsLimitMax,   // I
    qreal*       paramsError,      // O
    uint         paramsCount,      // I
    qreal const* yValues,          // I
    uint         dataPointsCount)  // I
{
  DelegateCalculationDbl function(this, &thisClass::callbackY);
  DelegateCalculationDbl functionJacobian(this, &thisClass::callbackJacobianLM);

  // minim. options mu, epsilon1, epsilon2, epsilon3
  double opts[LM_OPTS_SZ];
  opts[0] = LM_INIT_MU;
  opts[1] = 1e-12;
  opts[2] = 1e-12;
  opts[3] = 1e-18;

  // information regarding the minimization
  double info[LM_INFO_SZ];

  // output covariance matrix
  qreal_vec covar(paramsCount * paramsCount);

  uint const maxIterations = 1000;

  dlevmar_bc_der(&function, &functionJacobian, params, (qreal*)yValues,
                 paramsCount, dataPointsCount, (qreal*)paramsLimitMin,
                 (qreal*)paramsLimitMax, NULL, maxIterations, opts, info, NULL,
                 covar.data(), NULL);

  for_i (paramsCount)
    paramsError[i] = sqrt(covar[i * paramsCount + i]);  // the diagonal
}

void FittingLevenbergMarquardt::callbackJacobianLM(qreal* parValues,
                                                   qreal* jacobian,
                                                   int    parameterLength,
                                                   int    xLength, void*)
{
  for_int (ix, xLength) {
    for_int (ip, parameterLength) {
      *jacobian++ = function_->dy(xValues_[ix], ip, parValues);
    }
  }
}

//------------------------------------------------------------------------------
}}
// eof
