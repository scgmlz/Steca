// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_fit_methods.h
//! @brief     Fitting methods
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef CORE_FIT_METHODS_H
#define CORE_FIT_METHODS_H

#include "core_fit_functions.h"

namespace core {

namespace fit {
//------------------------------------------------------------------------------

class FittingMethod {
public:
  FittingMethod();
  virtual ~FittingMethod() {}

  void fit(Function&, rcCurve);

protected:
  virtual void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                           qreal const*, uint) = 0;

  // these pointers are valid during fit() call
  Function*    function_;
  qreal const* xValues_;

protected:
  /// calculate a vector of y(x)
  void callbackY(qreal*, qreal*, int, int, void*);
};

//------------------------------------------------------------------------------

class FittingLinearLeastSquare : public FittingMethod {
  SUPER(FittingLinearLeastSquare, FittingMethod)
public:
  FittingLinearLeastSquare();

protected:
  void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                   qreal const*, uint);
};

//------------------------------------------------------------------------------

class FittingLevenbergMarquardt : public FittingMethod {
  SUPER(FittingLevenbergMarquardt, FittingMethod)
public:
  FittingLevenbergMarquardt();

protected:
  void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                   qreal const*, uint);

private:
  void callbackJacobianLM(qreal*, qreal*, int, int, void*);
};

//------------------------------------------------------------------------------
}}
#endif
