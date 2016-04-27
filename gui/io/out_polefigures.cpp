// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_polefigures.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "out_polefigures.h"
#include "out_table.h"
#include "panels/panel.h"
#include "thehub.h"
#include "core_polefigure.h"

#ifdef STECA_LABS
#include "gl/gl_canvas.h"
#endif

namespace gui { namespace io {
//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);

  QDoubleSpinBox *stepAlpha, *stepBeta;
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& hub): super("", hub, Qt::Horizontal) {
  box_->addWidget(label("α"));
  box_->addWidget((stepAlpha = spinCell(8,1.,30.)));
  box_->addWidget(label("β"));
  box_->addWidget((stepBeta = spinCell(8,1.,30.)));
  box_->addStretch();

  stepAlpha->setValue(5);
  stepBeta->setValue(5);
}

//------------------------------------------------------------------------------

OutPoleTabs::OutPoleTabs(TheHub& hub): super(hub) {
  {
    addTab("Table");
  }
  {
    addTab("Graph");
  }
#ifdef STECA_LABS
  {
    addTab("Sphere");
  }
#endif
}

//------------------------------------------------------------------------------

OutPoleFigures::OutPoleFigures(TheHub& hub,rcstr title,QWidget* parent)
: super(hub,title,parent) {
  params_ = new OutPoleFiguresParams(hub);
  auto *tabs = new OutPoleTabs(hub);
  setWidgets(params_,tabs);

  tableWidget_ = new OutTableWidget(hub,
      {"α","β","γ1","γ2","inten","2θ","fwhm"},
      {cmp_real,cmp_real,cmp_real,cmp_real,cmp_real,cmp_real,cmp_real});

  tabs->tab(0).box->addWidget(tableWidget_);

#ifdef STECA_LABS
  auto *canvas = new gl::Canvas();
  tabs->tab(2).box->addWidget(canvas);
  auto *camera = new gl::Camera();
  canvas->setCamera(camera);
#endif
}

void OutPoleFigures::calculate() {
  auto &table = tableWidget_->table();
  table.clear();

  qreal alphaStep = params_->stepAlpha->value();
  qreal betaStep  = params_->stepBeta->value();
  auto rs1 = hub_.reflectionInfos(betaStep);
  auto rs2 = core::pole::interpolate(rs1,alphaStep,betaStep,10,10,10,.8);

  for (core::ReflectionInfo const& r: rs2)
    table.addRow({r.alpha(), r.beta(), r.rgeGamma().min, r.rgeGamma().max,
                  r.inten(), r.tth(), r.fwhm() });
}

//------------------------------------------------------------------------------
}}
// eof
