// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/fun.h
//! @brief     Defines classes Function, SimpleFunction
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FUN_H
#define FUN_H

#include "def/special_pointers.h"
#include "typ/json.h"
#include "typ/range.h"

namespace typ {

//! Abstract function

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

    virtual uint parameterCount() const = 0;

    virtual Parameter& parameterAt(uint) = 0;
    virtual Parameter const& parameterAt(uint i) const {
        return const_cast<Function*>(this)->parameterAt(i);
    }

    // evaluate the function y = f(x), with given (parValues) or own parameters
    virtual qreal y(qreal x, qreal const* parValues = nullptr) const = 0;

    // partial derivative / parameter, with given (parValues) or own parameters
    virtual qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const = 0;

    virtual JsonObj to_json() const { return JsonObj(); }
    virtual void from_json(JsonObj const&) THROWS {}
};


//! abstract function with parameters

class SimpleFunction : public Function {
public:
    void setParameterCount(uint);
    uint parameterCount() const;
    Parameter& parameterAt(uint);

    virtual void reset();

    JsonObj to_json() const;
    void from_json(JsonObj const&) THROWS;
    virtual str name() const = 0;

protected:
    vec<Parameter> parameters_;
    qreal parValue(uint parIndex, qreal const* parValues) const;
    void setValue(uint parIndex, qreal val);

private:
    using super = Function;
};

} // namespace typ

#endif // FUN_H
