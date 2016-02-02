#include "core_image.h"
#include "core_session.h"
#include <QPixmap>

namespace core {

Image::Transform::Transform(int val_): val((e)(val_ & 7)) {
}

Image::Transform Image::Transform::mirror(bool on) const {
  return on ? Transform(val |  MIRROR)
            : Transform(val & ~MIRROR);
}

Image::Transform Image::Transform::rotateTo(Transform rot) const {
  return Transform((val & MIRROR) | (rot.val & 3));
}

Image::Transform Image::Transform::rotate() const {
  return rotateTo(val+1);
}

Image::Image(uint size_, intens_t const* src): size(0) {
  fill(0,size_);
  addIntensities(src);
}

void Image::clear() {
  fill(0,0);
  intIntens.clear();
}

void Image::fill(Image::intens_t defValue, uint size_) {
  size = size_;

  uint count = size * size;
  intensities.fill(defValue,count);
  intIntens.set(defValue);
}

uint Image::index(Transform transform, uint x, uint y) const {
  auto flip = [this](uint &index) { index = size - 1 - index; };

  switch (transform.val) {
  case Transform::NONE:
    break;
  case Transform::ROTATE_1:
    qSwap(x,y); flip(y);
    break;
  case Transform::ROTATE_2:
    flip(x); flip(y);
    break;
  case Transform::ROTATE_3:
    qSwap(x,y); flip(x);
    break;
  case Transform::MIRROR:
    flip(x);
    break;
  case Transform::MIRROR_ROTATE_1:
    qSwap(x,y); flip(x); flip(y);
    break;
  case Transform::MIRROR_ROTATE_2:
    flip(y);
    break;
  case Transform::MIRROR_ROTATE_3:
    qSwap(x,y);
    break;
  }

  return index(size,x,y);
}

Image::intens_t const& Image::intensity(Transform transform,uint x, uint y) const {
  return intensities[index(transform,x,y)];
}

void Image::setIntensity(Transform transform,uint x, uint y, intens_t val) {
  intensities[index(transform,x,y)] = val;
  intIntens.clear();
}

void Image::addIntensities(intens_t const* src) {
  if (src) {
    auto data  = intensities.data();
    uint count = size * size;
    while(count-- > 0) *data++ += *src++;
    intIntens.clear();
  }
}

Interval const& Image::getIntIntens() const {
  if (intIntens.isClear()) {
    auto data    = intensities.data();
    uint count   = size * size;
    while(count-- > 0) intIntens.extend(*data++);
  }

  return intIntens;
}

}

// eof
