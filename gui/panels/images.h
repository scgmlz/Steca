#ifndef IMAGES_H
#define IMAGES_H

#include "panel.h"

namespace panel {

class ImageList: public ListView {
  SUPER(ImageList,ListView)
public:
  ImageList();
};

class Images: public Panel {
  SUPER(Images,Panel)
public:
  Images(MainWin&);

private:
  ImageList *l;
};

}

#endif
