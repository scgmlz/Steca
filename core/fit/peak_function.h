//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/peak_function.h
//! @brief     Defines class PeakFunction
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef PEAK_FUNCTION_H
#define PEAK_FUNCTION_H

#include "core/fit/fit_function.h"
#include "core/fit/fitted.h"
#include <QString>

class RawOutcome;
class PeakOutcome;

//! Abstract peak function

class PeakFunction : public FitFunction {
public:
    virtual PeakOutcome outcome(const Fitted&) const;
    virtual int nPar() const { return 3; }

    static Fitted fromFit(const QString&, const Curve&, const RawOutcome&);
};

#endif // PEAK_FUNCTION_H
