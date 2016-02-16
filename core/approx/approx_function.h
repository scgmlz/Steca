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
    /// Constraint; if !isValid() -> same as <value,value>
    Range range;
  };

public:
  Function();
  virtual ~Function();

  typedef QVector<Parameter> parameters_t;

  void setParameterCount(uint);
  uint getParameterCount() const;

  parameters_t& getParameters() {
    return parameters;
  }

  parameters_t const& getParameters() const {
    return const_cast<Function*>(this)->getParameters();
  }

  virtual qreal y(qreal x) const = 0;

protected:
  parameters_t parameters;
};

class Functions final {
public:
  Functions();
 ~Functions();

  typedef QVector<Function*> functions_t;

  functions_t const& getFunctions() const {
    return functions;
  }

private:
  functions_t functions;
};

class SumFunctions: public Function {
  SUPER(SumFunctions,Function)
public:
  SumFunctions();

  qreal y(qreal x) const;

protected:
  Functions functions;
};

}}

#endif
