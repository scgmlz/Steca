#include "diffractogram.h"
#include <QCheckBox>
#include "../3rd/qcustomplot.h"

Diffractogram::Diffractogram(): super("Diffractogram") {
  auto sp = sizePolicy();
  sp.setVerticalStretch(1);
  setSizePolicy(sp);

  auto v = new VBox;
  setLayout(v);

  v->addWidget(new QCheckBox("From all images"));
  v->addWidget(new QCustomPlot());
}

// eof
