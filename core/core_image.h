#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_defs.h"
#include <QSize>
#include <QVector>

namespace core {

class Session;

class Image {
public:
  typedef float intensity_t;

  Image(QSize const&, intensity_t const* intensities) THROWS;

  QSize const& getSize()   const { return size; }
  uint         pixCount()  const { return size.width() * size.height(); }

  // Session -> transform
  uint index(Session const&,uint x, uint y) const;

  intensity_t intensity(uint index)     const;
  intensity_t intensity(Session const&, uint x, uint y) const;
  intensity_t maximumIntensity()        const { return maxIntensity; }

private:
  QSize size;
  QVector<intensity_t> intensities;
  intensity_t          maxIntensity;
};

}

#endif
