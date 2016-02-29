/** \file
 * Fitting (was: approximation) methods
 */

#ifndef CORE_FIT_METHODS_H
#define CORE_FIT_METHODS_H

#include "core_fit_functions.h"

namespace core { namespace fit {
//------------------------------------------------------------------------------

class FittingMethod {
public:
  FittingMethod();
  virtual ~FittingMethod();

  bool fitWithoutCheck(Function&,Curve&);

protected:
  bool fit(Function&,Curve&, bool sideConditionCheckIsActive);
  virtual bool approximate(qreal*,qreal const*,qreal const*,qreal*,uint,qreal const*,uint) = 0;

  /// valid during fit()
  Function*    function;
  qreal const* xValues;

protected:
  void __functionY(qreal*,qreal*,int,int,void*);
};

//------------------------------------------------------------------------------

// TODO is needed?
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
