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

/// 2D (x/y) array
template<typename T>
class Array2D {
public:
  /// empty array
  Array2D(): xySize(0,0) {
  }

  /// 2D image size
  QSize const& size() const {
    return xySize;
  }

  /// number of elements
  uint getCount() const {
    return xySize.height() * xySize.width();
  }

  /// make empty
  void clear() {
    fill(T(),QSize(0,0));
  }

  /// allocate and fill with a value
  void fill(T const& val, QSize const& size) {
    xySize = size;  // set size first
    ts.fill(val,getCount());
  }

  /// allocate and fill with a default value
  void fill(QSize const& size) {
    fill(T(),size);
  }

  /// Calculate the 1D index of an element. Row by row.
  uint index(uint x, uint y) const {
    return x + y * xySize.width();
  }

  /// access using 1D index
  T const& at(uint i) const {
    return ts.at(i);
  }

  /// access using 2D index
  T const& at(uint x,uint y) const {
    return ts.at(index(x,y));
  }

  /// set using 1D index
  void setAt(uint i, T const& val) {
    ts[i] = val;
  }

  /// set using 2D index
  void setAt(uint x, uint y, T const& val) {
    ts[index(x,y)] = val;
  }

  /// raw access
  T* getData() {
    return ts.data();
  }

  /// raw access
  T const* getData() const {
    return ts.data();
  }

  /// subscript operator
  T& operator[](uint i) {
    return ts[i];
  }

  /// subscript operator
  T const& operator[](uint i) const {
    return ts[i];
  }

protected:
  QSize      xySize;
  QVector<T> ts;
};

//------------------------------------------------------------------------------
}
#endif // CORE_TYPE_ARRAY2D_H
