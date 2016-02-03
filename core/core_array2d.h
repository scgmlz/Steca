/** \file
 * 2D array.
 */

#ifndef CORE_ARRAY2D_H
#define CORE_ARRAY2D_H

#include "core_defs.h"
#include <QSize>

namespace core {

template<typename T>
class Array2D {
public:
  Array2D() {
  }

  /// 2D image size
  QSize const& getSize() const {
    return size;
  }

  /// number of elements
  uint getCount() const {
    return size.height() * size.width();
  }

  /// make empty
  void clear() {
    fill(T(),QSize());
  }

  /// allocate with a value
  void fill(T val, QSize const& size_) {
    size = size_;
    ts.fill(val,getCount());
  }

  /// allocate with a default value
  void fill(QSize const& size) {
    fill(T(),size);
  }

  /// Calculate the 1D index of a pixel.
  uint index(uint x, uint y) const {
    return x + y * size.width();
  }

  /// Access
  T const& at(uint x,uint y) const {
    return ts.at(index(x,y));
  }

  /// Access
  void setAt(uint x, uint y, T const& val) {
    ts[index(x,y)] = val;
  }

  /// Raw access
  T* getData() {
    return ts.data();
  }

  /// Raw access
  T const* getData() const {
    return ts.data();
  }

protected:
  QSize size;
  QVector<T> ts;
};

}

#endif
