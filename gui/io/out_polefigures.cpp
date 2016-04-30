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
#include "core_reflection.h"
#include <QPainter>
#include <QStringList>

namespace gui { namespace io {
//------------------------------------------------------------------------------

class PoleWidget: public QWidget {
  SUPER(PoleWidget,QWidget)
public:
  void set(core::ReflectionInfos);

protected:
  core::ReflectionInfos rs_;

  void paintEvent(QPaintEvent*);

  QPointF p(qreal alpha, qreal beta) const;
  void paintGrid();
  void paintInfo();

  // valid during paintEvent
  QPainter *p_;
  QPointF   c_;
  qreal     r_;
};

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

QPointF PoleWidget::p(qreal alpha, qreal beta) const {
  qreal r = r_*alpha/90;

  beta  = deg2rad(beta);
  return QPointF(r*cos(beta), -r*sin(beta));
}

void PoleWidget::paintGrid() {
  for (int alpha = 10; alpha<=90; alpha+=10) {
    qreal r = r_ / 90 * alpha;
    p_->drawEllipse(c_,r,r);
  }

  for (int beta = 0; beta<360; beta += 10) {
    p_->drawLine(p(10,beta), p(90,beta));
  }
}

void PoleWidget::paintInfo() {
  for (auto const &r: rs_) {
    qreal in = r.inten() / 16; // TODO better min-max
    if (qIsFinite(in))
      p_->drawEllipse(p(r.alpha(),r.beta()),in,in);
  }
}

//------------------------------------------------------------------------------

class OutPoleFiguresParams: public panel::BoxPanel {
  SUPER(OutPoleFiguresParams,panel::BoxPanel)
public:
  OutPoleFiguresParams(TheHub&);

  QComboBox *reflections_;
  QRadioButton *rbPoints_, *rbInterpolated_;
  QCheckBox *cbGammaLimit_;
  QGroupBox    *moreParams_, *gammaLimit_;
  QDoubleSpinBox *stepAlpha, *stepBeta, *centerSearchRadius, *searchRadius,
                 *gammaLimitMax, *gammaLimitMin;
  QSpinBox  *treshold;
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
  bp->setColumnStretch(4,1);

  box_->addWidget((moreParams_ = new QGroupBox()),2,0);
  auto mp = gridLayout();
  moreParams_->setLayout(mp);

  mp->addWidget(label("α step"),                         0,0);
  mp->addWidget((stepAlpha = spinCell(8,1.,30.)),        0,1);
  mp->setHorizontalSpacing(10);
  mp->addWidget(label("Treshold"),                       0,2);
  mp->addWidget(treshold = spinCell(4,0,100),            0,3);
  mp->addWidget(label("Radius from polefigure center"),  1,0);
  mp->addWidget(centerSearchRadius = spinCell(8,1.,30.), 1,1);
  mp->addWidget(label("Search radius"),                  1,2);
  mp->addWidget(searchRadius = spinCell(8.,1.,30.),      1,3);
  mp->setColumnStretch(4,1);

  box_->addWidget(cbGammaLimit_ = check("Gamma Limitation"), 0,0);
  box_->addWidget(gammaLimit_ = new QGroupBox());
  auto gl = gridLayout();
  gammaLimit_->setLayout(gl);
  gammaLimit_->setDisabled(true);

  gl->addWidget(label("Gamma limitation max"),                   0,0);
  gl->setHorizontalSpacing(10);
  gl->addWidget(gammaLimitMax = spinCell(8,1.,30.),              0,1);
  gl->addWidget(label("Gamma limitation min"),                   1,0);
  gl->addWidget(gammaLimitMin = spinCell(8,1.,30.),              1,1);
  gl->setColumnStretch(2,1);

  connect(rbPoints_, &QRadioButton::clicked, [this]() {
    moreParams_->setDisabled(true);
  });

  connect(rbInterpolated_, &QRadioButton::clicked, [this]() {
    moreParams_->setEnabled(true);
  });

  connect(cbGammaLimit_, &QCheckBox::toggled, [this](int on) {
    cbGammaLimit_->setChecked(on);
    gammaLimit_->setEnabled(on);
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
}

void OutPoleFigures::calculate() {
  auto &table = tableWidget_->table();
  table.clear();

  qreal alphaStep = params_->stepAlpha->value();
  qreal betaStep  = params_->stepBeta->value();

  auto index = params_->reflections_->currentIndex();
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty())
    return;

  auto reflection = reflections.at(index);
  rs_ = hub_.reflectionInfos(*reflection, betaStep);

  if (params_->moreParams_->isHidden()) {
    // points
  } else {
    // interpolated
    rs_ = core::pole::interpolate(rs_,alphaStep,betaStep,10,10,10,.8);
  }

  for (auto const& r: rs_)
    table.addRow({r.alpha(), r.beta(), r.rgeGamma().min, r.rgeGamma().max,
                  r.inten(), r.tth(), r.fwhm() });

  poleWidget_->set(rs_);
}

//------------------------------------------------------------------------------
}}
// eof
