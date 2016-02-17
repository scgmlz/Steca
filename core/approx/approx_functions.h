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

  virtual uint getParameterCount() const = 0;
  virtual qreal y(qreal x) const = 0;
};

class SingleFunction: public Function {
  SUPER(SingleFunction,Function)
public:
public:
  SingleFunction();

  typedef QVector<Parameter> parameters_t;

  void setParameterCount(uint);
  uint getParameterCount() const;

  parameters_t& getParameters() {
    return parameters;
  }

  parameters_t const& getParameters() const {
    return const_cast<thisCls*>(this)->getParameters();
  }

  virtual qreal y(qreal x) const = 0;

protected:
  parameters_t parameters;
};

class SumFunctions: public Function {
  SUPER(SumFunctions,Function)
public:
  SumFunctions();
 ~SumFunctions();

  typedef QVector<Function*> functions_t;
  void addFunction(Function*);            ///< takes ownership

  uint getParameterCount() const;

  qreal y(qreal x) const;

protected:
  functions_t functions;
};

class Polynomial: public SingleFunction {
  SUPER(Polynomial,SingleFunction)
public:
  Polynomial(uint degree = 0);

  void setDegree(uint degree) {
    super::setParameterCount(degree+1);
  }

  qreal y(qreal x) const;
};

}}

#endif
