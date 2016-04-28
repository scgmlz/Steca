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
#include "views.h"
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

  views::ReflectionView *reflectionView_;

  QRadioButton *rbPoints_, *rbInterpolated_;
  QGroupBox    *moreParams_;
  QDoubleSpinBox *stepAlpha, *stepBeta;
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& hub): super("", hub, Qt::Vertical) {
  auto hb = hbox();
  box_->addLayout(hb);

  hb->addWidget(label("β step"));
  hb->addWidget((stepBeta = spinCell(8,1.,30.)));
  hb->addWidget((reflectionView_ = new views::ReflectionView(hub_)));
  hb->addStretch();

  hb = hbox();
  box_->addLayout(hb);

  hb->addWidget((rbPoints_ = radioButton("points")));
  hb->addWidget((rbInterpolated_ = radioButton("interpolated")));
  hb->addStretch();

  box_->addWidget((moreParams_ = new QGroupBox()));
  auto mp = gridLayout();
  moreParams_->setLayout(mp);

  mp->addWidget(label("α step"),0,0);
  mp->addWidget((stepAlpha = spinCell(8,1.,30.)),0,1);

  connect(rbPoints_, &QRadioButton::clicked, [this]() {
    moreParams_->hide();
  });

  connect(rbInterpolated_, &QRadioButton::clicked, [this]() {
    moreParams_->show();
  });

  stepAlpha->setValue(5);
  stepBeta->setValue(5);
  rbPoints_->click();

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

  auto reflection = params_->reflectionView_->selectedReflection();
  if (!reflection)
    return;

  rs = hub_.reflectionInfos(*reflection, betaStep);

  if (params_->moreParams_->isHidden()) {
    // points
  } else {
    // interpolated
    rs = core::pole::interpolate(rs,alphaStep,betaStep,10,10,10,.8);
  }

  for (auto const& r: rs)
    table.addRow({r.alpha(), r.beta(), r.rgeGamma().min, r.rgeGamma().max,
                  r.inten(), r.tth(), r.fwhm() });
}

//------------------------------------------------------------------------------
}}
// eof
