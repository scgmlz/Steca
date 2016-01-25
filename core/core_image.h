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

  Image(uint, intensity_t const* intensities) THROWS;

  uint getSize()  const { return size; }
  uint pixCount() const { return size * size; }

  // Session -> transform
  uint index(Session const&,uint x, uint y) const;

  intensity_t intensity(uint index)     const;
  intensity_t intensity(Session const&, uint x, uint y) const;
  intensity_t maximumIntensity()        const { return maxIntensity; }

private:
  uint size; // TODO for now a smplification: square images
  QVector<intensity_t> intensities;
  intensity_t          maxIntensity;
};

}

#endif
