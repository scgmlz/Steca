/*******************************************************************************
 * STeCa2 - StressTextureCalculator ver. 2
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
  CLS(Array2D)
private:
  size2d size_;

  typedef T* col_t;
  col_t* ts_;

  void alloc(size2d::rc size) {
    free();

    if (!(size_ = size).isEmpty()) {
      ts_ = static_cast<col_t*>(calloc(size_.w, sizeof(col_t*)));
      for (uint i=0; i<size_.w; ++i)
        ts_[i] = static_cast<col_t>(calloc(size_.h, sizeof(T)));
    }
  }

  void free() {
    if (ts_) {
      for (uint i=0; i<size_.w; ++i)
        ::free(ts_[i]);
      ::free(ts_); ts_ = nullptr;
    }
  }

public:
  // empty array
  Array2D(): size_(0,0), ts_(nullptr) {
  }

  Array2D(rc that): Array2D() {
    *this = that;
  }

  rc operator=(rc that) {
    alloc(that.size_);

    for (uint i=0; i<size_.w; ++i)
      for (uint j=0; j<size_.h; ++j)
        ts_[i][j] = that.ts_[i][j];

    return *this;
  }

  virtual ~Array2D() {
    free();
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
    alloc(size2d(0,0));
  }

  // allocate and fill with a value
  void fill(T const& val, size2d const& size) {
    alloc(size);
    for (uint i=0; i<size_.w; ++i)
      for (uint j=0; j<size_.h; ++j)
        ts_[i][j] = val;
  }

  // allocate and fill with a default value
  void resize(size2d::rc size) {
    fill(T(), size);
  }

  // access using 1D index
  T const& at(uint i) const {
    return at(i % size_.w, i / size_.h);
  }

  // access using 2D index
  T const& at(uint i, uint j) const {
    EXPECT(i < size_.w && j < size_.h)
    return ts_[i][j];
  }

  // set using 1D index
  void setAt(uint i, T const& val) {
    setAt(i % size_.w, i / size_.h, val);
  }

  // set using 2D index
  void setAt(uint i, uint j, T const& val) {
    EXPECT(i < size_.w && j < size_.h)
    ts_[i][j] = val;
  }

  // mutable access
  T& refAt(uint i, uint j) const {
    EXPECT(i < size_.w && j < size_.h)
    return ts_[i][j];
  }
};

//------------------------------------------------------------------------------
}
#endif // TYP_ARRAY2D_H
