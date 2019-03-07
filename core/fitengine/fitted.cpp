//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fitengine/fitted.cpp
//! @brief     Implements class Fitted
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/fitengine/fitted.h"
#include "qcr/base/debug.h" // ASSERT
#include <qmath.h>

//  ***********************************************************************************************
//! @class Fitted

Fitted::Fitted(const FitFunction* _f,
               const std::vector<double>& _parVal, const std::vector<double>& _parErr)
        : success_ {true}
        , f_ {_f}
        , parVal_ {_parVal}
        , parErr_ {_parErr}
{
    ASSERT(parErr_.size()==parVal_.size());
}

// TODO replace by vectorial access wherever possible

double Fitted::y(const double x) const
{
    ASSERT(success_); // computing y makes only sense after a successful fit
    double ret;
    f_->setY(parVal_.data(), 1, &x, &ret);
    return ret;
}
