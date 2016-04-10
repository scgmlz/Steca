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
template<typename T>
class Array2D {
public:
  /// empty array
  Array2D(): _size(0,0) {
  }

  /// 2D image size
  QSize const& size() const {
    return _size;
  }

  /// number of elements
  uint count() const {
    return _size.width() * _size.height();
  }

  /// make empty
  void clear() {
    fill(T(),QSize(0,0));
  }

  /// allocate and fill with a value
  void fill(T const& val, QSize const& size) {
    _size = size;  // set size first
    _ts.fill(val,count());
  }

  /// allocate and fill with a default value
  void fill(QSize const& size) {
    fill(T(),size);
  }

  /// Calculate the 1D index of an element. Row by row.
  uint index(uint i, uint j) const {
    return i + j * _size.width();
  }

  /// access using 1D index
  T const& at(uint i) const {
    return _ts.at(i);
  }

  /// access using 2D index
  T const& at(uint i,uint j) const {
    return _ts.at(index(i,j));
  }

  /// set using 1D index
  void setAt(uint i, T const& val) {
    _ts[i] = val;
  }

  /// set using 2D index
  void setAt(uint i, uint j, T const& val) {
    _ts[index(i,j)] = val;
  }

  /// raw access
  T* data() {
    return _ts.data();
  }

  /// raw access
  T const* data() const {
    return const_cast<Array2D*>(this)->data();
  }

  /// subscript operator
  T& operator[](uint i) {
    return _ts[i];
  }

  /// subscript operator
  T const& operator[](uint i) const {
    return const_cast<Array2D*>(this)[i];
  }

protected:
  QSize      _size;
  QVector<T> _ts;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_ARRAY2D_H
