#ifndef APPROX_METHODS_H
#define APPROX_METHODS_H

#include "approx_functions.h"

namespace core { namespace approx {

class FittingMethod {
public:
  FittingMethod();
  virtual ~FittingMethod();

  bool fitWithoutCheck(Function&,Curve&);

protected:
  bool fit(Function&,Curve&, bool sideConditionCheckIsActive);
  virtual bool approximate(qreal*,qreal*,qreal*,qreal*,uint,qreal*,qreal*,uint) = 0;

  Function *function; ///< set during fit()
  Curve    *curve;    ///< set during fit()
};

class FittingLinearLeastSquare: public FittingMethod {
  SUPER(FittingLinearLeastSquare,FittingMethod)
public:
  FittingLinearLeastSquare();

protected:
  bool approximate(qreal*,qreal*,qreal*,qreal*,uint,qreal*,uint);
};

class FittingLevenbergMarquardt: public FittingLinearLeastSquare {
  SUPER(FittingLevenbergMarquardt,FittingLinearLeastSquare)
public:
  FittingLevenbergMarquardt();

protected:
  bool approximate(qreal*,qreal*,qreal*,qreal*,uint,qreal*,qreal*,uint);

private:
  void __functionLM(qreal*,qreal*,int,int,void*);
  void __functionJacobianLM(qreal*,qreal*,int,int,void*);
};

}}

#endif
