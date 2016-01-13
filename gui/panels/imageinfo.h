#ifndef IMAGEINFO_H
#define IMAGEINFO_H

#include "panel.h"
#include <QTreeWidget>

namespace panel {

class ImageInfo: public BoxPanel {
  SUPER(ImageInfo,BoxPanel)
public:
  ImageInfo(MainWin&);
};

}

#endif
