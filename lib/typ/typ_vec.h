/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#ifndef TYP_VEC_H
#define TYP_VEC_H

#include "def/def_macros.h"
#include "def/def_gsl.h"
#include <QVector>
#include <initializer_list>

namespace typ {
//------------------------------------------------------------------------------

template <typename T>
class vec : protected QVector<T> {
  CLS(vec) WITH_SUPER(QVector<T>)
public:

  vec()                                   : super()  {}
  vec(std::initializer_list<T> l)         : super(l) {}

  explicit vec(uint count)                : super(to_i(count))       {}
  explicit vec(uint count, T const& init) : super(to_i(count), init) {}

  uint  count() const   { return to_u(super::count()); }
  void  reserve(uint n) { super::reserve(to_i(n));     }

  using super::clear;
  using super::isEmpty;
  using super::begin;
  using super::end;
  using super::cbegin;
  using super::cend;
  using super::data;
  using super::constData;
  using super::first;
  using super::last;

  vec& fill(T const& init) {
    return static_cast<vec&>(super::fill(init));
  }

  vec& fill(T const& init, uint count) {
    return static_cast<vec&>(super::fill(init, to_i(count)));
  }

  void resize(uint count)       { super::resize(to_i(count));        }
  void append(T   const& that)  { *this += that;                     }
  void append(vec const& that)  { *this += that;                     }
  void remove(uint i)           { super::remove(to_i(i));            }

  T const& at(uint i) const     { return super::at(to_i(i));         }
  T& operator[](uint i)         { return super::operator[](to_i(i)); }
};

//------------------------------------------------------------------------------
}

// most useful vectors (that's why they are in the global namespace)
typedef typ::vec<qreal> qreal_vec;
typedef typ::vec<uint>  uint_vec;

#endif // TYP_VEC_H
