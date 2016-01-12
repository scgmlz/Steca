#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_defs.h"
#include <QSize>
#include <QVector>

namespace core {

class Image {
public:
  Image(QSize const&,int const* intensities) THROWS;

  QSize    const& getSize()        const { return size; }
  uint            dataCount()      const { return size.width() * size.height(); }

  int intensity(uint index)        const;
  int intensity(uint x, uint y)    const;
  int maximumIntensity()           const { return maxIntensity; }

private:
  QSize        size;
  QVector<int> intensities;
  int          maxIntensity;
};

}

#endif
