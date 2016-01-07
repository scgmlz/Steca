#include "image.h"
#include "debug.h"

namespace core {

Image::Image(QSize const& size_,type const* intensities_) THROWS
: size(size_) {

  CHECK(size.isValid(),"Invalid image size")

  int count = size.width() * size.height();
  intensities.resize(count);
  memcpy(intensities.data(),intensities_,count * sizeof(type));
}

}

// eof
