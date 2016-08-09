// ************************************************************************** //
//
//  Steca2:    StressTextureCalculator ver. 2
//
//! @file      fit_methods.h
//! @brief     Fitting methods
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef FIT_METHODS_H
#define FIT_METHODS_H

#include "fit/fit_fun.h"

namespace fit {
//------------------------------------------------------------------------------

class Method {
public:
  Method();
  virtual ~Method();

  void fit(typ::Function&, typ::Curve::rc);

protected:
  virtual void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                           qreal const*, uint) = 0;

  // these pointers are valid during fit() call
  typ::Function* function_;
  qreal const*   xValues_;

protected:
  // calculate a vector of y(x)
  void callbackY(qreal*, qreal*, int, int, void*);
};

//------------------------------------------------------------------------------

class LinearLeastSquare : public Method {
  CLS(LinearLeastSquare) SUPER(Method)
public:
  LinearLeastSquare();

protected:
  void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                   qreal const*, uint);
};

//------------------------------------------------------------------------------

class LevenbergMarquardt : public Method {
  CLS(LevenbergMarquardt) SUPER(Method)
public:
  LevenbergMarquardt();

protected:
  void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                   qreal const*, uint);
private:
  void callbackJacobianLM(qreal*, qreal*, int, int, void*);
};

//------------------------------------------------------------------------------
}
#endif
