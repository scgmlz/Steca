#include "diffractogram.h"
#include "../3rd/qcustomplot.h"

namespace panel {

Diffractogram::Diffractogram(MainWin& mainWin,Session& session)
: super("Diffractogram",mainWin,session,Qt::Vertical) {
  box->addWidget(new QCustomPlot());
  box->addWidget(check("From all images"));
}

}

// eof
