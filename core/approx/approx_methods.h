#ifndef APPROX_METHODS_H
#define APPROX_METHODS_H

#include "approx_functions.h"

namespace core { namespace approx {

class Curve {
public:
  struct Point {
    Point(qreal x=0, qreal y=0, qreal tolerance=0.001);
    qreal x, y, tolerance;
  };

public:
  Curve();

  typedef QVector<Point> curve_t;

  void addPoint(Point const&);

private:
  curve_t curve;
  Range domainX, domainY;
};

class FittingMethod {
public:
  FittingMethod(Function&);
  virtual ~FittingMethod();

  bool fitWithoutCheck(Curve&);

protected:
  Function &function;

  virtual bool fit(Curve&, bool sideConditionCheckIsActive) = 0;
};

class FittingLinearLeastSquare: public FittingMethod {
  SUPER(FittingLinearLeastSquare,FittingMethod)
public:
  FittingLinearLeastSquare(Function&);

protected:
  bool fit(Curve&, bool sideConditionCheckIsActive);
};

class FittingLevenbergMarquardt: public FittingLinearLeastSquare {
  SUPER(FittingLevenbergMarquardt,FittingLinearLeastSquare)
public:
  FittingLevenbergMarquardt(Function&);

protected:
  bool fit(Curve&, bool sideConditionCheckIsActive);
};

}}

#endif
