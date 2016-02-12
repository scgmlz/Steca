#include "core_approximation.h"
#include <qmath.h>

namespace core {

Function::Parameter::Parameter(): value(0) {
//    this->_name = "unknown";
//    this->_value = 0;
//    this->_errorAbsolute = 0;
//    this->_valueIsFixed = false;

//    //side conditions
//    this->valueLimits = Limits();
//    this->valueLimitsIsUsed = false;

//    this->valueDeltaMaxAbsolute = DBL_MAX;
//    this->valueDeltaMaxAbsoluteIsUsed = false;

//    this->valueDeltaMaxPercent = DBL_MAX;
//    this->valueDeltaMaxPercentIsUsed = false;

//    this->errorAbsolutMax = DBL_MAX;
//    this->errorAbsolutMaxIsUsed = false;

//    this->errorPercentMax = DBL_MAX;
  //    this->errorPercentMaxIsUsed = false;
}

qreal Function::Parameter::getValue() const {
  return value;
}

Range Function::Parameter::getRange() const {
  return range.isValid() ? range : Range(value);
}

bool Function::Parameter::checkValue(qreal v) {
  if (range.isValid()) {
    if (!range.contains(v)) return false;
  }

//  if (this->valueDeltaMaxAbsoluteIsUsed)
//  {
//    if (qAbs(value - this->_value) > this->valueDeltaMaxAbsolute)
//    {
//      //std::cout << "Parameter \""
//      //		  << this->_name.c_str()
//      //		  << "\" is out of definition: fitted diffrent is "
//      //		  << std::fabs(value - this->_value)
//      //		  << ", Max different is "
//      //		  << this->valueDeltaMaxAbsolute
//      //		  << std::endl;
//      return false;
//    }
//  }

//  if (this->valueDeltaMaxPercentIsUsed)
//  {
//    qreal deltaPercent = qAbs(this->_value - value) / this->_value * 100;
//    if (deltaPercent > this->valueDeltaMaxPercent)
//    {
//      //std::cout << "Parameter \""
//      //		  << this->_name.c_str()
//      //		  << "\" is out of definition: different is "
//      //		  << deltaPercent
//      //		  << "%, Max different is "
//      //		  << this->valueDeltaMaxPercent
//      //		  << "%"
//      //		  << std::endl;
//      return false;
//    }
//  }

  return true;
}

//void FunctionParameter::setName(std::string name)
//{
//    this->_name = name;
//}

//bool FunctionParameter::setValue(qreal value)
//{
//    if (!this->checkValueWithCurrentSideCondition(value))
//        return false;
//    this->_value = value;
//    return true;
//}
//bool FunctionParameter::setValue(qreal value, qreal errorAbsolute)
//{
//    if (!this->checkValueWithCurrentSideCondition(value))
//        return false;
//    if (!this->checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, value))
//        return false;
//    this->_value = value;
//    this->_errorAbsolute = errorAbsolute;
//    return true;
//}
//bool FunctionParameter::setErrorAbsolute(qreal errorAbsolute)
//{
//    if (!this->checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, this->_value))
//        return false;
//    this->_errorAbsolute = errorAbsolute;
//    return true;
//}
//bool FunctionParameter::checkValueWithCurrentSideCondition(qreal value)
//{
//    if (this->valueLimitsIsUsed)
//    {
//        if ((value < this->valueLimits.min) || (value > this->valueLimits.max))
//        {
//            //std::cout << "Parameter \""
//            //		  << this->_name.c_str()
//            //		  << "\" is out of definition: fitted value="
//            //		  << value
//            //		  << " LimitMax="
//            //		  << this->valueLimits.max
//            //		  << " and LimitMin="
//            //		  << this->valueLimits.min
//            //		  << std::endl;
//            return false;
//        }
//    }

//    if (this->valueDeltaMaxAbsoluteIsUsed)
//    {
//        if (qAbs(value - this->_value) > this->valueDeltaMaxAbsolute)
//        {
//            //std::cout << "Parameter \""
//            //		  << this->_name.c_str()
//            //		  << "\" is out of definition: fitted diffrent is "
//            //		  << std::fabs(value - this->_value)
//            //		  << ", Max different is "
//            //		  << this->valueDeltaMaxAbsolute
//            //		  << std::endl;
//            return false;
//        }
//    }

//    if (this->valueDeltaMaxPercentIsUsed)
//    {
//        qreal deltaPercent = qAbs(this->_value - value) / this->_value * 100;
//        if (deltaPercent > this->valueDeltaMaxPercent)
//        {
//            //std::cout << "Parameter \""
//            //		  << this->_name.c_str()
//            //		  << "\" is out of definition: different is "
//            //		  << deltaPercent
//            //		  << "%, Max different is "
//            //		  << this->valueDeltaMaxPercent
//            //		  << "%"
//            //		  << std::endl;
//            return false;
//        }
//    }

//    return true;
//}
//bool FunctionParameter::checkErrorAbsoluteWithCurrentSideCondition(qreal errorAbsolute, qreal value/*needed to check against relativ errors*/)
//{
//    if (this->errorAbsolutMaxIsUsed)
//    {
//        if (errorAbsolute > this->errorAbsolutMax)
//        {
//            //std::cout << "Error of parameter \""
//            //		  << this->_name.c_str()
//            //		  << "\" is out of definition: calculated absolute error="
//            //		  << errorAbsolute
//            //		  << " max absolute error="
//            //		  << this->errorAbsolutMax
//            //		  << std::endl;
//            return false;
//        }
//    }

//    if (this->errorPercentMaxIsUsed)
//    {
//        qreal errorPercent = errorAbsolute / value * 100;
//        if (errorPercent > this->errorPercentMax)
//        {
//            //std::cout << "Error of parameter \""
//            //		  << this->_name.c_str()
//            //		  << "\" is out of definition: calculated relativ error="
//            //		  << errorPercent
//            //		  << "% max relativ error="
//            //		  << this->errorPercentMax << "%"
//            //		  << std::endl;
//            return false;
//        }
//    }

//    return true;
//}

Function::Function() {
}

//Function::Function(Function const& other)
//{
//    this->parameters = other.parameters;
//}
Function::~Function()
{
}

void Function::setParameterCount(uint n) {
  parameters.fill(Parameter(),n);
}

uint Function::getParameterCount() const {
  return parameters.count();
}

//void Function::_createVectors(uint size)
//{
//    this->parameters.assign(size, FunctionParameter());
//}
//void Function::_resizeVectors(uint size)
//{
//    this->parameters.resize(size, FunctionParameter());
//}
//bool Function::setParameterWithConditionalCheck(qreal value, uint parameter)
//{
//    if (checkParameterForCurrentSideCondition(value, parameter))
//    {
//        this->setParameter(value, parameter);
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//bool Function::setParameterErrorAbsoluteWithConditionalCheck(qreal errorAbsolute, qreal value, uint parameter)
//{
//    if (checkParameterErrorAbsoluteForCurrentSideCondition(errorAbsolute, value, parameter))
//    {
//        this->setParameterErrorAbsolute(errorAbsolute, parameter);
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//bool Function::getParameterAbsoluteLimitIsUsed(uint parameter)
//{
//    if (parameter < this->parameters.size())
//        return this->parameters[parameter].valueLimitsIsUsed;
//    else
//        return false;
//}
//qreal Function::getParameterAbsoluteLimitMin(uint parameter)
//{
//    qreal limit = -DBL_MAX;
//    if (parameter < this->parameters.size())
//    {
//        if (this->getParameterAbsoluteLimitIsUsed(parameter))
//            limit = this->parameters[parameter].valueLimits.min;
//    }
//    return limit;
//}
//qreal Function::getParameterAbsoluteLimitMax(uint parameter)
//{
//    qreal limit = DBL_MAX;
//    if (parameter < this->parameters.size())
//    {
//        if (this->getParameterAbsoluteLimitIsUsed(parameter))
//            limit = this->parameters[parameter].valueLimits.max;
//    }
//    return limit;
//}
//std::vector<qreal> Function::getParameterValueVector()
//{
//    std::vector<qreal> values;

//    for (uint i=0; i<this->getParameterNumber(); i++)
//        values.push_back(this->getParameter(i));

//    return values;
//}
//qreal Function::getY(qreal x)
//{
//    return this->_getY(x, this->getParameterValueVector());
//}
//std::vector<qreal> Function::getDyda(qreal x)
//{
//    return this->_getDyda(x, this->getParameterValueVector());
//}
//bool Function::checkParameterForCurrentSideCondition(qreal value, uint parameter)
//{
//    if (parameter >= this->parameters.size())
//        return false;

//    return this->parameters[parameter].checkValueWithCurrentSideCondition(value);
//}
//bool Function::checkParameterErrorAbsoluteForCurrentSideCondition(qreal errorAbsolute, qreal value/*needed to check against relativ errors*/, uint parameter)
//{
//    if (parameter >= this->parameters.size())
//        return false;

//    return this->parameters[parameter].checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, value);
//}

Polynomial::Polynomial(uint degree) {
  setDegree(degree);
}

//qreal Polynomial::_getY(const qreal &x, const std::vector<qreal> &parameter)
//{
//    return this->__calculateY(x, parameter);
//}
//bool Polynomial::_addY(const qreal *x, const uint &xLength, const std::vector<qreal> &parameter, qreal *y, const uint &yLength)
//{
//    if (xLength != yLength)
//        return false;

//    for (uint i=0; i<xLength; i++)
//    {
//        y[i] += this->__calculateY(x[i], parameter);
//    }
//    return true;
//}
//std::vector<qreal> Polynomial::_getDyda(const qreal &x, const std::vector<qreal> &parameter)
//{
//    qreal *jacobianPart = new qreal[parameter.size()];
//    memset(jacobianPart, 0, parameter.size() * sizeof(qreal));

//    if (this->__calculateDyda(x, parameter, 0, jacobianPart, (uint)parameter.size()))
//    {
//        std::vector<qreal> tempVec(this->getParameterNumber());
//        for (uint i=0; i<this->getParameterNumber(); i++)
//            tempVec[i] = jacobianPart[i];
//        return tempVec;
//    }

//    std::vector<qreal> tempVec;
//    return tempVec;

//    //std::cout << "Polynomial::_getDyda: not implemented" << std::endl;

//    //std::vector<qreal> tempVec;
//    //return tempVec;
//}
//bool Polynomial::_addDyda(const qreal *x, const uint &xLength, const std::vector<qreal> &parameter, const uint &parameterStartIndex, const uint &parameterDeltaIndex, qreal *jacobian, const uint &jacobianLength)
//{
//    if ((xLength * parameterDeltaIndex) != jacobianLength)
//        return false;

//    for (uint xIndex=0, jacobianIndex=parameterStartIndex; xIndex<xLength; xIndex++, jacobianIndex+=parameterDeltaIndex)
//    {
//        if (!this->__calculateDyda(x[xIndex], parameter, jacobianIndex, jacobian, jacobianLength))
//            return false;
//    }

//    return true;

//    //std::cout << "Polynomial::_getDyda: not implemented" << std::endl;

//    //return false;
//}
//#ifdef _MSC_VER
//__forceinline qreal Polynomial::__calculateY(const qreal &x, const std::vector<qreal> &parameter)
//#else
//inline  qreal Polynomial::__calculateY(const qreal &x, const std::vector<qreal> &parameter)
//#endif
//{
//    qreal value = 0;
//    for (uint i=0; i<parameter.size(); i++)
//        value += parameter[i] * pow(x, (int)i);
//    return value;
//}
//#ifdef _MSC_VER
//__forceinline bool Polynomial::__calculateDyda(const qreal &x, const std::vector<qreal> &parameter, const uint &positionInsideTarget, qreal *target, const uint &targetLength)
//#else
//inline bool Polynomial::__calculateDyda(const qreal &x, const std::vector<qreal> &parameter, const uint &positionInsideTarget, qreal *target, const uint &targetLength)
//#endif
//{
//    if ((positionInsideTarget + parameter.size()) > targetLength)
//        return false;

//    for (uint i=0; i<parameter.size(); i++)
//        target[positionInsideTarget + i] = pow(x, (int)i);		//Ableitung ai

//    return true;
//}

//bool ApproximationTools::operator==(ApproximationTools::CurvePoint const&lhs, ApproximationTools::CurvePoint const&rhs)
//{
//    if(((ApproximationTools::CurvePoint&)lhs).getX() != ((ApproximationTools::CurvePoint&)rhs).getX())
//        return false;
//    if(((ApproximationTools::CurvePoint&)lhs).getY() != ((ApproximationTools::CurvePoint&)rhs).getY())
//        return false;
//    if(((ApproximationTools::CurvePoint&)lhs).getToleranz() != ((ApproximationTools::CurvePoint&)rhs).getToleranz())
//        return false;

//    return true;
//}
//bool ApproximationTools::operator!=(ApproximationTools::CurvePoint const&lhs, ApproximationTools::CurvePoint const&rhs)
//{
//    return !(lhs == rhs);
//}

void Curve::getCurve(reals_t &x, reals_t &y, reals_t &tolerance) const {
  x.clear(); y.clear(); tolerance.clear();
  for_i(curve.size()) {
    auto point = curve.at(i);
    x.append(point.x);
    y.append(point.y);
    tolerance.append(point.tolerance);
  }
}

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

//bool Curve::getPoint(ApproximationTools::CurvePoint &point, uint index/*=0*/)
//{
//    if (index < this->__curveVector.size())
//    {
//        point = this->__curveVector[index];
//        return true;
//    }
//    else return false;
//}
//void Curve::setCurve(std::vector<CurvePoint> curveVector)
//{
//    this->deleteCurve();
//    this->__curveVector = curveVector;
//    for (uint i=0; i<curveVector.size(); i++)
//        this->__redefineDomain(curveVector[i]);
//}
//std::vector<CurvePoint> Curve::getCurve()
//{
//    return this->__curveVector;
//}
//uint Curve::getCurve(qreal *x, qreal *y, qreal *tolerance, const uint size)
//{
//    return Curve::getCurve(*this, x, y, tolerance, size);
//}
//void Curve::deleteCurve()
//{
//    this->__curveVector.clear();
//    this->__domainX.min = DBL_MAX;
//    this->__domainX.max = DBL_MIN;
//    this->__domainX.min = DBL_MAX;
//    this->__domainX.max = DBL_MIN;
//}
//ApproximationTools::Limits Curve::getDomainX()
//{
//    return this->__domainX;
//}
//ApproximationTools::Limits Curve::getDomainY()
//{
//    return this->__domainY;
//}
//void Curve::__redefineDomain(ApproximationTools::CurvePoint point)
//{
//    if (point.getX() > this->__domainX.max)
//        this->__domainX.max = point.getX();
//    if (point.getX() < this->__domainX.min)
//        this->__domainX.min = point.getX();
//    if (point.getY() > this->__domainY.max)
//        this->__domainY.max = point.getY();
//    if (point.getY() < this->__domainY.min)
//        this->__domainY.min = point.getY();
//}
//bool ApproximationTools::operator==(ApproximationTools::Curve const&lhs, ApproximationTools::Curve const&rhs)
//{
//    if(((ApproximationTools::Curve&)lhs).getDomainX() != ((ApproximationTools::Curve&)rhs).getDomainX())
//        return false;
//    if(((ApproximationTools::Curve&)lhs).getDomainY() != ((ApproximationTools::Curve&)rhs).getDomainY())
//        return false;
//    if(((ApproximationTools::Curve&)lhs).getCurveSize() != ((ApproximationTools::Curve&)rhs).getCurveSize())
//        return false;

//    for (uint i=0; i<((ApproximationTools::Curve&)lhs).getCurveSize(); i++)
//    {
//        if(((ApproximationTools::Curve&)lhs).getCurve()[i] != ((ApproximationTools::Curve&)rhs).getCurve()[i])
//            return false;
//    }

//    return true;
//}
//bool ApproximationTools::operator!=(ApproximationTools::Curve const&lhs, ApproximationTools::Curve const&rhs)
//{
//    return !(lhs == rhs);
//}

}

// eof
