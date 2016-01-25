#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Image::Image(QSize const& size_, intensity_t const* src) THROWS
: size(size_), maxIntensity(0) {

  RUNTIME_CHECK(size.isValid(),"Invalid image size");

  int count = size.width() * size.height();
  intensities.resize(count);

  auto dest = intensities.data();
  while(count-- > 0)
    maxIntensity = qMax(*dest++ = *src++, maxIntensity);
}

uint Image::index(Session const& session, uint x, uint y) const {
  if (session.turnClock) {
    qSwap(x,y); y = size.width() - 1 - y;
  }
  else if (session.turnCounter) {
    qSwap(x,y); x = size.height() - 1 - x;
  }
  if (session.upDown) {
    y = size.height() - 1 - y;
  }
  if (session.leftRight) {
    x = size.width() - 1 - x;
  }
  return x + y*size.width();
}

Image::intensity_t Image::intensity(uint index) const {
  return intensities[index];
}

Image::intensity_t Image::intensity(Session const& session,uint x, uint y) const {
  return intensities[index(session,x,y)];
}

}

// eof
