#include "images.h"
#include "mainwin.h"

namespace panel {

ImageList::ImageList() {
}

//------------------------------------------------------------------------------

Images::Images(MainWin& mainWin): super(mainWin,"Images",Qt::Vertical) {
  box->addWidget(new ImageList);
  box->addWidget(textButton(mainWin.actImagesCombine));
}

}

// eof
