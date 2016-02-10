#include "core_approximation.h"
#include <qmath.h>

namespace core {

// TODO
#define DBL_MIN -1000
#define DBL_MAX +1000

FunctionParameter::FunctionParameter()
{
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
//void FunctionParameter::setName(std::string name)
//{
//    this->_name = name;
//}
//bool FunctionParameter::setValue(double value)
//{
//    if (!this->checkValueWithCurrentSideCondition(value))
//        return false;
//    this->_value = value;
//    return true;
//}
//bool FunctionParameter::setValue(double value, double errorAbsolute)
//{
//    if (!this->checkValueWithCurrentSideCondition(value))
//        return false;
//    if (!this->checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, value))
//        return false;
//    this->_value = value;
//    this->_errorAbsolute = errorAbsolute;
//    return true;
//}
//bool FunctionParameter::setErrorAbsolute(double errorAbsolute)
//{
//    if (!this->checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, this->_value))
//        return false;
//    this->_errorAbsolute = errorAbsolute;
//    return true;
//}
//bool FunctionParameter::checkValueWithCurrentSideCondition(double value)
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
//        double deltaPercent = qAbs(this->_value - value) / this->_value * 100;
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
//bool FunctionParameter::checkErrorAbsoluteWithCurrentSideCondition(double errorAbsolute, double value/*needed to check against relativ errors*/)
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
//        double errorPercent = errorAbsolute / value * 100;
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
//    this->_parameterVector = other._parameterVector;
//}
Function::~Function()
{
}
//void Function::_createVectors(unsigned int size)
//{
//    this->_parameterVector.assign(size, FunctionParameter());
//}
//void Function::_resizeVectors(unsigned int size)
//{
//    this->_parameterVector.resize(size, FunctionParameter());
//}
//bool Function::setParameterWithConditionalCheck(qreal value, unsigned int parameter)
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
//bool Function::setParameterErrorAbsoluteWithConditionalCheck(qreal errorAbsolute, qreal value, unsigned int parameter)
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
//bool Function::getParameterAbsoluteLimitIsUsed(unsigned int parameter)
//{
//    if (parameter < this->_parameterVector.size())
//        return this->_parameterVector[parameter].valueLimitsIsUsed;
//    else
//        return false;
//}
//qreal Function::getParameterAbsoluteLimitMin(unsigned int parameter)
//{
//    qreal limit = -DBL_MAX;
//    if (parameter < this->_parameterVector.size())
//    {
//        if (this->getParameterAbsoluteLimitIsUsed(parameter))
//            limit = this->_parameterVector[parameter].valueLimits.min;
//    }
//    return limit;
//}
//qreal Function::getParameterAbsoluteLimitMax(unsigned int parameter)
//{
//    qreal limit = DBL_MAX;
//    if (parameter < this->_parameterVector.size())
//    {
//        if (this->getParameterAbsoluteLimitIsUsed(parameter))
//            limit = this->_parameterVector[parameter].valueLimits.max;
//    }
//    return limit;
//}
//std::vector<qreal> Function::getParameterValueVector()
//{
//    std::vector<qreal> values;

//    for (unsigned int i=0; i<this->getParameterNumber(); i++)
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
//bool Function::checkParameterForCurrentSideCondition(qreal value, unsigned int parameter)
//{
//    if (parameter >= this->_parameterVector.size())
//        return false;

//    return this->_parameterVector[parameter].checkValueWithCurrentSideCondition(value);
//}
//bool Function::checkParameterErrorAbsoluteForCurrentSideCondition(qreal errorAbsolute, qreal value/*needed to check against relativ errors*/, unsigned int parameter)
//{
//    if (parameter >= this->_parameterVector.size())
//        return false;

//    return this->_parameterVector[parameter].checkErrorAbsoluteWithCurrentSideCondition(errorAbsolute, value);
//}

Polynomial::Polynomial() {
  setDegree(0);
}

Polynomial::Polynomial(const Polynomial& other) : Function(other)
{
    //this->_clone(this, polynomial);
}
//Function* Polynomial::clone()
//{
//    Polynomial *tempPolynomial = new Polynomial(*this);
//    return (Function*)tempPolynomial;
//}
//qreal Polynomial::_getY(const qreal &x, const std::vector<qreal> &parameter)
//{
//    return this->__calculateY(x, parameter);
//}
//bool Polynomial::_addY(const qreal *x, const unsigned int &xLength, const std::vector<qreal> &parameter, qreal *y, const unsigned int &yLength)
//{
//    if (xLength != yLength)
//        return false;

//    for (unsigned int i=0; i<xLength; i++)
//    {
//        y[i] += this->__calculateY(x[i], parameter);
//    }
//    return true;
//}
//std::vector<qreal> Polynomial::_getDyda(const qreal &x, const std::vector<qreal> &parameter)
//{
//    qreal *jacobianPart = new qreal[parameter.size()];
//    memset(jacobianPart, 0, parameter.size() * sizeof(qreal));

//    if (this->__calculateDyda(x, parameter, 0, jacobianPart, (unsigned int)parameter.size()))
//    {
//        std::vector<qreal> tempVec(this->getParameterNumber());
//        for (unsigned int i=0; i<this->getParameterNumber(); i++)
//            tempVec[i] = jacobianPart[i];
//        return tempVec;
//    }

//    std::vector<qreal> tempVec;
//    return tempVec;

//    //std::cout << "Polynomial::_getDyda: not implemented" << std::endl;

//    //std::vector<qreal> tempVec;
//    //return tempVec;
//}
//bool Polynomial::_addDyda(const qreal *x, const unsigned int &xLength, const std::vector<qreal> &parameter, const unsigned int &parameterStartIndex, const unsigned int &parameterDeltaIndex, qreal *jacobian, const unsigned int &jacobianLength)
//{
//    if ((xLength * parameterDeltaIndex) != jacobianLength)
//        return false;

//    for (unsigned int xIndex=0, jacobianIndex=parameterStartIndex; xIndex<xLength; xIndex++, jacobianIndex+=parameterDeltaIndex)
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
//    for (unsigned int i=0; i<parameter.size(); i++)
//        value += parameter[i] * pow(x, (int)i);
//    return value;
//}
//#ifdef _MSC_VER
//__forceinline bool Polynomial::__calculateDyda(const qreal &x, const std::vector<qreal> &parameter, const unsigned int &positionInsideTarget, qreal *target, const unsigned int &targetLength)
//#else
//inline bool Polynomial::__calculateDyda(const qreal &x, const std::vector<qreal> &parameter, const unsigned int &positionInsideTarget, qreal *target, const unsigned int &targetLength)
//#endif
//{
//    if ((positionInsideTarget + parameter.size()) > targetLength)
//        return false;

//    for (unsigned int i=0; i<parameter.size(); i++)
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

//unsigned int Curve::getCurve(ApproximationTools::Curve &curve, double *x, double *y, double *tolerance, const unsigned int size)
//{
//    unsigned int counter;
//    for (counter = 0; counter<size; counter++)
//    {
//        CurvePoint point;
//        if (curve.getPoint(point, counter))
//        {
//            if (x != NULL)
//                x[counter] = point.getX();
//            if (y != NULL)
//                y[counter] = point.getY();
//            if (tolerance != NULL)
//                tolerance[counter] = point.getToleranz();
//        }
//        else
//        {
//            break;
//        }
//    }

//    return counter;
//}

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

//bool Curve::getPoint(ApproximationTools::CurvePoint &point, unsigned int index/*=0*/)
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
//    for (unsigned int i=0; i<curveVector.size(); i++)
//        this->__redefineDomain(curveVector[i]);
//}
//std::vector<CurvePoint> Curve::getCurve()
//{
//    return this->__curveVector;
//}
//unsigned int Curve::getCurve(double *x, double *y, double *tolerance, const unsigned int size)
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
//unsigned int Curve::getCurveSize()
//{
//    return (unsigned int)this->__curveVector.size();
//}
//bool Curve::isCurveEmpty()
//{
//    return this->__curveVector.empty();
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

//    for (unsigned int i=0; i<((ApproximationTools::Curve&)lhs).getCurveSize(); i++)
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
