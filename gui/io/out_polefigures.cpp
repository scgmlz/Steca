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
#include "colors.h"
#include "core_polefigure.h"
#include "core_reflection.h"
#include "types/core_angles.h"
#include <QPainter>
#include <QStringList>

namespace gui { namespace io {
//------------------------------------------------------------------------------

using deg = core::deg;
using rad = core::rad;

class PoleWidget: public QWidget {
  SUPER(PoleWidget,QWidget)
public:
  PoleWidget();
  void set(core::ReflectionInfos);
  bool     fullCircle_;

protected:
  core::ReflectionInfos rs_;
  void paintEvent(QPaintEvent*);

  QPointF p(deg alpha, deg beta) const;
  void paintGrid();
  void paintInfo();

  // valid during paintEvent
  QPainter *p_;
  QPointF   c_;
  qreal     r_;
};

PoleWidget::PoleWidget() : fullCircle_(false) {
}

void PoleWidget::set(core::ReflectionInfos rs) {
  rs_ = rs;
  update();
}

void PoleWidget::paintEvent(QPaintEvent*) {
  int w = size().width(), h = size().height();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.translate(w/2, h/2);

  p_ = &painter;
  c_ = QPointF(0,0);
  r_ = qMin(w,h) / 2;

  paintGrid();
  paintInfo();
}

QPointF PoleWidget::p(deg alpha, deg beta) const {
  qreal r = r_*alpha/90;

  rad betaRad = beta.toRad();
  return QPointF(r*cos(betaRad), -r*sin(betaRad));
}

void PoleWidget::paintGrid() {
  int alphaMax = fullCircle_ ? 180 : 90, betaMax = 360;

  for (int alpha = 10; alpha <= alphaMax; alpha +=10) {
    qreal r = r_ / 90 * alpha;
    p_->drawEllipse(c_,r,r);
  }

  for (int beta = 0; beta < betaMax; beta +=10) {
    p_->drawLine(p(10,beta), p(alphaMax,beta));
  }
}

void PoleWidget::paintInfo() {
  auto rgeMax = rs_.rgeInten().max;

  for (auto const &r: rs_) {
    qreal inten = r.inten();
    if (qIsFinite(inten)) {
      inten /= rgeMax;
      auto color = QColor(intenRgb(inten));
      p_->setPen(color); p_->setBrush(color);
      p_->drawEllipse(p(r.alpha(),r.beta()),inten*10,inten*10); // * factor just for display
    } else {
      p_->setPen(Qt::magenta); p_->setBrush(Qt::magenta);
      p_->drawEllipse(p(r.alpha(),r.beta()),5,5);
    }
  }
}

//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);

