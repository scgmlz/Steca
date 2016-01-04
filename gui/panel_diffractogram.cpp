#include "panel_diffractogram.h"
#include "../3rd/qcustomplot.h"

PanelDiffractogram::PanelDiffractogram(MainWin& mainWin)
: super(mainWin,Qt::Vertical,"Diffractogram") {
  box->addWidget(check("From all images"));
  box->addWidget(new QCustomPlot());
}

// eof
