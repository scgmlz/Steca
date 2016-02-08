#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Image::Image(QSize const& size, intens_t const* src) {
  fill(0,size);
  addIntensities(src);
}

void Image::clear() {
  super::clear();
  rgeIntens.invalidate();
}

void Image::fill(intens_t val, QSize const& size) {
  super::fill(val,size);
  rgeIntens.set(val);
}

void Image::setIntensity(uint i, intens_t val) {
  super::setAt(i,val);
  rgeIntens.invalidate();
}

void Image::addIntensities(intens_t const* src) {
  if (src) {
    auto data  = getData();
    uint count = getCount();
    while(count-- > 0) *data++ += *src++;
    rgeIntens.invalidate();
  }
}

Range const& Image::getRgeIntens() const {
  if (rgeIntens.isInvalid()) {
    auto data    = getData();
    uint count   = getCount();
    while(count-- > 0) rgeIntens.extend(*data++);
  }

  return rgeIntens;
}

}

// eof
