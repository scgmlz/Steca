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

    bool  checkValue(qreal value, qreal error=0);
    bool  setValue(qreal value, qreal error=0, bool force=false);

  private:
    qreal value;
    // constraints; TODO maybe not all needed?
    Range range;            ///< allowed range of values; if !isValid() -> same as <value,value>, i.e. fixed value
    qreal maxDelta;         ///< maximum change allowed; NaN -> no check
    qreal maxDeltaPercent;
    qreal maxError;         ///< maximum error allowed; NaN -> no check
    qreal maxErrorPercent;
  };

public:
  Function();
  virtual ~Function();

  virtual uint parameterCount() const = 0;
  virtual Parameter& getParameter(uint) = 0;

  virtual Parameter const& getParameter(uint i) const {
    return const_cast<Function*>(this)->getParameter(i);
  }

  /// the value of the function, outside or own parameter values
  virtual qreal y(qreal x, qreal const* parameterValues = nullptr) const = 0;

  /// partial derivative / parameter
  virtual qreal dy(qreal x, int parameterIndex, qreal const* parameterValues = nullptr) const = 0;
};

#ifndef QT_NO_DEBUG
QDebug& operator<<(QDebug&,Function const&);
#endif

class SingleFunction: public Function {
  SUPER(SingleFunction,Function)
public:
  SingleFunction();

  typedef QVector<Parameter> parameters_t;

  void setParameterCount(uint);
  uint parameterCount() const;
  Parameter& getParameter(uint);

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
  Parameter& getParameter(uint);

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, int parameterIndex, qreal const* parameterValues = nullptr) const;

protected:
  functions_t functions, functionForParameterIndex;
  QVector<Parameter*> parameters;
  QVector<uint> parameterStartForParameterIndex;
};

class Polynomial: public SingleFunction {
  SUPER(Polynomial,SingleFunction)
public:
  Polynomial(uint degree = 0);

  void setDegree(uint degree) {
    super::setParameterCount(degree+1);
  }

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, int parameterIndex, qreal const* parameterValues = nullptr) const;
};

}}

#endif
