// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      type_vec.h
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

#ifndef TYPE_VEC_H
#define TYPE_VEC_H

#include "types_inc_macros.h"
#include <QVector>
#include <initializer_list>

/* The derived classes control access to Qt classes:
 * - remove duplicities (count/size)
 * - use uint instead of int
 */

//------------------------------------------------------------------------------

template <typename T>
class vec : protected QVector<T> {
  SUPER(vec, QVector<T>)
public:
  super const& q() const { return *this; }  // access the Qt super class

  vec()                                   : super()                 {}
  vec(std::initializer_list<T> l)         : super(l)                {}

  explicit vec(uint count)                : super(int(count))       {}
  explicit vec(uint count, T const& init) : super(int(count), init) {}

  uint count() const   { return uint(super::count()); }
  void reserve(uint n) { super::reserve(int(n));      }

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
    return static_cast<vec&>(super::fill(init, int(count)));
  }

  void resize(uint count)       { super::resize(int(count));        }
  void append(T   const& that)  { *this += that;                    }  // append introduced in Qt 5.5
  void append(vec const& that)  { *this += that;                    }  // append introduced in Qt 5.5
  void remove(uint i)           { super::remove(int(i));            }

  T const& at(uint i) const     { return super::at(int(i));         }
  T& operator[](uint i)         { return super::operator[](int(i)); }
};

typedef vec<qreal> qreal_vec;
typedef vec<uint>  uint_vec;

//------------------------------------------------------------------------------
#endif  // TYPE_VEC_H
