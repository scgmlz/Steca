// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
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

protected:
  core::ReflectionInfos rs_;
  void paintEvent(QPaintEvent*);

  QPointF p(deg alpha, deg beta) const;
  void circle(QPointF c, qreal r);
  void paintGrid();
  void paintInfo();

  // valid during paintEvent
  QPainter  *p_;
  QPointF    c_;
  qreal      r_, alphaMax_;
  QCheckBox *cbFullSphere_;
};

PoleWidget::PoleWidget() {
  (cbFullSphere_ = check("Full sphere"))->setParent(this);
  connect(cbFullSphere_, &QCheckBox::toggled, [this] (bool) {
    update();
  });
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
  qreal r = r_*alpha/alphaMax_;

  rad betaRad = beta.toRad();
  return QPointF(r*cos(betaRad), -r*sin(betaRad));
}

void PoleWidget::circle(QPointF c, qreal r) {
  p_->drawEllipse(c,r,r);
}

void PoleWidget::paintGrid() {
  alphaMax_ = cbFullSphere_->isChecked() ? 180 : 90;

  QPen penMajor(Qt::gray), penMinor(Qt::lightGray);

  for (int alpha = 10; alpha <= alphaMax_; alpha +=10) {
    qreal r = r_ / alphaMax_ * alpha;
    p_->setPen(!(alpha%90) ? penMajor : penMinor);
    circle(c_,r);
  }

  for (int beta = 0; beta < 360; beta +=10) {
    p_->setPen(!(beta%30) ? penMajor : penMinor);
    p_->drawLine(p(10,beta), p(alphaMax_,beta));
  }
}

void PoleWidget::paintInfo() {
  qreal rgeMax = rs_.rgeInten().max;

  for (auto const &r: rs_) {
    qreal inten = r.inten();
    if (qIsFinite(inten)) {
      inten /= rgeMax;
      auto color = QColor(intenGraph(inten));
      p_->setPen(color); p_->setBrush(color);
      circle(p(r.alpha(),r.beta()),inten * r_/60);
    }
  }
}

//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);

  QGroupBox       *gbReflection_, *gbInterpolation_, *gbGammaLimit_;
  QCheckBox       *cbInterpolated_, *cbGammaLimit_;

  QComboBox       *reflection_;
  QDoubleSpinBox  *stepAlpha_, *stepBeta_, *idwRadius_, *averagingRadius_,
                  *gammaLimitMax_, *gammaLimitMin_;
  QSpinBox        *threshold_;
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub& hub): super("", hub, Qt::Horizontal) {
  box_->addWidget((gbReflection_    = new QGroupBox()));
  box_->addWidget((gbInterpolation_ = new QGroupBox()));
  box_->addWidget((gbGammaLimit_    = new QGroupBox()));

  auto g = gridLayout();
  gbReflection_->setLayout(g);

  g->addWidget(label("Reflection"),               0,0);
  g->addWidget((reflection_ = comboBox(hub_.reflectionsModel.names())), 0,1);
  g->addWidget(label("α step"),                   1,0);
  g->addWidget((stepAlpha_ = spinCell(6,1.,30.)), 1,1);
  g->addWidget(label("β step"),                   2,0);
  g->addWidget((stepBeta_ = spinCell(6, 1.,30.)), 2,1);
  g->addWidget(label(" "),                        3,0);
  g->setColumnStretch(4,1);

  g = gridLayout();
  gbInterpolation_->setLayout(g);

  g->addWidget((cbInterpolated_ = check("Interpolated")), 0,0);
  g->addWidget(label("Averaging radius"),                 1,0); // REVIEW naming
  g->addWidget((averagingRadius_ = spinCell(8.,0.,100.)), 1,1); // REVIEW reasonable limit
  g->addWidget(label("Idw radius"),                       2,0); // REVIEW naming
  g->addWidget((idwRadius_ = spinCell(8,0.,100.)),        2,1); // REVIEW reasonable limit
  g->addWidget(label("Threshold"),                        3,0);
  g->addWidget((threshold_ = spinCell(4,0,100)),          3,1);
  g->setColumnStretch(4,1);

  g = gridLayout();
  gbGammaLimit_->setLayout(g);

  g->addWidget((cbGammaLimit_ = check("Gamma limit")),    0,0);
  g->addWidget(label("max"),                              1,0);
  g->addWidget((gammaLimitMax_ = spinCell(8,0.,100.)),    1,1); // REVIEW reasonable limit
  g->addWidget(label("min"),                              2,0);
  g->addWidget((gammaLimitMin_ = spinCell(8,0.,100.)),    2,1); // REVIEW reasonable limit
  g->addWidget(label(" "),                                3,0);
  g->setColumnStretch(4,1);

  // values

  stepAlpha_->setValue(5);
  stepBeta_->setValue(5);

  averagingRadius_->setValue(0);
  idwRadius_->setValue(0);
  threshold_->setValue(100);

  gammaLimitMax_->setValue(0);
  gammaLimitMin_->setValue(0);

  auto interpolEnable = [this] (bool on) {
    stepAlpha_->setEnabled(on);
    averagingRadius_->setEnabled(on);
    idwRadius_->setEnabled(on);
    threshold_->setEnabled(on);
  };

  auto gammaEnable = [this] (bool on) {
    gammaLimitMax_->setEnabled(on);
    gammaLimitMin_->setEnabled(on);
  };

  interpolEnable(false);
  gammaEnable(false);

  connect(cbInterpolated_, &QCheckBox::toggled, [interpolEnable](bool on) {
    interpolEnable(on);
  });

  connect(cbGammaLimit_, &QCheckBox::toggled, [gammaEnable](int on) {
    gammaEnable(on);
  });
}

//------------------------------------------------------------------------------

OutPoleFigures::OutPoleFigures(TheHub& hub,rcstr title,QWidget* parent)
: super(hub,title,parent) {
  params_ = new OutPoleFiguresParams(hub);

  auto *tabs = new panel::TabsPanel(hub);
  setWidgets(params_,tabs);

  tableData_ = new OutTableWidget(hub,
    core::ReflectionInfo::dataTags(),
    core::ReflectionInfo::dataCmps());

  poleWidget_ = new PoleWidget();

  tabs->addTab("Points").box->addWidget(tableData_);
  tabs->addTab("Graph").box->addWidget(poleWidget_);
}

void OutPoleFigures::calculate() {
  auto &table = tableData_->table();
  table.clear();

  deg alphaStep = params_->stepAlpha_->value();
  deg betaStep  = params_->stepBeta_->value();

  auto index = params_->reflection_->currentIndex();
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty())
    return;

  auto reflection = reflections.at(index);
  rs_ = hub_.makeReflectionInfos(*reflection, betaStep);

  if (params_->cbInterpolated_->isChecked()) {
    qreal treshold  = (qreal)params_->threshold_->value()/100.0;
    qreal avgRadius = params_->averagingRadius_->value();
    qreal idwRadius = params_->idwRadius_->value();
    // REVIEW gui input for averagingAlphaMax?
    rs_ = core::pole::interpolate(rs_,alphaStep,betaStep,10,avgRadius,idwRadius,treshold);
  }

  for (auto const& r: rs_)
    table.addRow(r.data());

  table.sortData();

  poleWidget_->set(rs_);
}

//------------------------------------------------------------------------------
}}
// eof
