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

#include "types/core_type_curve.h"
#include "LevMar/levmar.h"
#include <QtMath>

namespace core { namespace fit {
//------------------------------------------------------------------------------
// STeCa (1) code, refactored

FittingMethod::FittingMethod() {
}

FittingMethod::~FittingMethod() {
}

bool FittingMethod::fitWithoutChecks(Function& function, core::Curve const& curve) {
  return fit(function, curve, false);
}

bool FittingMethod::fit(Function& function_, core::Curve const& curve, bool withChecks) {
  if (curve.isEmpty()) return false;

  function = &function_;
  xValues  = curve.getXs().data();

  // prepare data in a required format
  uint parCount = function->parameterCount();
  qreal_vec parValue(parCount), parMin(parCount), parMax(parCount), parError(parCount);

  for_i (parCount) {
    auto par = function->parameterAt(i);
    auto rge = par.valueRange();
    parValue[i] = par.value();
    parMin[i]   = rge.min;
    parMax[i]   = rge.max;
  }

  if (!approximate(parValue.data(),
                   parMin.data(), parMax.data(), parError.data(), parCount,
                   curve.getYs().data(), curve.count()))
    return false;

  // read data
  for_i (parCount) {
    if (!function->parameterAt(i).setValue(parValue[i], parError[i], !withChecks))
      return false;
  }

  return true;
}

void FittingMethod::__functionY(qreal* parameterValues, qreal* yValues, int /*parameterLength*/, int xLength, void*) {
  for_i (xLength)
    yValues[i] = function->y(xValues[i], parameterValues);
}

//------------------------------------------------------------------------------

FittingLinearLeastSquare::FittingLinearLeastSquare() {
}

bool FittingLinearLeastSquare::approximate(
  qreal*        parameter,          // IO initial parameter estimates -> estimated solution
  qreal const*  parameterLimitMin,  // I
  qreal const*  parameterLimitMax,  // I
  qreal*        parameterError,     // O
  uint          numberOfParameter,  // I
  qreal const*  yValues,            // I
  uint          numberOfDataPoints) // I
{
  DelegateCalculationDbl function(this, &thisClass::__functionY);

  // information regarding the minimization
  double info[LM_INFO_SZ];

  // output covariance matrix
  QVector<qreal> covar(numberOfParameter*numberOfParameter);

  uint const maxIterations = 1000;

  dlevmar_bc_dif(
    &function,
    parameter, (qreal*)yValues, numberOfParameter, numberOfDataPoints,
    (qreal*)parameterLimitMin, (qreal*)parameterLimitMax,
    NULL, maxIterations, NULL, info, NULL, covar.data(),
    NULL);

  for (uint i=0; i<numberOfParameter; i++)
    parameterError[i] = sqrt(covar[i*numberOfParameter + i]); // diagonal

  return true;
}

//------------------------------------------------------------------------------

FittingLevenbergMarquardt::FittingLevenbergMarquardt() {
}

bool FittingLevenbergMarquardt::approximate(
  qreal*        parameter,          // IO initial parameter estimates -> estimated solution
  qreal const*  parameterLimitMin,  // I
  qreal const*  parameterLimitMax,  // I
  qreal*        parameterError,     // O
  uint          numberOfParameter,  // I
  qreal const*  yValues,            // I
  uint          numberOfDataPoints) // I
{
  DelegateCalculationDbl function(this, &thisClass::__functionY);
  // Function to fill the Jacobian Matrix
  DelegateCalculationDbl functionJacobian(this, &thisClass::__functionJacobianLM);

  // minim. options mu, epsilon1, epsilon2, epsilon3
  double opts[LM_OPTS_SZ];
  opts[0] = LM_INIT_MU;
  opts[1] = 1e-12;
  opts[2] = 1e-12;
  opts[3] = 1e-18;

  // information regarding the minimization
  double info[LM_INFO_SZ];

  // output covariance matrix
  QVector<qreal> covar(numberOfParameter*numberOfParameter);

  uint const maxIterations = 1000;

  dlevmar_bc_der(
    &function, &functionJacobian,
    parameter, (qreal*)yValues, numberOfParameter, numberOfDataPoints,
    (qreal*)parameterLimitMin, (qreal*)parameterLimitMax,
    NULL, maxIterations, opts, info, NULL, covar.data(),
    NULL);

  for (uint i=0; i<numberOfParameter; i++)
    parameterError[i] = sqrt(covar[i*numberOfParameter + i]); // diagonal

  return true;
}

void FittingLevenbergMarquardt::__functionJacobianLM(qreal* parameterValues, qreal* jacobian, int parameterLength, int xLength, void*) {
  for_int (xi,xLength) {
    for_i (parameterLength) {
      *jacobian++ = function->dy(xValues[xi],i,parameterValues);
    }
  }
}

//------------------------------------------------------------------------------
}}
// eof
