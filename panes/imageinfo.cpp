#include "imageinfo.h"

ImageInfo::ImageInfo(): super("ImageInfo") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);
}

// eof
