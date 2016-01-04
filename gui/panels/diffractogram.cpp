#include "diffractogram.h"
#include "../3rd/qcustomplot.h"

namespace panel {

Diffractogram::Diffractogram(MainWin& mainWin)
: super(mainWin,"Diffractogram",Qt::Vertical) {

  box->addWidget(check("From all images"));
  box->addWidget(new QCustomPlot());
}

}

// eof
