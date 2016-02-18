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

bool FittingMethod::fit(Function& function_, Curve& curve_, bool sideConditionCheckIsActive) {
  function = &function_; // TODO perhaps not needed and could be handle by the callback data?
  curve    = &curve_;

  if (curve->isEmpty()) return false;

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

  uint pointCount = curve->pointCount();
  reals_t xValues(pointCount), yValues(pointCount);

  for_i (pointCount) {
    auto point = curve->getPoint(i);
    xValues[i] = point.x;
    yValues[i] = point.y;
  }


//  if (curve.getCurveSize() != curve.getCurve(this->_workingXValues, workingYValues, toleranz, curve.getCurveSize()))
//  if (!this->_createParameterArrays(parameter, parameterLimitMax, parameterLimitMin, parameterArraySize))
//  //Run Fit
//  bool returnValue = false;

  __xs = xValues.data();

  if (approximate(parValue.data(),parMin.data(),parMax.data(),parError.data(),parCount,yValues.data(),pointCount)) {
  }

  //  if (this->_approximation(parameter, parameterLimitMax , parameterLimitMin, parameterError, parameterArraySize, workingYValues, curve.getCurveSize()))
//  {
//      //Fit was a success
//      //*****************

//      //Read final parameter and check the limits of the new calculated Parameters
//      if (sideConditionCheckIsActive)
//      {
//          if (this->_readParameterArray(parameter, parameterError, parameterArraySize))
//              returnValue = true;
//      }
//      else
//      {
//          if (this->_readParameterArrayWithoutCheck(parameter, parameterError, parameterArraySize))
//              returnValue = true;
//      }
//      //Copy new fitted parameter from the working copy back to the function vector.
//      if (returnValue)
//          FittingMethods::_cloneFunctionVector(this->_functionVecWorkingCopy, this->_functionVec);
//  }


  //  return returnValue;
}

FittingLinearLeastSquare::FittingLinearLeastSquare() {
}

bool FittingLinearLeastSquare::approximate(qreal *, qreal *, qreal *, qreal *, uint, qreal *, uint)
{

}

FittingLevenbergMarquardt::FittingLevenbergMarquardt() {
}

bool FittingLevenbergMarquardt::approximate(qreal *parameter /*IO*/,
                                            qreal *parameterLimitMax /*I*/,
                                            qreal *parameterLimitMin /*I*/,
                                            qreal *parameterError /*O*/,
                                            uint   numberOfParameter /*I*/,
                                            qreal *yValues /*I*/,
                                            uint   numberOfDataPoints /*I*/) {

  //Fit
  double *covar = new double[numberOfParameter*numberOfParameter];
  double info[LM_INFO_SZ];
  double opts[LM_OPTS_SZ];
  opts[0]=LM_INIT_MU;
  opts[1]=1E-12;
  opts[2]=1E-12;
  opts[3]=1E-18;
  DelegateCalculationDbl function(this, &FittingLevenbergMarquardt::__functionLM);
  DelegateCalculationDbl functionJacobian(this, &FittingLevenbergMarquardt::__functionJacobianLM);

  dlevmar_bc_der( &function					/* Function */,
                                    &functionJacobian			/* Function to fill the Jacobian Matrix */,
                                    parameter					/* I/O: initial parameter estimates. On output has the estimated solution */,
                                    yValues					/* Input: measurement vector. NULL implies a zero vector */,
                                    numberOfParameter			/* Input: parameter vector dimension (i.e. #unknowns) */,
                                    numberOfDataPoints		/* Input: measurement point vector dimension */,
                                    parameterLimitMin,        /* Input: vector of lower bounds. If NULL, no lower bounds apply */
                                    parameterLimitMax,        /* Input: vector of upper bounds. If NULL, no upper bounds apply */
                                    NULL,						/* Input: diagonal scaling constants. NULL implies no scaling */
                                    1000						/* Input: maximum number of iterations */,
                                    opts,						/* Input: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3]. Respectively the scale factor for initial \mu,
                                                               *        stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2. Set to NULL for defaults to be used
                                                               */
                                    info,						/* Output: information regarding the minimization. Set to NULL if don't care
                                                               * info[0]= ||e||_2 at initial p.
                                                               * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                                                               * info[5]= # iterations,
                                                               * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                                                               *                                 2 - stopped by small Dp
                                                               *                                 3 - stopped by itmax
                                                               *                                 4 - singular matrix. Restart from current p with increased mu
                                                               *                                 5 - no further error reduction is possible. Restart with increased mu
                                                               *                                 6 - stopped by small ||e||_2
                                                               *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                                                               * info[7]= # function evaluations
                                                               * info[8]= # Jacobian evaluations
                                                               * info[9]= # linear systems solved, i.e. # attempts for reducing error
                                                               */
                                    NULL  /* working memory: If !=NULL, it is assumed to point to a memory chunk at least LM_DER_WORKSZ(m, n)*sizeof(double) bytes long */,
                                    covar /* Output: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */,
                                    NULL  /* Pointer to possibly additional data, passed uninterpreted to func & jacf. Set to NULL if not needed. */
                                    ); // with analytic Jacobian

  for (uint i=0; i<numberOfParameter; i++)
      parameterError[i] = sqrt(covar[i*numberOfParameter + i]);

  return true;
}

void FittingLevenbergMarquardt::__functionLM(qreal* /*parameter*/, qreal* data, int parameterLength, int dataLength, void*) {
  for_i (dataLength)
    data[i] = function->y(__xs[i]);
}

void FittingLevenbergMarquardt::__functionJacobianLM(qreal* parameter, qreal* jacobian, int parameterLength, int dataLength, void* adata) {
}

}}

// eof
