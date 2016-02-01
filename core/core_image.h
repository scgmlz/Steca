#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_defs.h"
#include <QSize>

namespace core {

class Session;

class Image {
public:
  typedef float intensity_t;

  Image(uint size = 0, intensity_t const* = nullptr);

  void clear();
  void fill(intensity_t defValue, uint size);

  uint getSize()  const   { return size; }
  uint getCount() const   { return size * size; }

  intensity_t* getData()  { return intensities.data(); } // TODO perhaps too open
  intensity_t const* getData() const { return intensities.data(); } // TODO perhaps too open

  // Session -> transform
  uint index(Session const&,uint x, uint y) const;
  static inline uint index(uint size,uint x, uint y) { return x + y * size; }

  intensity_t& intensity(uint index);
  intensity_t& intensity(Session const&, uint x, uint y);
  intensity_t const& intensity(Session const&, uint x, uint y) const;

  void addIntensities(intensity_t const*);

  intensity_t maximumIntensity() const {
    return maxIntensity;
  }

private:
  uint size; // TODO a simplification - square images; make rect
  QVector<intensity_t> intensities;
  intensity_t maxIntensity;
};

}

#endif
