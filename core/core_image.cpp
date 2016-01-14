#include "core_image.h"
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

Image::intensity_t Image::intensity(uint index) const {
  return intensities[index];
}

Image::intensity_t Image::intensity(uint x, uint y) const {
  return intensities[x + y*size.width()];
}

QPixmap Image::pixmap(intensity_t maximumIntensity) {
  int count = dataCount();
  if (count < 1) return QPixmap();

  QSize const &size = getSize();
  uint  width = size.width(), height = size.height();
  ASSERT(width>0 && height>0) // true because count >= 1

  if (maximumIntensity <= 0) maximumIntensity = 1;  // sanity
  qreal const maximum = maximumIntensity;

  QImage image(size, QImage::Format_RGB32);

  for (uint y = 0; y < height; ++y) {
    for (uint x = 0; x < width; ++x) {
      qreal intens = (qreal)intensity(x,y) / maximum;

      QRgb rgb;
      if (intens < 0.25)
        rgb = qRgb(floor(0xff * intens * 4), 0, 0);
      else if (intens < 0.5)
        rgb = qRgb(0xff, floor(0xff * (intens - 0.25) * 4), 0);
      else if (intens < 0.75)
        rgb = qRgb(0xff - floor(0xff * (intens - 0.5) * 4), 0xff, floor(0xff * (intens - 0.5) * 4));
      else
        rgb = qRgb((int)floor(0xff * (intens - 0.75) * 4), 0xff, 0xff);

      image.setPixel(x, y, rgb);
    }
  }

  return QPixmap::fromImage(image);
}


}

// eof
