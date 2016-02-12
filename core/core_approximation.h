/** \file
 * Approximation tools. Adapted from STeCa.
 */

#include "core_defs.h"
#include "core_lib.h"

#ifndef CORE_APPROXIMATION_H
#define CORE_APPROXIMATION_H

namespace core {

class Function {
public:
  class Parameter  {
  public:
    Parameter();

    qreal getValue() const;
    Range getRange() const;

    bool checkValue(qreal);

//    bool setValue(qreal, bool force=false);

    //    void setName(std::string name);
  //    bool setValue(qreal value
  //    bool setValue(qreal value, qreal errorAbsolute);
  //    bool setErrorAbsolute(qreal errorAbsolute);
  //    void setValueIsFixed(bool isFixed) {this->_valueIsFixed = isFixed;}

  //    void setValueDoNotCheckSideConditions(qreal value) {this->_value = value;}
  //    void setErrorAbsoluteDoNotCheckSideConditions(qreal errorAbsolute) {this->_errorAbsolute = errorAbsolute;}

  //    std::string getName() {return this->_name;}
  //    qreal getValue() {return this->_value;}
  //    qreal getErrorAbsolute() {return this->_errorAbsolute;}
  //    qreal getErrorPercent() {return this->_errorAbsolute / this->_value * 100;}
  //    bool getValueIsFixed() {return this->_valueIsFixed;}

  //    bool checkValueWithCurrentSideCondition(qreal value);
  //    bool checkErrorAbsoluteWithCurrentSideCondition(qreal errorAbsolute, qreal value/*needed to check against relativ errors*/);

  private:
    //    std::string _name;
    qreal value;
  //    qreal _errorAbsolute;
  //    bool _valueIsFixed;

    /// Constraint; if !isValid() -> same as <value,value>
    Range range;

  //    qreal valueDeltaMaxAbsolute;
  //    bool valueDeltaMaxAbsoluteIsUsed;

  //    qreal valueDeltaMaxPercent;
  //    bool valueDeltaMaxPercentIsUsed;

  //    qreal errorAbsolutMax;
  //    bool errorAbsolutMaxIsUsed;

  //    qreal errorPercentMax;
  //    bool errorPercentMaxIsUsed;
  };

public:
  Function();
  virtual ~Function();

  typedef QVector<Parameter> parameters_t;

  void setParameterCount(uint);
  uint getParameterCount() const;

  parameters_t const& getParameters() const {
    return parameters;
  }

protected:
  parameters_t parameters;

//  Function(const Function& other);

public:
  //Set Functions
//  void setParameter(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].setValueDoNotCheckSideConditions(value);}
//  void setParameterIsFixed(bool isFixed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].setValueIsFixed(isFixed);}
//  void setParameterErrorAbsolute(qreal errorAbsolute, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].setErrorAbsoluteDoNotCheckSideConditions(errorAbsolute);}

  //Set Conditional Functions
//  bool setParameterWithConditionalCheck(qreal value, unsigned int parameter);
//  bool setParameterErrorAbsoluteWithConditionalCheck(qrea_parameterVectorl errorAbsolute, qreal value, unsigned int parameter);

//  void setParameterAbsoluteLimitMin(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueLimits.min = value;}
//  void setParameterAbsoluteLimitMax(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueLimits.min = value;}
//  void setParameterAbsoluteLimitIsUsed(bool isUsed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueLimitsIsUsed = isUsed;}

//  void setParameterDeltaAbsolute(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueDeltaMaxAbsolute = value;}
//  void setParameterDeltaAbsoluteIsUsed(bool isUsed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueDeltaMaxAbsoluteIsUsed = isUsed;}

//  void setParameterDeltaPercent(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueDeltaMaxPercent = value;}
//  void setParameterDeltaPercentIsUsed(bool isUsed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].valueDeltaMaxPercentIsUsed = isUsed;}

//  void setParameterErrorAbsolutLimit(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].errorAbsolutMax = value;}
//  void setParameterErrorAbsolutLimitIsUsed(bool isUsed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].errorAbsolutMaxIsUsed = isUsed;}

//  void setParameterErrorPercentLimit(qreal value, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].errorPercentMax = value;}
//  void setParameterErrorPercentLimitIsUsed(bool isUsed, unsigned int parameter) {if (parameter < this->parameters.size()) this->parameters[parameter].errorPercentMaxIsUsed = isUsed;}

  //Get Functions
//  std::string getParameterName(unsigned int parameter) {if (parameter < this->parameters.size()) return this->parameters[parameter].getName(); else return "out of range";}
//  qreal getParameter(unsigned int parameter) {if (parameter < this->parameters.size()) return this->parameters[parameter].getValue(); else return 0;}
//  bool getParameterIsFixed(unsigned int parameter) {if (parameter < this->parameters.size()) return this->parameters[parameter].getValueIsFixed(); else return false;}
//  qreal getParameterErrorAbsolute(unsigned int parameter) {if (parameter < this->parameters.size()) return this->parameters[parameter].getErrorAbsolute(); else return 0;}
//  qreal getParameterErrorPercent(unsigned int parameter) {if (parameter < this->parameters.size()) return this->parameters[parameter].getErrorPercent(); else return 0;}
//  unsigned int getParameterNumber() {return (unsigned int)this->parameters.size();}
//  std::vector<qreal> getParameterValueVector();

