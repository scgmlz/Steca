/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef FIT_METHODS_H
#define FIT_METHODS_H

#include "fit_fun.h"

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
  CLASS(LinearLeastSquare) SUPER(Method)
public:
  LinearLeastSquare();

protected:
  void approximate(qreal*, qreal const*, qreal const*, qreal*, uint,
                   qreal const*, uint);
};

//------------------------------------------------------------------------------

class LevenbergMarquardt : public Method {
  CLASS(LevenbergMarquardt) SUPER(Method)
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
