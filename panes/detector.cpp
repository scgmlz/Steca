#include "detector.h"
#include "widgets/editcell.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QLabel>

Detector::Detector(): super("Detector") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(0);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  auto g = new Grid;
  v->addLayout(g);

  g->addWidget(new QLabel("Detector dist."),0,0);
  g->addWidget(new EditCell(),0,1);
  g->addWidget(new QLabel("mm"),0,2);

  g->addWidget(new QLabel("Pixel size"),1,0);
  g->addWidget(new EditCell(),1,1);
  g->addWidget(new QLabel("mm"),1,2);

  v->addWidget(new QCheckBox("Beamcenter offset"));
  auto h = new HBox;
  v->addLayout(h);
  h->addWidget(new QLabel("X"));
  h->addWidget(new EditCell());
  h->addWidget(new QLabel("Y"));
  h->addWidget(new EditCell());
  h->addWidget(new QLabel("pix"));
}

// eof
