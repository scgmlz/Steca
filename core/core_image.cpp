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

Image::Transform Image::Transform::nextRotate() const {
  return rotateTo(val+1);
}

Image::Image(uint size_, intens_t const* src): size(0) {
  fill(0,size_);
  addIntensities(src);
}

void Image::clear() {
  fill(0,0);
  rgeIntens.invalidate();
}

void Image::fill(Image::intens_t defValue, uint size_) {
  size = size_;

  intensities.fill(defValue,getCount());
  rgeIntens.set(defValue);
}

uint Image::getCount() const {
  return size * size;
}

uint Image::index(Transform transform, uint x, uint y) const {
  auto flip = [this](uint &index) { index = size - 1 - index; };

  switch (transform.val) {
  case Transform::ROTATE_0:
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
  case Transform::MIRROR_ROTATE_0:
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
  rgeIntens.invalidate();
}

void Image::addIntensities(intens_t const* src) {
  if (src) {
    auto data  = intensities.data();
    uint count = getCount();
    while(count-- > 0) *data++ += *src++;
    rgeIntens.invalidate();
  }
}

Range const& Image::getRgeIntens() const {
  if (rgeIntens.isInvalid()) {
    auto data    = intensities.data();
    uint count   = getCount();
    while(count-- > 0) rgeIntens.extend(*data++);
  }

  return rgeIntens;
}

}

// eof
