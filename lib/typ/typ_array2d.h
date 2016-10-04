// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_array2d.h
//! @brief     2D array
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#ifndef TYP_ARRAY2D_H
#define TYP_ARRAY2D_H

#include "def/defs.h"
#include "def/def_compare.h"
#include "typ/typ_vec.h"

namespace typ {
//------------------------------------------------------------------------------

struct size2d {
  CLS(size2d)

  uint w, h;

  size2d()                 : size2d(0, 0) {}
  size2d(uint w_, uint h_) : w(w_), h(h_) {}

  bool isEmpty() const {
    return 0 == w && 0 == h;
  }

  uint count() const {
    return w * h;
  }

  int compare(rc that) const {
    COMPARE_VALUE(w)
    COMPARE_VALUE(h)
    return 0;
  }

  friend bool operator==(rc s1, rc s2) {
    return s1.w == s2.w && s1.h == s2.h;
  }

  friend bool operator!=(rc s1, rc s2) {
    return !(s1 == s2);
  }

  friend size2d operator-(rc s1, rc s2) {
    int w = to_i(s1.w) - to_i(s2.w);
    int h = to_i(s1.h) - to_i(s2.h);
    return size2d(to_u(qMax(w, 0)), to_u(qMax(h, 0)));
  }

  size2d transposed() const {
    return size2d(h, w);
  }

  size2d scaled(qreal f) const {
    f = qMax(f, .0);
    return size2d(to_u(qRound(w*f)), to_u(qRound(h*f)));
  }
};

// 2D (indexed by uint i/j) array
template<typename T> class Array2D {
public:
  // empty array
  Array2D(): size_(0,0) {
  }

  // is empty?
  bool isEmpty() const {
    return 0 == count();
  }

  // 2D image size
  size2d::rc size() const {
    return size_;
  }

  // number of elements
  uint count() const {
    return size_.count();
  }

  // make empty
  void clear() {
    fill(T(), size2d(0,0));
  }

  // allocate and fill with a value
  void fill(T const& val, size2d const& size) {
    size_ = size;  // set size first
    ts_.fill(val, count());
  }

  // allocate and fill with a default value
  void fill(size2d::rc size) {
    fill(T(), size);
  }

  // Calculate the 1D index of an element. Row by row.
  uint index(uint i, uint j) const {
    return i + j * size_.w;
  }

  // access using 1D index
  T const& at(uint i) const {
    return ts_.at(i);
  }

  // access using 2D index
  T const& at(uint i, uint j) const {
    return ts_.at(index(i,j));
  }

  // set using 1D index
  void setAt(uint i, T const& val) {
    ts_[i] = val;
  }

  // set using 2D index
  void setAt(uint i, uint j, T const& val) {
    ts_[index(i,j)] = val;
  }

  // raw access
  T* data() {
    return ts_.data();
  }

  // raw access
  T const* data() const {
    return const_cast<Array2D*>(this)->data();
  }

  // subscript operator
  T& operator[](uint i) {
    return ts_[i];
  }

  // subscript operator
  T const& operator[](uint i) const {
    return const_cast<Array2D*>(this)[i];
  }

protected:
  size2d size_;
  vec<T> ts_;
};

//------------------------------------------------------------------------------
}
#endif // TYP_ARRAY2D_H