  //Get Conditional Functions
//  bool getParameterAbsoluteLimitIsUsed(unsigned int parameter);
//  qreal getParameterAbsoluteLimitMin(unsigned int parameter);
//  qreal getParameterAbsoluteLimitMax(unsigned int parameter);

  //Check Functions
//  bool checkParameterForCurrentSideCondition(qreal value, unsigned int parameter);
//  bool checkParameterErrorAbsoluteForCurrentSideCondition(qreal errorAbsolute, qreal value/*needed to check against relativ errors*/, unsigned int parameter);

  //Math Functions
//  qreal getY(qreal x);
//  std::vector<qreal> getDyda(qreal x);

protected:
//  void _resizeVectors(unsigned int size);	//Resize the vector. If new parameter are added, than they are init with zero, not fixed and no side conditions.
//  void _createVectors(unsigned int size);	//Init all parameter with zero, not fixed and no side condintions.

  //Functions for FIT
  //This FIT Function do not have the mathematical code, because of the inline replacement.
  //The implementation must be done inside the child's to allow the compiler the inline replacement of the private calc function from the child's.
//  virtual qreal _getY(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/) = 0;
//  virtual bool _addY(const qreal *x /*I: x values of the curve*/,
//                     const unsigned int &xLength /*I: number of data points of the curve*/,
//                     const std::vector<qreal> &parameter /*I: parameter of the function*/,
//                     qreal *y /*O: calculated values of the curve*/,
//                     const unsigned int &yLength /*size of the allocated y array*/) = 0;

//  virtual std::vector<qreal> _getDyda(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/) = 0;
//  virtual bool _addDyda(const qreal *x /*I: x values of the curve*/,
//                        const unsigned int &xLength /*I: number of data points of the curve*/,
//                        const std::vector<qreal> &parameter /*I: parameter of the function*/,
//                        const unsigned int &parameterStartIndex /*I: Position where the values are written. e.g.: If it is the second function and the first has 3 parameter, this value must be 3.*/,
//                        const unsigned int &parameterDeltaIndex /*I: Number of parameter of all functions.*/,
//                        qreal *jacobian /*O: calculated jacobian matrix*/,
//                        const unsigned int &jacobianLength /*size of the allocated jacobian matrix*/) = 0;

protected:
};

class Polynomial: public Function {
  SUPER(Polynomial,Function)
public:
  Polynomial(uint degree = 0);

  void setDegree(uint degree) {
    super::setParameterCount(degree+1);
  }

  unsigned int getDegree() {return /*(unsigned int)this->getParameterNumber()*/ - 1;}

private:
  //Functions for FIT
  //This FIT Function can't be moved to the abstract father, because of the inline replacement.
  //The implementation must be done here to allow the compiler the inline replacement of the private calc function.
//  qreal _getY(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/);
//  bool _addY(const qreal *x /*I: x values of the curve*/,
//             const unsigned int &xLength /*I: number of data points of the curve*/,
//             const std::vector<qreal> &parameter /*I: parameter of the function*/,
//             qreal *y /*O: calculated values of the curve*/,
//             const unsigned int &yLength /*size of the allocated y array*/);

//  std::vector<qreal> _getDyda(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/);
//  bool _addDyda(const qreal *x /*I: x values of the curve*/,
//                const unsigned int &xLength /*I: number of data points of the curve*/,
//                const std::vector<qreal> &parameter /*I: parameter of the function*/,
//                const unsigned int &parameterStartIndex /*I: Position where the values are written. e.g.: If it is the second function and the first has 3 parameter, this value must be 3.*/,
//                const unsigned int &parameterDeltaIndex /*I: Number of parameter of all functions.*/,
//                qreal *jacobian /*O: calculated jacobian matrix*/,
//                const unsigned int &jacobianLength /*size of the allocated jacobian matrix*/);

  //Implementation of the calculation
//  qreal __calculateY(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/);
//  bool __calculateDyda(const qreal &x /*I: x value of the data point*/, const std::vector<qreal> &parameter /*I: parameter of the function*/,
//                       const unsigned int &positionInsideTarget /*I: Position where the values are written. e.g.: If it is the second function and the first has 3 parameter, this value must be 3.*/,
//                       qreal *target /*O: calculated jacobian matrix*/,
//                       const unsigned int &targetLength /*size of the allocated jacobian matrix*/);
};

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
  //	bool getPoint(ApproximationTools::CurvePoint &point, unsigned int index=0);
  //	void setCurve(std::vector<CurvePoint> curveVector);
  //	std::vector<CurvePoint> getCurve();
  void getCurve(reals_t &x, reals_t &y, reals_t &tolerance) const;  // TODO out?
  uint getCurveSize() const { return curve.size();    } // TODO out?
  bool isCurveEmpty() const { return curve.isEmpty(); } // TODO rename
  //	ApproximationTools::Limits getDomainX();
  //	ApproximationTools::Limits getDomainY();

private:
  curve_t curve;
  Range domainX, domainY;

private:
//	void __redefineDomain(ApproximationTools::CurvePoint point);

private:
};
//APPROXIMATION_TOOLS_EXPORT bool operator==(ApproximationTools::Curve const&lhs, ApproximationTools::Curve const&rhs);
//APPROXIMATION_TOOLS_EXPORT bool operator!=(ApproximationTools::Curve const&lhs, ApproximationTools::Curve const&rhs);

}

#endif
