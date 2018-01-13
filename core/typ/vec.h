// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/vec.h
//! @brief     Defines and implements templated class vec
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef VEC_H
#define VEC_H

#include "core/def/numbers.h"

//! limited QVector, only needed methods reexported

template <typename T> class vec : protected QVector<T> {
private:
    using super = QVector<T>;
public:
    vec() : super() {}
    vec(std::initializer_list<T> l) : super(l) {}

    explicit vec(int count) : super(count) {}
    explicit vec(int count, T const& init) : super(count, init) {}

    int count() const { return to_u(super::count()); }
    void reserve(int n) { super::reserve(n); }

    super const& sup() const { return *this; }
    using super::clear;
    using super::isEmpty;
    using super::begin;
    using super::end;
    using super::cbegin;
    using super::cend;
    using super::data;
    using super::first;

    vec& fill(T const& init) { return static_cast<vec&>(super::fill(init)); }

    vec& fill(T const& init, int count) {
        return static_cast<vec&>(super::fill(init, count));
    }

    void resize(int count) { super::resize(count); }
    void append(T const& that) { *this += that; }
    void append(vec const& that) { *this += that; }
    void remove(int i) { super::remove(i); }

    T const& at(int i) const { return super::at(i); }
    T& operator[](int i) { return super::operator[](i); }
    const T& operator[](int i) const { return super::operator[](i); }
};

// most useful vectors (that's why they are in the global namespace)
typedef vec<qreal> qreal_vec;
typedef vec<int> int_vec;

#endif // VEC_H
