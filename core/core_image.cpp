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
