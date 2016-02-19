#include "approx_methods.h"
#include "LevMar/levmar.h"

namespace core { namespace approx {

FittingMethod::FittingMethod() {
}

FittingMethod::~FittingMethod() {
}

bool FittingMethod::fitWithoutCheck(Function& function, Curve& curve) {
  return fit(function, curve, false);
}

bool FittingMethod::fit(Function& function_, Curve& curve, bool sideConditionCheckIsActive) {
  function = &function_; // TODO perhaps not needed and could be handle by the callback data?

  if (curve.isEmpty()) return false;

  // prepare data in a required format
  uint parCount = function->parameterCount();
  reals_t parValue(parCount), parMin(parCount), parMax(parCount), parError(parCount);

  for_i (parCount) {
    auto par = function->getParameter(i);
    auto rge = par.getRange();
    parValue[i] = par.getValue();
    parMin[i]   = rge.min;
    parMax[i]   = rge.max;
  }

  uint pointCount = curve.pointCount();
  reals_t xValues(pointCount), yValues(pointCount);

  for_i (pointCount) {
    auto point = curve.getPoint(i);
    xValues[i] = point.x;
    yValues[i] = point.y;
  }

  if (!approximate(parValue.data(),parMin.data(),parMax.data(),parError.data(),parCount,xValues.data(),yValues.data(),pointCount))
    return false;

  // read data
  for_i (parCount) {
    if (!function->getParameter(i).setValue(parValue[i], parError[i], !sideConditionCheckIsActive))
      return false;
  }

  return true;
}

FittingLinearLeastSquare::FittingLinearLeastSquare() {
}

bool FittingLinearLeastSquare::approximate(qreal*,qreal const*,qreal const*,qreal*,uint,qreal const*,qreal const*,uint) {
  NEVER_HERE return false; // TODO
}

FittingLevenbergMarquardt::FittingLevenbergMarquardt() {
}

bool FittingLevenbergMarquardt::approximate(
  qreal*        parameter,          // IO initial parameter estimates -> estimated solution
  qreal const*  parameterLimitMin,  // I
  qreal const*  parameterLimitMax,  // I
  qreal*        parameterError,     // O
  uint          numberOfParameter,  // I
  qreal const*  xValues,            // I
  qreal const*  yValues,            // I
  uint          numberOfDataPoints) // I
{
  DelegateCalculationDbl function(this, &FittingLevenbergMarquardt::__functionLM);
  // Function to fill the Jacobian Matrix
  DelegateCalculationDbl functionJacobian(this, &FittingLevenbergMarquardt::__functionJacobianLM);

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
    (qreal*)xValues  // additional data - TODO differently?
  );

  for (uint i=0; i<numberOfParameter; i++)
    parameterError[i] = sqrt(covar[i*numberOfParameter + i]); // diagonal

  return true;
}

void FittingLevenbergMarquardt::__functionLM(qreal* parameterValues, qreal* yValues, int /*parameterLength*/, int xLength, void* xValues) {
  for_i (xLength) {
    yValues[i] = function->y(((qreal*)xValues)[i], parameterValues);
  }
}

void FittingLevenbergMarquardt::__functionJacobianLM(qreal* parameterValues, qreal* jacobian, int parameterLength, int xLength, void* xValues) {
  for_i (xLength) {
    int &xi = i;
    for_i (parameterLength) {
      *jacobian++ = function->dy(((qreal*)xValues)[xi],i,parameterValues);
    }
  }
}

}}

// eof
