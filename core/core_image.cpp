#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Intensities::Intensities(uint size_): size(0) {
  set(0,size_);
}

void Intensities::set(Intensities::intensity_t defValue, uint size_) {
  size = size_;

  int count = size * size;
  intensities.fill(defValue,count);
}

uint Intensities::index(Session const& session, uint x, uint y) const {
  auto flip = [this](uint &index) { index = size - 1 - index; };

  if (session.turnClock) {
    qSwap(x,y); flip(y);
  }
  else if (session.turnCounter) {
    qSwap(x,y); flip(x);
  }

  if (session.upDown) {
    flip(y);
  }

  if (session.leftRight) {
    flip(x);
  }

  return index(size,x,y);
}

Intensities::intensity_t& Intensities::intensity(uint index) {
  return intensities[index];
}

Intensities::intensity_t& Intensities::intensity(Session const& session,uint x, uint y) {
  return intensities[index(session,x,y)];
}

Intensities::intensity_t const& Intensities::intensity(Session const& session,uint x, uint y) const {
  return intensities[index(session,x,y)];
}

Image::Image(uint size, intensity_t const* src) THROWS
: Intensities(size), maxIntensity(0) {
  auto dest = intensities.data();
  uint count = size * size;
  while(count-- > 0)
    maxIntensity = qMax(*dest++ = *src++, maxIntensity);
}

void Image::sumIntensities(Intensities::intensity_t const* src) {
  ASSERT(src)
  maxIntensity = 0;
  auto dest = intensities.data();
  uint count = size * size;
  while(count-- > 0)
    maxIntensity = qMax(*dest++ = *src++, maxIntensity);
}

}

// eof
