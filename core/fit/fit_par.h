//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_par.h
//! @brief     Defines classes FitParameter
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FIT_PAR_H
#define FIT_PAR_H

#include "core/typ/range.h"

//! One fit parameter, with value, error, and allowed range.

class FitParameter {
public:
    FitParameter();

    void setValue(double value, double error);
    void reset();
    void setAllowedRange(double min, double max);

    double value() const { return value_; }
    double error() const { return error_; }
    const Range& range() const;

private:
    double value_, error_;
    Range range_; //!< allowed range of values
};

#endif // FIT_PAR_H
