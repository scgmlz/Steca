#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Image::Image(uint size_, intensity_t const* src)
: size(0), maxIntensity(0) {
  fill(0,size_);
  addIntensities(src);
}

void Image::clear() {
  fill(0,0);
}

void Image::fill(Image::intensity_t defValue, uint size_) {
  size = size_;

  int count = size * size;
  intensities.fill(defValue,count);
}

uint Image::index(Session const& session, uint x, uint y) const {
  auto flip = [this](uint &index) { index = size - 1 - index; };

  switch (session.rotate) {
  case 0:
    break;
  case 1:
    qSwap(x,y); flip(y);
    break;
  case 2:
    flip(x); flip(y);
    break;
  case 3:
    qSwap(x,y); flip(x);
    break;
  default:
    NOT_HERE
  }

  if (session.mirror) {
    flip(x);
  }

  return index(size,x,y);
}

Image::intensity_t& Image::intensity(uint index) {
  return intensities[index];
}

Image::intensity_t& Image::intensity(Session const& session,uint x, uint y) {
  return intensities[index(session,x,y)];
}

Image::intensity_t const& Image::intensity(Session const& session,uint x, uint y) const {
  return intensities[index(session,x,y)];
}

void Image::addIntensities(intensity_t const* src) {
  if (src) {
    maxIntensity = 0;
    auto data = intensities.data();
    uint count = getCount();
    while(count-- > 0)
      maxIntensity = qMax((*data++ += *src++), maxIntensity);
  }
}

}

// eof
