// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      fit_methods.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "fit_methods.h"
#include "def/def_alg.h"
#include "levmar.h"
#include <qmath.h>

namespace fit {
//------------------------------------------------------------------------------

using namespace typ;

Method::Method() {
}

Method::~Method() {
}

void Method::fit(Function& function, Curve::rc curve) {
  if (curve.isEmpty())
    return;

  function_ = &function;
  xValues_  = curve.xs().data();

  // prepare data in a required format
  uint      parCount = function_->parameterCount();
  qreal_vec parValue(parCount), parMin(parCount), parMax(parCount),
            parError(parCount);

  for_i (parCount) {
    auto par = function_->parameterAt(i);
    auto rge = par.valueRange();

    EXPECT(qIsFinite(par.value())) // TODO if not so, return false ?
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

void Method::callbackY(qreal* parValues, qreal* yValues,
                              int /*parCount*/, int    xLength, void*) {
  for_i (xLength)
    yValues[i] = function_->y(xValues_[i], parValues);
}

//------------------------------------------------------------------------------

LinearLeastSquare::LinearLeastSquare() {
}

template <typename T>
T* remove_const(T const* t) {
  return const_cast<T*>(t);
}

void LinearLeastSquare::approximate(
    qreal* params,  // IO initial parameter estimates -> estimated solution
    qreal const* paramsLimitMin,   // I
    qreal const* paramsLimitMax,   // I
    qreal*       paramsError,      // O
    uint         paramsCount,      // I
    qreal const* yValues,          // I
    uint         dataPointsCount)  // I
{
  DelegateCalculationDbl function(this, &Cls::callbackY);

  // information regarding the minimization
  double info[LM_INFO_SZ];

  // output covariance matrix
  qreal_vec covar(paramsCount * paramsCount);

  uint const maxIterations = 1000;

  dlevmar_bc_dif(&function, params, remove_const(yValues), to_i(paramsCount),
                 to_i(dataPointsCount), remove_const(paramsLimitMin),
                 remove_const(paramsLimitMax), NULL, maxIterations, NULL, info,
                 NULL, covar.data(), NULL);

  for_i (paramsCount)
    paramsError[i] = sqrt(covar[i * paramsCount + i]);  // the diagonal
}

//------------------------------------------------------------------------------

LevenbergMarquardt::LevenbergMarquardt() {
}

void LevenbergMarquardt::approximate(
    qreal* params,  // IO initial parameter estimates -> estimated solution
    qreal const* paramsLimitMin,   // I
    qreal const* paramsLimitMax,   // I
    qreal*       paramsError,      // O
    uint         paramsCount,      // I
    qreal const* yValues,          // I
    uint         dataPointsCount)  // I
{
  DelegateCalculationDbl function(this, &Cls::callbackY);
  DelegateCalculationDbl functionJacobian(this, &Cls::callbackJacobianLM);

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

  dlevmar_bc_der(&function, &functionJacobian, params, remove_const(yValues),
                 to_i(paramsCount), to_i(dataPointsCount), remove_const(paramsLimitMin),
                 remove_const(paramsLimitMax), NULL, maxIterations, opts, info, NULL,
                 covar.data(), NULL);

  for_i (paramsCount)
    paramsError[i] = sqrt(covar[i * paramsCount + i]);  // the diagonal
}

void LevenbergMarquardt::callbackJacobianLM(qreal* parValues,
                                                   qreal* jacobian,
                                                   int parameterLength,
                                                   int xLength, void*)
{
  for_int (ix, xLength) {
    for_int (ip, parameterLength) {
      *jacobian++ = function_->dy(xValues_[ix], to_u(ip), parValues);
    }
  }
}

//------------------------------------------------------------------------------
}
// eof
