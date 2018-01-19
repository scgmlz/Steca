// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/parametric_function.h
//! @brief     Defines class Function
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PARAMETRIC_FUNCTION_H
#define PARAMETRIC_FUNCTION_H

#include "core/typ/json.h"
#include "core/typ/range.h"

//! Abstract function with parameters

class Function {
public:
    class Parameter final {
    public:
        Parameter();

        qreal value() const { return value_; }
        qreal error() const { return error_; }

        Range valueRange() const; // allowed range of values
        void setValueRange(qreal min, qreal max);

        void setValue(qreal value, qreal error);

        JsonObj to_json() const;
        void from_json(JsonObj const&) THROWS;

    private:
        qreal value_, error_;
        Range range_; //!< allowed range of values
    };

    virtual ~Function() {}

    // evaluate the function y = f(x), with given (parValues) or own parameters
    virtual qreal y(qreal x, qreal const* parValues = nullptr) const = 0;

    // partial derivative / parameter, with given (parValues) or own parameters
    virtual qreal dy(qreal x, int parIndex, qreal const* parValues = nullptr) const = 0;

public:
    void setParameterCount(int);
    int parameterCount() const;
    Parameter& parameterAt(int);

    virtual void reset();

    virtual JsonObj to_json() const;
    virtual void from_json(JsonObj const&) THROWS;
    virtual str name() const = 0;

protected:
    vec<Parameter> parameters_;
    qreal parValue(int parIndex, qreal const* parValues) const;
    void setValue(int parIndex, qreal val);
};

#endif // PARAMETRIC_FUNCTION_H
