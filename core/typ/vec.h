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

// TODO rm for good

template <typename T> class vec : public QVector<T> {
private:
    using super = QVector<T>;
public:
    vec() : super() {}
    vec(std::initializer_list<T> l) : super(l) {}

    explicit vec(int count) : super(count) {}
    explicit vec(int count, const T& init) : super(count, init) {}

    const super& sup() const { return *this; }

    vec& fill(const T& init) { return static_cast<vec&>(super::fill(init)); }

    vec& fill(const T& init, int count) {
        return static_cast<vec&>(super::fill(init, count));
    }

};

#endif // VEC_H
