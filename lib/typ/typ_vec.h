// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_vec.h
//! @brief     Veneered QT class.
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

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
  WITH_SUPER(vec, QVector<T>)
public:

  vec()                                   : super()  {}
  vec(std::initializer_list<T> l)         : super(l) {}

  explicit vec(uint count)                : super(to_i(count))       {}
  explicit vec(uint count, T const& init) : super(to_i(count), init) {}

  uint count() const   { return to_u(super::count()); }
  void reserve(uint n) { super::reserve(to_i(n));     }

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
  void append(T   const& that)  { *this += that;                    }
  void append(vec const& that)  { *this += that;                    }
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
