#include "core_lens.h"

#include "core_image.h"

namespace core {
//------------------------------------------------------------------------------

void Lens::nextChangedImpl() {
}

//------------------------------------------------------------------------------

PlainLens::PlainLens(Image const& image)
  : rawImage(&image)
{
}

uint PlainLens::getPriority() const {
  return PRIORITY;
}

intens_t PlainLens::getIntensity(uint x, uint y) const {
  return rawImage->at(x, y);
}

QSize PlainLens::getSize() const {
  return rawImage->getSize();
}

//------------------------------------------------------------------------------

TransformationLens::TransformationLens(ImageTransform const& transformation)
  : transform(transformation)
{
}

uint TransformationLens::getPriority() const {
  return PRIORITY;
}

intens_t TransformationLens::getIntensity(uint x, uint y) const {
  auto s = getSize();
  const uint w = s.width();
  const uint h = s.height();

  switch (transform.val) {
  case ImageTransform::ROTATE_0:
    break;
  case ImageTransform::ROTATE_1:
    std::swap(x, y);
    y = w - y - 1;
    break;
  case ImageTransform::ROTATE_2:
    x = w - x - 1;
    y = h - y - 1;
    break;
  case ImageTransform::ROTATE_3:
    std::swap(x, y);
    break;
  case ImageTransform::MIRROR_ROTATE_0:
    x = w - x - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_1:
    y = h - y - 1;
    std::swap(x, y);
    y = w - y - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_2:
    y = h - y - 1;
    break;
  case ImageTransform::MIRROR_ROTATE_3:
    y = h - y - 1;
    std::swap(x, y);
    break;
  default:
    NEVER_HERE;
  }

  return next->getIntensity(x, y);
}


QSize TransformationLens::getSize() const {
  auto s = next->getSize();
  switch (transform.val) {
  case ImageTransform::ROTATE_0:
  case ImageTransform::ROTATE_2:
  case ImageTransform::MIRROR_ROTATE_0:
  case ImageTransform::MIRROR_ROTATE_2:
    return s;
  case ImageTransform::ROTATE_1:
  case ImageTransform::ROTATE_3:
  case ImageTransform::MIRROR_ROTATE_1:
  case ImageTransform::MIRROR_ROTATE_3:
    s.transpose();
    return s;
  default:
    NEVER_HERE return QSize();
  }
}

//------------------------------------------------------------------------------

ROILens::ROILens(ImageCut const& imageCut)
  : cut(imageCut)
{
}

uint ROILens::getPriority() const {
  return PRIORITY;
}

intens_t ROILens::getIntensity(uint x, uint y) const {
  x += cut.left;
  y += cut.top;
  return next->getIntensity(x, y);
}

QSize ROILens::getSize() const {
  auto s = next->getSize();
  s.rwidth() -= cut.left + cut.right;
  s.rheight() -= cut.top + cut.bottom;
  return s;
}

//------------------------------------------------------------------------------

shp_LensSystem makeLensSystem(Image const& image) {
    return shp_LensSystem(new PlainLens(image));
}

//------------------------------------------------------------------------------
}

// eof
