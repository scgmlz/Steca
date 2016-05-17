// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_type_array2d.h
//! @brief     2D array
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPE_ARRAY2D_H
#define CORE_TYPE_ARRAY2D_H

#include "core_defs.h"
#include "core_types_fwd.h"
#include <QSize>

namespace core {
//------------------------------------------------------------------------------

/// 2D (i/j) array
template<typename T> class Array2D {
public:
  /// empty array
  Array2D(): size_(0,0) {
  }

  /// 2D image size
  QSize const& size() const {
    return size_;
  }

  /// number of elements
  uint count() const {
    return size_.width() * size_.height();
  }

  /// make empty
  void clear() {
    fill(T(),QSize(0,0));
  }

  /// allocate and fill with a value
  void fill(T const& val, QSize const& size) {
    size_ = size;  // set size first
    ts_.fill(val,count());
  }

  /// allocate and fill with a default value
  void fill(QSize const& size) {
    fill(T(),size);
  }

  /// Calculate the 1D index of an element. Row by row.
  uint index(uint i, uint j) const {
    return i + j * size_.width();
  }

  /// access using 1D index
  T const& at(uint i) const {
    return ts_.at(i);
  }

  /// access using 2D index
  T const& at(uint i,uint j) const {
    return ts_.at(index(i,j));
  }

  /// set using 1D index
  void setAt(uint i, T const& val) {
    ts_[i] = val;
  }

  /// set using 2D index
  void setAt(uint i, uint j, T const& val) {
    ts_[index(i,j)] = val;
  }

  /// raw access
  T* data() {
    return ts_.data();
  }

  /// raw access
  T const* data() const {
    return const_cast<Array2D*>(this)->data();
  }

  /// subscript operator
  T& operator[](uint i) {
    return ts_[i];
  }

  /// subscript operator
  T const& operator[](uint i) const {
    return const_cast<Array2D*>(this)[i];
  }

protected:
  QSize      size_;
  QVector<T> ts_;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_ARRAY2D_H
