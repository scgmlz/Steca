#include "detector.h"
#include "gui_helpers.h"

namespace panel {

Detector::Detector(MainWin& mainWin): super(mainWin,"Detector",Qt::Vertical) {
  auto g = gridLayout();
  box->addLayout(g);

  g->addWidget(label("Detector dist."),0,0);
  g->addWidget(editCell(),0,1);
  g->addWidget(label("mm"),0,2);

  g->addWidget(label("Pixel size"),1,0);
  g->addWidget(editCell(),1,1);
  g->addWidget(label("mm"),1,2);

  box->addWidget(check("Beamcenter offset"));
  auto h = hbox();
  box->addLayout(h);
  h->addWidget(label("X"));
  h->addWidget(editCell());
  h->addWidget(label("Y"));
  h->addWidget(editCell());
  h->addWidget(label("pix"));
}

}

// eof
