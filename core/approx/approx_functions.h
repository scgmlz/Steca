#ifndef APPROX_FUNCTION_H
#define APPROX_FUNCTION_H

#include <core_lib.h>

namespace core { namespace approx {

class Function {
public:
  class Parameter final {
  public:
    Parameter();

    qreal getValue() const;
    Range getRange() const;

    bool  checkValue(qreal);
    bool  setValue(qreal, bool force=false);

  private:
    qreal value;
    Range range;  ///< Constraint; if !isValid() -> same as <value,value>
  };

public:
  Function();
  virtual ~Function();

  virtual uint parameterCount() const = 0;
  virtual Parameter const& getParameter(uint) const = 0;

  /// value of the function
  virtual qreal y(qreal x) const = 0;
  /// partial derivative / parameter
  virtual qreal dy(qreal x, int parameterIndex) const = 0;
};

class SingleFunction: public Function {
  SUPER(SingleFunction,Function)
public:
  SingleFunction();

  typedef QVector<Parameter> parameters_t;

  void setParameterCount(uint);
  uint parameterCount() const;
  Parameter const& getParameter(uint) const;

  parameters_t& getParameters() {
    return parameters;
  }

  parameters_t const& getParameters() const {
    return const_cast<thisCls*>(this)->getParameters();
  }

protected:
  parameters_t parameters;
};

class SumFunctions final: public Function {
  SUPER(SumFunctions,Function)
public:
  SumFunctions();
 ~SumFunctions();

  typedef QVector<Function*> functions_t;
  void addFunction(Function*);            ///< takes ownership

  uint parameterCount() const;
  Parameter const& getParameter(uint) const;

  qreal y(qreal x) const;
  qreal dy(qreal x, int parameterIndex) const;

protected:
  functions_t functions;
  uint parCount;
  QVector<Parameter const*> parameters;
};

class Polynomial: public SingleFunction {
  SUPER(Polynomial,SingleFunction)
public:
  Polynomial(uint degree = 0);

  void setDegree(uint degree) {
    super::setParameterCount(degree+1);
  }

  qreal y(qreal x) const;
  qreal dy(qreal x, int parameterIndex) const;
};

class Curve {
public:
  struct Point {
    Point(qreal x=0, qreal y=0, qreal tolerance=0.001);
    qreal x, y, tolerance;
  };

public:
  Curve();

  bool isEmpty() const;

  typedef QVector<Point> points_t;

  void addPoint(Point const&);
  uint pointCount() const;
  Point const& getPoint(uint) const;

private:
  points_t points;
  Range domainX, domainY;
};

}}

#endif
