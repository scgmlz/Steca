#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Image::Image(uint size_, intensity_t const* src) THROWS
: size(size_), maxIntensity(0) {

  RUNTIME_CHECK(size > 0,"Invalid image size");

  int count = size * size;
  intensities.resize(count);

  auto dest = intensities.data();
  while(count-- > 0)
    maxIntensity = qMax(*dest++ = *src++, maxIntensity);
}

uint Image::index(Session const& session, uint x, uint y) const {
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

  return x + y * size;
}

Image::intensity_t Image::intensity(uint index) const {
  return intensities[index];
}

Image::intensity_t Image::intensity(Session const& session,uint x, uint y) const {
  return intensities[index(session,x,y)];
}

}

// eof
