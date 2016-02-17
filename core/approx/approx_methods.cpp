#include "approx_methods.h"

namespace core { namespace approx {

Curve::Point::Point(qreal x_, qreal y_, qreal tolerance_)
: x(x_), y(y_), tolerance(tolerance_) {
}

Curve::Curve() {
}

void Curve::addPoint(Point const& point) {
  domainX.extend(point.x);
  domainY.extend(point.y);
  curve.append(point);
}

FittingMethod::FittingMethod(Function& function_)
: function(function_) {
}

FittingMethod::~FittingMethod() {
}

bool FittingMethod::fitWithoutCheck(Curve &curve) {
  return fit(curve, false);
}

FittingLinearLeastSquare::FittingLinearLeastSquare(Function& function)
: super(function) {
}

bool FittingLinearLeastSquare::fit(Curve&, bool sideConditionCheckIsActive) {
}

FittingLevenbergMarquardt::FittingLevenbergMarquardt(Function& function)
  : super(function) {
}

bool FittingLevenbergMarquardt::fit(Curve&, bool sideConditionCheckIsActive) {
}

}}

// eof
