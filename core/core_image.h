#ifndef CORE_IMAGE_H
#define CORE_IMAGE_H

#include "core_defs.h"
#include <QSize>
#include <QVector>

namespace core {

class Session;

class Intensities {
public:
  typedef float intensity_t;

  Intensities(uint size = 0);
  void set(intensity_t defValue, uint size);

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

protected:
  uint size; // TODO for now a smplification: square images
  QVector<intensity_t> intensities;
};

class Image: public Intensities {
public:

  Image(uint, intensity_t const* intensities) THROWS;

  uint getSize()  const { return size; }

  intensity_t maximumIntensity() const { return maxIntensity; }

  void sumIntensities(intensity_t const*);

private:
  intensity_t maxIntensity;
};

}

#endif
