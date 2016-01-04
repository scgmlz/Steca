#include "panel_detector.h"
#include "gui_helpers.h"

PanelDetector::PanelDetector(MainWin& mainWin): super(mainWin,Qt::Vertical,"Detector") {
  auto g = grid();
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

// eof
