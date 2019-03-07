//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/double_with_error.h
//! @brief     Defines class DoubleWithError
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DOUBLE_WITH_ERROR_H
#define DOUBLE_WITH_ERROR_H

//! One double value with error estimate. Used to hold fitted parameter.

class DoubleWithError {
public:
    DoubleWithError() = delete;
    DoubleWithError(double value, double error);
    DoubleWithError(const DoubleWithError&) = default; // TODO delete;
    DoubleWithError(DoubleWithError&&) = default;

    double value() const { return value_; }
    double error() const { return error_; }
    double roundedError(int) const;

private:
    const double value_;
    const double error_;
};

#endif // DOUBLE_WITH_ERROR_H
