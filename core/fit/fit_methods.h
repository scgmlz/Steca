// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/fit/fit_methods.h
//! @brief     Defines classes fit::{Method,  LevenbergMarquardt}
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FIT_METHODS_H
#define FIT_METHODS_H

#include "fit_fun.h"

namespace fit {

class Method {
public:
    void fit(typ::Function&, typ::Curve const&);

protected:
    virtual void
    approximate(qreal*, qreal const*, qreal const*, qreal*, uint, qreal const*, uint) = 0;

    // these pointers are valid during fit() call
    typ::Function* function_;
    qreal const* xValues_;

protected:
    // calculate a vector of y(x)
    void callbackY(qreal*, qreal*, int, int, void*);
};

class LevenbergMarquardt : public Method {
private:
    using super = Method;
protected:
    void approximate(qreal*, qreal const*, qreal const*, qreal*, uint, qreal const*, uint);

private:
    void callbackJacobianLM(qreal*, qreal*, int, int, void*);
};

} // namespace fit

#endif
