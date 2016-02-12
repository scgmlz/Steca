#ifndef CORE_FITTING_H
#define CORE_FITTING_H

#include "core_defs.h"
#include "core_approximation.h"

namespace core {

class FittingMethods {
public:
  FittingMethods();
  virtual ~FittingMethods() {}

  typedef QVector<Function> functions_t;

//  FittingMethods(const ApproximationTools::FittingMethods& other);
//  FittingMethods& operator=(const ApproximationTools::FittingMethods &rhs);

protected:
//  void _copyInterface(const ApproximationTools::FittingMethods &rhs);

public:

protected:
//  static std::vector<ApproximationTools::Function*> _cloneFunctionVector(const std::vector<ApproximationTools::Function*> &sourceVector);
//  static void _cloneFunctionVector(const std::vector<ApproximationTools::Function*> &sourceVector, std::vector<ApproximationTools::Function*> &targetVector);

public:
//  bool fit(Curve &curve);
  bool fitWithoutCheck(Curve&);

//  qreal getY(qreal x);
//  std::vector<qreal> getDyda(qreal x);

  void addFunction(Function const&);
//  bool overwriteFunction(ApproximationTools::Function *function/*copy the given function*/,
//                         uint number=0/*index of the function that has to be overwritten*/);
//  Function getFunction(uint number=0/*index of the function that will be copied*/);

  uint getParameterCount() const; // TODO out?


//  uint getFunctionVecNumber() {return (uint)this->_functionVec.size();}
//  bool deleteLastFunction();
//  void deleteAllFunctions();

//  void setFunctionVecCurrentToOriginal();
//  void setFunctionVecOriginalToCurrent();
//  void setApproximationCompareWithOriginal(bool useOriginal) {this->_approximationCompareOriginalIsUsed = useOriginal;}
  void setApproximationCompareWithLastAccepted(bool useLastAccepted) {
    _approximationCompareOriginalIsUsed = !useLastAccepted;
  }

//  void printDebugInfo() {std::cout << "Time: " << this->_usedTime << "ms Calls:" << this->_numberOfCalls << std::endl;}

protected:
  virtual bool _fit(Curve &, bool sideConditionCheckIsActive) = 0;

protected:
//  void _createWorkingCopy();
//  void _deleteWorkingCopy();


protected:
  functions_t functions;
//  std::vector<ApproximationTools::Function*> _functionVecOriginal;
//  std::vector<ApproximationTools::Function*> _functionVec;
//  std::vector<ApproximationTools::Function*> _functionVecWorkingCopy;
  bool _approximationCompareOriginalIsUsed;
};

class FittingLinearLeastSquare: public FittingMethods {
  SUPER(FittingLinearLeastSquare,FittingMethods)
public:
  FittingLinearLeastSquare() {
  }

public:
  //overload operators
//  FittingLinearLeastSquare& operator=(const ApproximationTools::FittingLinearLeastSquare &rhs)
//  {
//    this->_copyInterface(rhs);
//    this->_workingXValues = NULL;
//    return *this;
//  }

protected:
  bool _fit(Curve&, bool sideConditionCheckIsActive);

protected:
  void _createParameterArrays(reals_t &parameter, reals_t &parameterMaxLimit, reals_t &parameterMinLimit) const;
  bool _readParameterArray(reals_t &parameter, reals_t &parameterError);
  bool _readParameterArrayWithoutCheck(reals_t &parameter, reals_t &parameterError);

protected:
  virtual void _approximation(qreal *parameter /*IO*/,
                              qreal *parameterLimitMax /*I*/,
                              qreal *parameterLimitMin /*I*/,
                              qreal *parameterError /*O*/,
                              uint numberOfParameter /*I*/,
                              qreal *yValues /*I*/,
                              uint numberOfDataPoints /*I*/);

private:
//  void __functionLLS(reals_t &parameter, reals_t &data, int parameterLength, int dataLength, void *adata);

protected:
  //Temporary member for the optimization routine.
//  qreal *_workingXValues;
};

class FittingLevenbergMarquardt: public FittingLinearLeastSquare {
  SUPER(FittingLevenbergMarquardt,FittingLinearLeastSquare)
public:
  FittingLevenbergMarquardt() {
  }
//  FittingLevenbergMarquardt(const ApproximationTools::FittingLevenbergMarquardt& other) : ApproximationTools::FittingLinearLeastSquare(other)
//  {
//  }

public:
  //overload operators
//  FittingLevenbergMarquardt& operator=(const ApproximationTools::FittingLevenbergMarquardt &rhs)
//  {
//    this->_copyInterface(rhs);
//    this->_workingXValues = NULL;
//    return *this;
//  }

protected:
  void _approximation(qreal *parameter /*IO*/,
                              qreal *parameterLimitMax /*I*/,
                              qreal *parameterLimitMin /*I*/,
                              qreal *parameterError /*O*/,
                              uint numberOfParameter /*I*/,
                              qreal *yValues /*I*/,
                              uint numberOfDataPoints /*I*/);

private:
  void __functionLM(qreal *parameter, qreal *data, int parameterLength, int dataLength, void *adata);
  void __functionJacobianLM(qreal *parameter, qreal *jacobian, int parameterLength, int dataLength, void *adata);
};

}

#endif
