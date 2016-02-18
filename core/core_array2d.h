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
  using vector_t = QVector<T>;

  Array2D(): size(0,0) {
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
    fill(T(),QSize(0,0));
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
  T const& at(uint i) const {
    return ts.at(i);
  }

  T const& at(uint x,uint y) const {
    return ts.at(index(x,y));
  }

  /// Access
  void setAt(uint i, T const& val) {
    ts[i] = val;
  }

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

  /// 1D access
  T const& operator[](int i) const {
    return ts[i];
  }

protected:
  QSize    size;
  vector_t ts;
};

}

#endif
