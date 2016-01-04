#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include "panel.h"
#include <QTreeWidget>

namespace panel {

class ImageInfo: public Panel {
  SUPER(ImageInfo,Panel)
public:
  ImageInfo(MainWin&);
};

}

#endif