  QGroupBox       *moreParams_, *gbGammaLimit_;
  QComboBox       *reflections_;
  QRadioButton    *rbPoints_, *rbInterpolated_;
  QCheckBox       *cbFullCircle_, *cbGammaLimit_;
  QDoubleSpinBox  *stepAlpha, *stepBeta, *idwRadius_, *averagingRadius_,
                  *gammaLimitMax, *gammaLimitMin;
  QSpinBox        *treshold;
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& hub): super("", hub, Qt::Vertical) {
  auto bp = gridLayout();
  box_->addLayout(bp);

  bp->addWidget(label("β step"),0,0);
  bp->addWidget((stepBeta = spinCell(8,1.,30.)),0,1);

  str_lst ref; auto const& model = hub_.reflectionsModel;
  for_i (model.rowCount())
    ref.append(model.displayData(i));

  bp->addWidget(label("Reflection"),0,2);
  bp->addWidget(reflections_ = comboBox(ref),0,3);
  bp->setVerticalSpacing(5);

  bp->addWidget((rbPoints_ = radioButton("points")),1,0);
  bp->addWidget((rbInterpolated_ = radioButton("interpolated")),1,1);
  bp->addWidget((cbFullCircle_ = check("Full circle")),1,2);
  bp->setColumnStretch(4,1);

  box_->addWidget((moreParams_ = new QGroupBox()),2,0);
  auto mp = gridLayout();
  moreParams_->setLayout(mp);

  mp->addWidget(label("α step"),                         0,0);
  mp->addWidget((stepAlpha = spinCell(8,1.,30.)),        0,1);
  mp->setHorizontalSpacing(10);
  mp->addWidget(label("Treshold"),                       0,2);
  mp->addWidget(treshold = spinCell(4,0,100),            0,3);
  mp->addWidget(label("Averaging radius"),               1,0); // REVIEW naming
  mp->addWidget(averagingRadius_ = spinCell(8.,0.,100.), 1,3); // REVIEW reasonable limit
  mp->addWidget(label("Idw radius"),                     1,2); // REVIEW naming
  mp->addWidget(idwRadius_ = spinCell(8,0.,100.),        1,1); // REVIEW reasonable limit
  mp->setColumnStretch(4,1);

  box_->addWidget((cbGammaLimit_ = check("Gamma Limitation")), 0,0);
  box_->addWidget((gbGammaLimit_ = new QGroupBox()));
  auto gl = gridLayout();
  gbGammaLimit_->setLayout(gl);
  gbGammaLimit_->setDisabled(true);

  gl->addWidget(label("Gamma limitation max"),                   0,0);
  gl->setHorizontalSpacing(10);
  gl->addWidget(gammaLimitMax = spinCell(8,0.,100.),             0,1); // REVIEW reasonable limit
  gl->addWidget(label("Gamma limitation min"),                   1,0);
  gl->addWidget(gammaLimitMin = spinCell(8,0.,100.),             1,1); // REVIEW reasonable limit
  gl->setColumnStretch(2,1);

  connect(rbPoints_, &QRadioButton::clicked, [this]() {
    moreParams_->setDisabled(true);
  });

  connect(rbInterpolated_, &QRadioButton::clicked, [this]() {
    moreParams_->setEnabled(true);
  });

  connect(cbGammaLimit_, &QCheckBox::toggled, [this](int on) {
    gbGammaLimit_->setEnabled(on);
  });

  stepAlpha->setValue(5);
  stepBeta->setValue(5);
  treshold->setValue(100);
  averagingRadius_->setValue(0);
  idwRadius_->setValue(0);
  gammaLimitMax->setValue(0);
  gammaLimitMin->setValue(0);
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

  poleWidget_ = new PoleWidget();

  tabs->tab(0).box->addWidget(tableWidget_);
  tabs->tab(1).box->addWidget(poleWidget_);

  connect(params_->cbFullCircle_,&QCheckBox::toggled, [this,tabs] (bool on) {
    poleWidget_->fullCircle_ = on;
    tabs->tab(1).box->update();
  });
}

void OutPoleFigures::calculate() {
  auto &table = tableWidget_->table();
  table.clear();

  deg alphaStep = params_->stepAlpha->value();
  deg betaStep  = params_->stepBeta->value();

  auto index = params_->reflections_->currentIndex();
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty())
    return;

  auto reflection = reflections.at(index);
  rs_ = hub_.reflectionInfos(*reflection, betaStep);

  if (params_->rbInterpolated_->isChecked()) {
    qreal treshold  = (qreal)params_->treshold->value()/100.0;
    qreal avgRadius = params_->averagingRadius_->value();
    qreal idwRadius = params_->idwRadius_->value();
    // REVIEW imput for avgeragingAlphaMax?
    rs_ = core::pole::interpolate(rs_,alphaStep,betaStep,10,avgRadius,idwRadius,treshold);
  }

  for (auto const& r: rs_)
    table.addRow({(qreal)r.alpha(), (qreal)r.beta(), r.rgeGamma().min, r.rgeGamma().max,
                  r.inten(), (qreal)r.tth(), r.fwhm() });

  poleWidget_->set(rs_);
}

//------------------------------------------------------------------------------
}}
// eof
