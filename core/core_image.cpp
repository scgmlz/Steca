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

uint Image::index(Transform transform, uint x, uint y) const {
  auto flip_x = [this](uint &x) { x = size.width()  - 1 - x; };
  auto flip_y = [this](uint &y) { y = size.height() - 1 - y; };

  switch (transform.val) {
  case Transform::ROTATE_0:
    break;
  case Transform::ROTATE_1:
    qSwap(x,y); flip_y(y);
    break;
  case Transform::ROTATE_2:
    flip_x(x); flip_y(y);
    break;
  case Transform::ROTATE_3:
    qSwap(x,y); flip_x(x);
    break;
  case Transform::MIRROR_ROTATE_0:
    flip_x(x);
    break;
  case Transform::MIRROR_ROTATE_1:
    qSwap(x,y); flip_x(x); flip_y(y);
    break;
  case Transform::MIRROR_ROTATE_2:
    flip_y(y);
    break;
  case Transform::MIRROR_ROTATE_3:
    qSwap(x,y);
    break;
  }

  return super::index(x,y);
}

intens_t const& Image::intensity(Transform transform,uint x, uint y) const {
  return ts[index(transform,x,y)];
}

void Image::setIntensity(Transform transform,uint x, uint y, intens_t val) {
  ts[index(transform,x,y)] = val;
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
