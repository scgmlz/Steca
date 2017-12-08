// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/fun.h
//! @brief     Defines classes Function, SimpleFunction, SumFunctions
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
#include "typ/exception.h"
#include "typ/factory.h"
#include "typ/json.h"
#include "typ/range.h"
#include "typ/str.h"
#include "typ/vec.h"

namespace json_fun_key {
}

namespace typ {


//! Abstract function

class Function {
    public:
    class Factory : public typ::Factory<Function> {
private:
    using super = typ::Factory<Function>;
public:
    owner_not_null<Function*> make(JsonObj const&) THROWS;
    };

protected:
    static Factory factory_;

public:
    static void addFactoryMaker(rcstr key, owner_not_null<Factory::MakerBase*>);
    static void initFactory();

    static owner_not_null<Function*> make(JsonObj const&) THROWS;

public:
    class Parameter final {
        public:
        Parameter();

        qreal value() const { return value_; }
        qreal error() const { return error_; }

        Range valueRange() const; // allowed range of values
        void setValueRange(qreal min, qreal max);

        void setValue(qreal value, qreal error);

    public:
        JsonObj saveJson() const;
        void loadJson(JsonObj const&) THROWS;

    private:
        qreal value_, error_;

        // allowed range of values
        Range range_;
    };

public:
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

    virtual JsonObj saveJson() const;
    virtual void loadJson(JsonObj const&) THROWS;
};


//! abstract function with parameters

class SimpleFunction : public Function {
private:
    using super = Function;
public:
    void setParameterCount(uint);
    uint parameterCount() const;
    Parameter& parameterAt(uint);

    virtual void reset();

public:
    JsonObj saveJson() const;
    void loadJson(JsonObj const&) THROWS;

protected:
    vec<Parameter> parameters_;
    qreal parValue(uint parIndex, qreal const* parValues) const;
    void setValue(uint parIndex, qreal val);
};


//! concrete function that is a sum of other functions

class SumFunctions final : public Function {
private:
    using super = Function;
public:
    ~SumFunctions();

    void addFunction(owner_not_null<Function*>);

    // aggregate parameter list for all added functions
    uint parameterCount() const;
    Parameter& parameterAt(uint);

    qreal y(qreal x, qreal const* parValues = nullptr) const;
    qreal dy(qreal x, uint parIndex, qreal const* parValues = nullptr) const;

public:
    JsonObj saveJson() const;
    void loadJson(JsonObj const&) THROWS;

protected:
    // summed functions
    vec<Function*> functions_;
    // the aggregate parameter list
    vec<Parameter*> allParameters_;
    // look up the original function for a given aggregate parameter index
    vec<Function*> function4parIndex_;
    // the starting index of parameters of a summed function, given the aggregate
    // parameter index
    uint_vec firstParIndex4parIndex_;
};
}
#endif // FUN_H
