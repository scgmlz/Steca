/** \file
 * Functions for data fitting (was: approximation)
 */

#ifndef CORE_FIT_FUNCTIONS_H
#define CORE_FIT_FUNCTIONS_H

#include <core_types.h>

class QJsonObject;

namespace core { namespace fit {
//------------------------------------------------------------------------------
/// Abstract function

class Function {
public:
  class Parameter final {
  public:
    Parameter();

    qreal getValue() const  { return value; }
    Range getRange() const; ///< the allowed range

    void setRange(qreal min,qreal max);

    /// checks whether a new value/error pair would pass the constraints
    bool  checkValue(qreal value, qreal error=0);
    /// conditionally sets the new value/error pair
    bool  setValue(qreal value, qreal error=0, bool force=false);

  public:
    void loadFrom(QJsonObject const&) THROWS;
    void saveTo(QJsonObject&) const;

  private:
    qreal value;

    // constraints; TODO maybe not all needed?

    /// allowed range of values
    /// if !isValid() -> means the same as <value,value>, i.e. fixed value
    Range range;

    /// maximum change allowed; NaN -> no check
    qreal maxDelta;
    qreal maxDeltaPercent;

    /// maximum error allowed; NaN -> no check
    qreal maxError;
    qreal maxErrorPercent;
  };

public:
  static Function* factory(rcstr);

  Function();
  virtual ~Function();

  virtual uint parameterCount() const   = 0;

  virtual Parameter& getParameter(uint) = 0;
  virtual Parameter const& getParameter(uint i) const {
    return const_cast<Function*>(this)->getParameter(i);
  }

  /// evaluate the function y = f(x), with given (parameterValues) or own parameters
  virtual qreal y(qreal x, qreal const* parameterValues = nullptr) const = 0;

  /// partial derivative / parameter, with given (parameterValues) or own parameters
  virtual qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const = 0;

public:
  virtual void loadFrom(QJsonObject const&) THROWS;
  virtual void saveTo(QJsonObject&) const;

};

#ifndef QT_NO_DEBUG
// debug prints
QDebug& operator<<(QDebug&,Function const&);
#endif

//------------------------------------------------------------------------------
/// abstract function with parameters

class SimpleFunction: public Function {
  SUPER(SimpleFunction,Function)
public:
  SimpleFunction();

  void setParameterCount(uint);
  uint parameterCount() const;
  Parameter& getParameter(uint);

  virtual void reset();

public:
  void loadFrom(QJsonObject const&) THROWS;
  void saveTo(QJsonObject&) const;

protected:
  QVector<Parameter> parameters;
  qreal parValue(uint parIndex, qreal const* parameterValues) const;
  void  setValue(uint parIndex, qreal val);
};

//------------------------------------------------------------------------------
/// concrete function that is a sum of other functions

class SumFunctions final: public Function {
  SUPER(SumFunctions,Function)
public:
  SumFunctions();
 ~SumFunctions();

  /// add a (new) Function instance, take its ownership (for delete)
  void addFunction(Function*);

  /// aggregate parameter list for all added functions
  uint parameterCount() const;
  Parameter& getParameter(uint);

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

public:
  void loadFrom(QJsonObject const&) THROWS;
  void saveTo(QJsonObject&) const;

protected:
  /// summed functions
  QVector<Function*>  functions;
  /// the aggregate parameter list
  QVector<Parameter*> parameters;
  /// look up the original function for a given aggregate parameter index
  QVector<Function*>  function_parIndex;
  /// the starting index of parameters of a summed function, given the aggregate parameter index
  QVector<uint> firstParIndex_parIndex;

};

//------------------------------------------------------------------------------
/// a polynomial

class Polynomial: public SimpleFunction {
  SUPER(Polynomial,SimpleFunction)
public:
  Polynomial(uint degree = 0);

  uint getDegree() const;
  void setDegree(uint);

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

public:
  void loadFrom(QJsonObject const&) THROWS;
  void saveTo(QJsonObject&) const;
};

//------------------------------------------------------------------------------
/// Abstract peak function

class PeakFunction: public SimpleFunction {
  SUPER(PeakFunction,SimpleFunction)
public:
  enum eType {
    PEAK_GAUSSIAN, PEAK_LORENTZIAN, PEAK_PSEUDOVOIGT1, PEAK_PSEUDOVOIGT2,
    NUM_PEAK_TYPES
  };

  static PeakFunction* factory(eType);

  PeakFunction();

  virtual eType type() const = 0;

  virtual void  setGuessPeak(XY const&);
  virtual void  setGuessFWHM(qreal);

  XY const&     getGuessPeak()  { return guessPeak; }
  qreal         getGuessFWHM()  { return guessFwhm; }

  virtual XY    getFitPeak() = 0;
  virtual qreal getFitFWHM() = 0;

  void reset();

public:
  void loadFrom(QJsonObject const&) THROWS;
  void saveTo(QJsonObject&) const;

private:
  XY guessPeak; qreal guessFwhm;
};

//------------------------------------------------------------------------------

class Gaussian: public PeakFunction {
  SUPER(Gaussian,PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMA };

  Gaussian(qreal ampl=1, qreal xShift=0, qreal sigma=1);

  eType type() const { return PEAK_GAUSSIAN; }

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

  void  setGuessPeak(XY const&);
  void  setGuessFWHM(qreal);

  XY    getFitPeak();
  qreal getFitFWHM();

public:
  void saveTo(QJsonObject&) const;
};

//------------------------------------------------------------------------------

class CauchyLorentz: public PeakFunction {
  SUPER(CauchyLorentz,PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parGAMMA };

  CauchyLorentz(qreal ampl=1, qreal xShift=0, qreal gamma=1);

  eType type() const { return PEAK_LORENTZIAN; }

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

  void setGuessPeak(XY const&);
  void setGuessFWHM(qreal);

  XY    getFitPeak();
  qreal getFitFWHM();

public:
  void saveTo(QJsonObject&) const;
};

//------------------------------------------------------------------------------

class PseudoVoigt1: public PeakFunction {
  SUPER(PseudoVoigt1,PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMAGAMMA, parETA };

  PseudoVoigt1(qreal ampl=1, qreal xShift=0, qreal sigmaGamma=1, qreal eta=0.1);

  eType type() const { return PEAK_PSEUDOVOIGT1; }

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

  void setGuessPeak(XY const&);
  void setGuessFWHM(qreal);

  XY    getFitPeak();
  qreal getFitFWHM();

public:
  void saveTo(QJsonObject&) const;
};

//------------------------------------------------------------------------------

class PseudoVoigt2: public PeakFunction {
  SUPER(PseudoVoigt2,PeakFunction)
public:
  enum { parAMPL, parXSHIFT, parSIGMA, parGAMMA, parETA };

  PseudoVoigt2(qreal ampl=1, qreal xShift=0, qreal sigma=1, qreal gamma=1, qreal eta=0.1);

  eType type() const { return PEAK_PSEUDOVOIGT2; }

  qreal y(qreal x, qreal const* parameterValues = nullptr) const;
  qreal dy(qreal x, uint parameterIndex, qreal const* parameterValues = nullptr) const;

  void setGuessPeak(XY const&);
  void setGuessFWHM(qreal);

  XY    getFitPeak();
  qreal getFitFWHM();

public:
  void saveTo(QJsonObject&) const;
};

//------------------------------------------------------------------------------
}}
#endif
