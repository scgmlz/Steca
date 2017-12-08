// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/fit/fit_methods.h
//! @brief     Defines class FitWrapper
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

class FitWrapper {
public:
    void fit(typ::Function&, typ::Curve const&);

private:
    // these pointers are valid during fit() call
    typ::Function* function_;
    qreal const* xValues_;

    void approximate(qreal*, qreal const*, qreal const*, qreal*, uint, qreal const*, uint);

    void callbackY(qreal*, qreal*, int, int, void*);
    void callbackJacobianLM(qreal*, qreal*, int, int, void*);
};

} // namespace fit

#endif
