#include "core_image.h"

namespace core {

Image::Image(QSize const& size_,int const* src) THROWS
: size(size_) {

  RUNTIME_CHECK(size.isValid(),"Invalid image size")

  int count = size.width() * size.height();
  intensities.resize(count);

  auto dest = intensities.data();
  while(count-- > 0)
    *dest++ = *src++;
}

int Image::intensity(uint index) const {
  return intensities[index];
}

int Image::intensity(uint x, uint y) const {
  return intensities[x + y*size.width()];
}

}

// eof
