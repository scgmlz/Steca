#include "core_image_cut.h"

namespace core {
//------------------------------------------------------------------------------

ImageCut::ImageCut(uint top_, uint bottom_, uint left_, uint right_)
: top(top_), bottom(bottom_), left(left_), right(right_) {
}

bool ImageCut::operator==(ImageCut const& that) {
  return top==that.top && bottom==that.bottom && left==that.left && right==that.right;
}

uint ImageCut::getWidth(QSize const& fullSize) const {
  return qMax(fullSize.width() - (int)left - (int)right, 0);
}

uint ImageCut::getHeight(QSize const& fullSize) const {
  return qMax(fullSize.height() - (int)top - (int)bottom, 0);
}

uint ImageCut::getCount(QSize const& fullSize) const {
  return getWidth(fullSize) * getHeight(fullSize);
}

//------------------------------------------------------------------------------
}

// eof
