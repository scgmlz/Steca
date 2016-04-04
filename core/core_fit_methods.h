// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_fit_methods.h
//! @brief     Fitting methods
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_FIT_METHODS_H
#define CORE_FIT_METHODS_H

#include "core_fit_functions.h"

namespace core {

class Curve;

namespace fit {
//------------------------------------------------------------------------------

class FittingMethod {
public:
  FittingMethod();
  virtual ~FittingMethod();

  bool fitWithoutChecks(Function&, core::Curve const&);

protected:
  bool fit(Function&, core::Curve const&, bool withChecks);
  virtual bool approximate(qreal*,qreal const*,qreal const*,qreal*,uint,qreal const*,uint) = 0;

  // these pointers are valid during fit() call
  Function*    function;
  qreal const* xValues;

protected:
  /// callback to calculate a vector of y(x)
  void __functionY(qreal*,qreal*,int,int,void*);
};

//------------------------------------------------------------------------------

class FittingLinearLeastSquare: public FittingMethod {
  SUPER(FittingLinearLeastSquare,FittingMethod)
public:
  FittingLinearLeastSquare();

protected:
  bool approximate(qreal*,qreal const*,qreal const*,qreal*,uint,qreal const*,uint);
};

//------------------------------------------------------------------------------

class FittingLevenbergMarquardt: public FittingMethod {
  SUPER(FittingLevenbergMarquardt,FittingMethod)
public:
  FittingLevenbergMarquardt();

protected:
  bool approximate(qreal*,qreal const*,qreal const*,qreal*,uint,qreal const*,uint);

private:
  void __functionJacobianLM(qreal*,qreal*,int,int,void*);
};

//------------------------------------------------------------------------------
}}
#endif
