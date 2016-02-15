#include "core_fitting.h"
#include "LevMar/levmar.h"

namespace core {

/*********************************/
/*** Fitting Algorithm Classes ***/
/*********************************/
//*** FittingMethods ***
FittingMethods::FittingMethods() {
//    this->_usedTime=0;
//    this->_numberOfCalls=0;
}
//FittingMethods::FittingMethods(const ApproximationTools::FittingMethods& other)
//{
//    this->_copyInterface(other);
//}
//FittingMethods& FittingMethods::operator=(const ApproximationTools::FittingMethods &rhs)
//{
//    this->_copyInterface(rhs);
//    return *this;
//}
//void FittingMethods::_copyInterface(const ApproximationTools::FittingMethods &rhs)
//{
//    //copy debug infos
//    this->_usedTime = rhs._usedTime;
//    this->_numberOfCalls = rhs._numberOfCalls;
//    //copy configuration
//    this->_approximationCompareOriginalIsUsed = rhs._approximationCompareOriginalIsUsed;
//    //copy function vectors
//    this->_functionVec = FittingMethods::_cloneFunctionVector(rhs._functionVec);
//    this->_functionVecOriginal = FittingMethods::_cloneFunctionVector(rhs._functionVecOriginal);
//    this->_functionVecWorkingCopy = FittingMethods::_cloneFunctionVector(rhs._functionVecWorkingCopy);
//}
//uint FittingMethods::getNumberOfParameter(std::vector<ApproximationTools::Function*> functionVector)
//{
//    uint numberOfParameter = 0;
//    for (uint i=0; i<functionVector.size(); i++)
//        numberOfParameter += functionVector[i]->getParameterNumber();
//    return numberOfParameter;
//}
//std::vector<ApproximationTools::Function*> FittingMethods::_cloneFunctionVector(const std::vector<ApproximationTools::Function*> &sourceVector)
//{
//    std::vector<ApproximationTools::Function*> targetVector;

//    //Copy sourceVector to targetVector
//    for (uint i=0; i<sourceVector.size(); i++)
//        targetVector.push_back(sourceVector[i]->clone());

//    return targetVector;
//}
//void FittingMethods::_cloneFunctionVector(const std::vector<ApproximationTools::Function*> &sourceVector, std::vector<ApproximationTools::Function*> &targetVector)
//{
//    //Delete targetVector
//    while (!targetVector.empty())
//    {
//        if (targetVector[targetVector.size()-1] != NULL)
//        {
//            ApproximationTools::Function *tmp = targetVector[targetVector.size()-1];
//            delete tmp;
//        }
//        targetVector.pop_back();
//    }

//    targetVector = FittingMethods::_cloneFunctionVector(sourceVector);
//    return;
//}
//bool FittingMethods::fit(Curve &curve)
//{
//    if (curve.isCurveEmpty())
//        return false;

//    return this->_fit(curve, true);
//}
bool FittingMethods::fitWithoutCheck(Curve &curve) {
  return _fit(curve, false);
}
//qreal FittingMethods::getY(qreal x)
//{
//    //Calculate y
//    qreal y=0;
//    for (uint i=0; i<this->_functionVecWorkingCopy.size(); i++)
//        y+=this->_functionVecWorkingCopy[i]->getY(x);

//    return y;
//}
//std::vector<qreal> FittingMethods::getDyda(qreal x)
//{
//    //Calculate y
//    std::vector<qreal> dyda;
//    for (uint i=0; i<this->_functionVecWorkingCopy.size(); i++)
//    {
//        std::vector<qreal> tmp = this->_functionVecWorkingCopy[i]->getDyda(x);
//        for (uint j=0; j<tmp.size(); j++)
//            dyda.push_back(tmp[j]);
//    }

//    return dyda;
//}
void FittingMethods::addFunction(Function const& function) {
  functions.append(function);
  //    this->setFunctionVecCurrentToOriginal(); ???
}

uint FittingMethods::getParameterCount() const {
  uint num = 0;
  for (auto &fun: functions)
    num += fun.getParameterCount();
  return num;
}

//bool FittingMethods::overwriteFunction(ApproximationTools::Function *function /*copy the given function*/,
//                                       uint number/*=0*/ /*index of the function that has to be overwritten*/)
//{
//    if (number < this->_functionVec.size())
//    {
//        if (this->_functionVec[number] != NULL)
//            delete this->_functionVec[number];
//        this->_functionVec[number] = NULL;
//        this->_functionVec[number] = function->clone();
//        this->setFunctionVecCurrentToOriginal();
//        return true;
//    }
//    return false;
//}

//Function FittingMethods::getFunction(uint number/*=0*/ /*index of the function that will be copied*/) {
//    if (number < this->_functionVec.size())
//        return this->_functionVec[number]->clone();
//    return NULL;
//}
//bool FittingMethods::deleteLastFunction()
//{
//    if (this->_functionVec.empty())
//        return false;

//    if (this->_functionVec[this->_functionVec.size()-1] != NULL)
//        delete this->_functionVec[this->_functionVec.size()-1];
//    this->_functionVec[this->_functionVec.size()-1] = NULL;
//    this->_functionVec.pop_back();
//    this->setFunctionVecCurrentToOriginal();
//    return true;
//}
//void FittingMethods::deleteAllFunctions()
//{
//    while (!this->_functionVec.empty())
//        this->deleteLastFunction();
//}

//void FittingMethods::setFunctionVecCurrentToOriginal()
//{
//    FittingMethods::_cloneFunctionVector(this->_functionVec, this->_functionVecOriginal);
//}
//void FittingMethods::setFunctionVecOriginalToCurrent()
//{
//    FittingMethods::_cloneFunctionVector(this->_functionVecOriginal, this->_functionVec);
//}
//void FittingMethods::_createWorkingCopy()
//{
//    if (this->_approximationCompareOriginalIsUsed)
//        FittingMethods::_cloneFunctionVector(this->_functionVecOriginal, this->_functionVecWorkingCopy);
//    else
//        FittingMethods::_cloneFunctionVector(this->_functionVec, this->_functionVecWorkingCopy);
//}
//void FittingMethods::_deleteWorkingCopy()
//{
//    if (this->_functionVecWorkingCopy.empty())
//        return;

//    if (this->_functionVecWorkingCopy[this->_functionVecWorkingCopy.size()-1] != NULL)
//        delete this->_functionVecWorkingCopy[this->_functionVecWorkingCopy.size()-1];
//    this->_functionVecWorkingCopy[this->_functionVecWorkingCopy.size()-1] = NULL;
//    this->_functionVecWorkingCopy.pop_back();
//}

bool FittingLinearLeastSquare::_fit(Curve &curve, bool sideConditionCheckIsActive) {
    if (curve.isCurveEmpty())
        return false;

    //Create temporary working variables for the optimization routin.
    //The x values of the curve are give via a class member variable into the optimization.
    //Convert curve to local format (several 1D arrays)
    reals_t _workingXValues, workingYValues, toleranz;
    curve.getCurve(_workingXValues, workingYValues, toleranz);

    //Create temporary working parameter arrays
    reals_t parameter, parameterLimitMax, parameterLimitMin;
    _createParameterArrays(parameter, parameterLimitMax, parameterLimitMin);

    //Run Fit
    uint parameterCount = getParameterCount();
    _approximation(parameter.data(), parameterLimitMax.data(), parameterLimitMin.data(), parameterCount, workingYValues.data(), curve.getCurveSize());

    //Read final parameter and check the limits of the new calculated Parameters
    if (sideConditionCheckIsActive) {
      if (_readParameterArray(parameter.data()))
        return true;
    } else {
      _readParameterArrayWithoutCheck(parameter.data());
      return true;
    }

    return false;
}

void FittingLinearLeastSquare::_createParameterArrays(reals_t &parameter, reals_t &parameterMaxLimit, reals_t &parameterMinLimit) const {
  // TODO make type Parameter: val, Range
  parameter.clear(); parameterMaxLimit.clear(); parameterMinLimit.clear();
  for (auto const& fun: functions) {
    for (auto const& par: fun.getParameters()) {
      parameter.append(par.getValue());
      auto range = par.getRange();
      parameterMinLimit.append(range.min);
      parameterMaxLimit.append(range.max);
    }
  }
}

bool FittingLinearLeastSquare::_readParameterArray(qreal *parameter) {
  functions_t fs = functions;
  for (auto &f: fs) {
    for (auto &par: f.getParameters()) {
      if (!par.setValue(*parameter++))
        return false;
    }
  }

  functions = fs;
  return true;
}

void FittingLinearLeastSquare::_readParameterArrayWithoutCheck(qreal *parameter) {
  for (auto &f: functions) {
    for (auto &par: f.getParameters()) {
      par.setValue(*parameter++,true);
    }
  }
}

void FittingLinearLeastSquare::_approximation(qreal *parameter,
                                                                 qreal *parameterLimitMax,
                                                                 qreal *parameterLimitMin,
                                                                 uint numberOfParameter,
                                                                 qreal *yValues,
                                                                 uint numberOfDataPoints)
{
  reals_t covar(numberOfParameter*numberOfParameter);
  qreal info[LM_INFO_SZ];

  DelegateCalculationDbl function(this, &FittingLinearLeastSquare::__functionLLS);
//    dlevmar_bc_dif( &function					/* Function */,
//                                      parameter					/* I/O: initial parameter estimates. On output has the estimated solution */,
//                                      yValues					/* Input: measurement vector. NULL implies a zero vector */,
//                                      numberOfParameter			/* Input: parameter vector dimension (i.e. #unknowns) */,
//                                      numberOfDataPoints		/* Input: measurement point vector dimension */,
//                                      parameterLimitMin,        /* Input: vector of lower bounds. If NULL, no lower bounds apply */
//                                      parameterLimitMax,        /* Input: vector of upper bounds. If NULL, no upper bounds apply */
//                                      NULL,						/* Input: diagonal scaling constants. NULL implies no scaling */
//                                      1000						/* Input: maximum number of iterations */,
//                                      NULL,						/* Input: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3]. Respectively the scale factor for initial \mu,
//                                                                 *        stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2. Set to NULL for defaults to be used
//                                                                 */
//                                      info,						/* Output: information regarding the minimization. Set to NULL if don't care
//                                                                 * info[0]= ||e||_2 at initial p.
//                                                                 * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
//                                                                 * info[5]= # iterations,
//                                                                 * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
//                                                                 *                                 2 - stopped by small Dp
//                                                                 *                                 3 - stopped by itmax
//                                                                 *                                 4 - singular matrix. Restart from current p with increased mu
//                                                                 *                                 5 - no further error reduction is possible. Restart with increased mu
//                                                                 *                                 6 - stopped by small ||e||_2
//                                                                 *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
//                                                                 * info[7]= # function evaluations
//                                                                 * info[8]= # Jacobian evaluations
//                                                                 * info[9]= # linear systems solved, i.e. # attempts for reducing error
//                                                                 */
//                                      NULL  /* working memory: If !=NULL, it is assumed to point to a memory chunk at least LM_DER_WORKSZ(m, n)*sizeof(qreal) bytes long */,
//                                      covar /* Output: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */,
//                                      NULL  /* Pointer to possibly additional data, passed uninterpreted to func & jacf. Set to NULL if not needed. */
//                                      ); // with analytic Jacobian

//    //*** Write Error Values ***
//    //********* START **********
//    /*
//        N = number of data points
//        M = number of parameter
//        SSE = sum of squared errors (chi^2)
//        MSE = normalized sum of squared errors (SSE / N)
//        DOF = degrees of freedom (N - M)
//        SSE / DOF is better than MSE => SSE/(N - M) better as SSE/N
//        RSME = root mean square error ( sqrt(SSE/DOF) )
//        Error = sqrt( covar_ii * SSE / DOF) or sqrt( covar_ii ) * RSME
//    */
//    //The values inside the covar matrix are already multiplied by SSE and devided by DOF.
//    //Only the square root must be pulled.
//    for (uint i=0; i<numberOfParameter; i++)
//        parameterError[i] = sqrt(covar[i*numberOfParameter + i]);
//    //********** END ***********
//    //*** Write Error Values ***

//    return true;
}

void FittingLinearLeastSquare::__functionLLS(qreal *parameter, qreal *data, int parameterLength, int dataLength, void *adata)
{
//    // Option A (fastest)
//    //*******************
//    //Prepare the parameter vectors for every function
//    uint numberOfFunctions = (uint)this->_functionVecWorkingCopy.size();
//    int parameterCounter=0;
//    std::vector<std::vector<qreal> > functionParameterVector;
//    for (uint j=0; j<numberOfFunctions; j++)
//    {
//        std::vector<qreal> tempParameter;
//        for (uint k=0; k<this->_functionVecWorkingCopy[j]->getParameterNumber(); k++)
//            tempParameter.push_back(parameter[parameterCounter++]);
//        functionParameterVector.push_back(tempParameter);
//        if (parameterCounter > parameterLength)
//            return;
//    }
//    //Calculate y
//    memset(data, 0, dataLength * sizeof(qreal));
//    for (uint j=0; j<numberOfFunctions; j++)
//    {
//        if (!this->_functionVecWorkingCopy[j]->_addY(this->_workingXValues, dataLength, functionParameterVector[j], data, dataLength))
//            return;
//    }

//    //// Option B (fast)
//    ////******************
//    ////Prepare the parameter vectors for every function
//    //uint numberOfFunctions = (uint)this->_functionVecWorkingCopy.size();
//    //int parameterCounter=0;
//    //std::vector<std::vector<qreal>> functionParameterVector;
//    //for (uint j=0; j<numberOfFunctions; j++)
//    //{
//    //	std::vector<qreal> tempParameter;
//    //	for (uint k=0; k<this->_functionVecWorkingCopy[j]->getParameterNumber(); k++)
//    //		tempParameter.push_back(parameter[parameterCounter++]);
//    //	functionParameterVector.push_back(tempParameter);
//    //}
//    ////Calculate y
//    //for(int i=0; i<dataLength; i++)
//    //{
//    //	qreal y=0;
//    //	for (uint j=0; j<numberOfFunctions; j++)
//    //	{
//    //		y+=this->_functionVecWorkingCopy[j]->_getY(this->_workingXValues[i], functionParameterVector[j]);
//    //	}
//    //	data[i] = y;
//    //}

//    //// Option C (normal)
//    ////******************
//    ////Calculate y
//    //for(int i=0; i<dataLength; i++)
//    //{
//    //	int counter=0;
//    //	qreal y=0;
//    //	for (uint j=0; j<this->_functionVecWorkingCopy.size(); j++)
//    //	{
//    //		std::vector<qreal> tempParameter;
//    //		for (uint k=0; k<this->_functionVecWorkingCopy[j]->getParameterNumber(); k++)
//    //			tempParameter.push_back(parameter[counter++]);

//    //		y+=this->_functionVecWorkingCopy[j]->_getY(this->_workingXValues[i], tempParameter);
//    //	}
//    //	data[i] = y;
//    //}
}

////*** FittingLevenbergMarquardt ***
void FittingLevenbergMarquardt::_approximation( qreal *parameter /*IO*/,
                                                qreal *parameterLimitMax /*I*/,
                                                qreal *parameterLimitMin /*I*/,
                                                uint numberOfParameter /*I*/,
                                                qreal *yValues /*I*/,
                                                uint numberOfDataPoints /*I*/)
{
    reals_t covar(numberOfParameter*numberOfParameter);
    qreal info[LM_INFO_SZ], opts[LM_OPTS_SZ];
    opts[0] = LM_INIT_MU;
    opts[1] = 1e-12;
    opts[2] = 1e-12;
    opts[3] = 1e-18;

    DelegateCalculationDbl function(this, &FittingLevenbergMarquardt::__functionLM);
    DelegateCalculationDbl functionJacobian(this, &FittingLevenbergMarquardt::__functionJacobianLM);
    dlevmar_bc_der( &function					/* Function */,
      &functionJacobian			/* Function to fill the Jacobian Matrix */,
      parameter					/* I/O: initial parameter estimates. On output has the estimated solution */,
      yValues					/* Input: measurement vector. NULL implies a zero vector */,
      numberOfParameter			/* Input: parameter vector dimension (i.e. #unknowns) */,
      numberOfDataPoints		/* Input: measurement point vector dimension */,
      parameterLimitMin,        /* Input: vector of lower bounds. If NULL, no lower bounds apply */
      parameterLimitMax,        /* Input: vector of upper bounds. If NULL, no upper bounds apply */
      NULL,						/* Input: diagonal scaling constants. NULL implies no scaling */
      1000						/* Input: maximum number of iterations */,
      opts,						/* Input: minim. options [\mu, \epsilon1, \epsilon2, \epsilon3]. Respectively the scale factor for initial \mu,
                                 *        stopping thresholds for ||J^T e||_inf, ||Dp||_2 and ||e||_2. Set to NULL for defaults to be used
                                 */
      info,						/* Output: information regarding the minimization. Set to NULL if don't care
                                 * info[0]= ||e||_2 at initial p.
                                 * info[1-4]=[ ||e||_2, ||J^T e||_inf,  ||Dp||_2, mu/max[J^T J]_ii ], all computed at estimated p.
                                 * info[5]= # iterations,
                                 * info[6]=reason for terminating: 1 - stopped by small gradient J^T e
                                 *                                 2 - stopped by small Dp
                                 *                                 3 - stopped by itmax
                                 *                                 4 - singular matrix. Restart from current p with increased mu
                                 *                                 5 - no further error reduction is possible. Restart with increased mu
                                 *                                 6 - stopped by small ||e||_2
                                 *                                 7 - stopped by invalid (i.e. NaN or Inf) "func" values. This is a user error
                                 * info[7]= # function evaluations
                                 * info[8]= # Jacobian evaluations
                                 * info[9]= # linear systems solved, i.e. # attempts for reducing error
                                 */
      NULL  /* working memory: If !=NULL, it is assumed to point to a memory chunk at least LM_DER_WORKSZ(m, n)*sizeof(qreal) bytes long */,
      covar.data() /* Output: Covariance matrix corresponding to LS solution; mxm. Set to NULL if not needed. */,
      NULL  /* Pointer to possibly additional data, passed uninterpreted to func & jacf. Set to NULL if not needed. */
      ); // with analytic Jacobian
}

void FittingLevenbergMarquardt::__functionLM(qreal *parameter, qreal *data, int parameterLength, int dataLength, void*) {
//  uint numberOfFunctions = (uint)this->_functionVecWorkingCopy.size();
//  int parameterCounter=0;
//  std::vector<std::vector<qreal> > functionParameterVector;
//  for (uint j=0; j<numberOfFunctions; j++)
//  {
//    std::vector<qreal> tempParameter;
//    for (uint k=0; k<this->_functionVecWorkingCopy[j]->getParameterNumber(); k++)
//      tempParameter.push_back(parameter[parameterCounter++]);
//    functionParameterVector.push_back(tempParameter);
//    if (parameterCounter > parameterLength)
//      return;
//  }

  //Calculate y
  memset(data, 0, dataLength * sizeof(qreal));
  for_i (functions.count()) {
//    ecah workingxval x
//    data[i] += functions[i].__calculateY(x);
  }

//  for (uint j=0; j<numberOfFunctions; j++)
//  {
//    if (!this->_functionVecWorkingCopy[j]->_addY(_workingXValues, functionParameterVector[j], data))
//      return;
//  }
}

void FittingLevenbergMarquardt::__functionJacobianLM(qreal *parameter, qreal *jacobian, int parameterLength, int dataLength, void*) {
  // Option A (fastest)
  //*******************
  //Prepare the parameter vectors for every function
//  uint numberOfFunctions = (uint)this->_functionVecWorkingCopy.size();
//  int parameterCounter=0;
//  std::vector<std::vector<qreal> > functionParameterVector;
//  for (uint j=0; j<numberOfFunctions; j++)
//  {
//    std::vector<qreal> tempParameter;
//    for (uint k=0; k<this->_functionVecWorkingCopy[j]->getParameterNumber(); k++)
//      tempParameter.push_back(parameter[parameterCounter++]);
//    functionParameterVector.push_back(tempParameter);
//  }
  //Calculate dyda
  memset(jacobian, 0, dataLength * parameterLength * sizeof(qreal));
  uint counterFunctionParameterLength = 0;
//  for (uint j=0; j<numberOfFunctions; j++)
//  {
//    if (!this->_functionVecWorkingCopy[j]->_addDyda(this->_workingXValues			/*x values of the data curve*/,
//                                                    dataLength						/*number of x values*/,
//                                                    functionParameterVector[j]		/*parameter vector of all functions*/,
//                                                    counterFunctionParameterLength	/*index inside the parameter vector fir the current function*/,
//                                                    parameterCounter				/*number of all parameter of all functions*/,
//                                                    jacobian						/*jacobian matrix*/,
//                                                    dataLength * parameterLength	/*size of the jacobian matrix*/))
//      return;
//    counterFunctionParameterLength += (uint)functionParameterVector[j].size();
//  }
}

}

// eof
