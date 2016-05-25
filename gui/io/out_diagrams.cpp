// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      out_diagrams.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "out_diagrams.h"
#include "QCP/qcustomplot.h"
#include "colors.h"
#include "core_polefigure.h"
#include "core_reflection.h"
#include "gui_helpers.h"
#include "thehub.h"
#include "types/core_async.h"

namespace gui { namespace io {
//------------------------------------------------------------------------------

class DiagramsWidget : public QCustomPlot {
  SUPER(DiagramsWidget, QCustomPlot)
public:
  DiagramsWidget();
  void set(core::ReflectionInfos);

  void plot(uint xIndex, uint yIndex);

protected:
  core::ReflectionInfos rs_;
  QCPGraph *graph_;
};

DiagramsWidget::DiagramsWidget() {
  graph_ = addGraph();
}

void DiagramsWidget::set(core::ReflectionInfos rs) {
  rs_ = rs;
}

void DiagramsWidget::plot(uint xIndex, uint yIndex) {
  uint count = rs_.count();

  graph_->clearData();

  qreal_vec xs(count), ys(count); uint_vec is(count);
  core::Range rgeY, rgeX;

  for_i (count) {
    auto row = rs_.at(i).data();
    rgeX.extendBy((xs[i] = row.at(xIndex).toDouble()));
    rgeY.extendBy((ys[i] = row.at(yIndex).toDouble()));
    is[i] = i;
  }

  if (!count || rgeX.isEmpty() || rgeY.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
    return;
  }

  std::sort(is.begin(), is.end(), [&xs,&ys](uint i1,uint i2) {
    return xs.at(i1) < xs.at(i2);
  });

  qreal_vec r(count);

  for_i (count) r[i] = xs.at(is[i]);
  xs = r;

  for_i (count) r[i] = ys.at(is[i]);
  ys = r;

  xAxis->setRange(rgeX.min, rgeX.max);
  yAxis->setRange(rgeY.min, rgeY.max);
  xAxis->setVisible(true);
  yAxis->setVisible(true);

  graph_->setData(xs, ys);

  replot();
}

//------------------------------------------------------------------------------

class OutDiagramsParams : public panel::BoxPanel {
  SUPER(OutDiagramsParams, panel::BoxPanel)
  friend class OutDiagramsWindow;
public:
  OutDiagramsParams(TheHub&);

private:
  QGroupBox *gbReflection_, *gbGammaLimit_, *gbAxes_, *gbPresets_;
  QCheckBox *cbGammaLimit_;
  QRadioButton *rbPresetAll_, *rbPresetNone_;

  QComboBox *reflection_;
  QDoubleSpinBox *stepGamma_, *gammaLimitMax_, *gammaLimitMin_;
  QComboBox *xAxis_, *yAxis_;
};

OutDiagramsParams::OutDiagramsParams(TheHub &hub)
: super("", hub, Qt::Horizontal)
{
  box_->addWidget((gbReflection_ = new QGroupBox()));
  box_->addWidget((gbGammaLimit_ = new QGroupBox()));
  box_->addWidget((gbAxes_       = new QGroupBox()));
  box_->addWidget((gbPresets_    = new QGroupBox()));

  auto g = gridLayout();
  gbReflection_->setLayout(g);

  g->addWidget(label("Reflection"), 0, 0);
  g->addWidget((reflection_ = comboBox(hub_.reflectionsModel.names())), 0, 1);
  g->addWidget(label("γ step"), 2, 0);
  g->addWidget((stepGamma_ = spinCell(6, 1., 30.)), 2, 1);
  g->addWidget(label(" "), 3, 0);
  g->setColumnStretch(4, 1);

  g = gridLayout();
  gbGammaLimit_->setLayout(g);

  g->addWidget((cbGammaLimit_ = check("Gamma limit")), 0, 0);
  g->addWidget(label("max"), 1, 0);
  g->addWidget((gammaLimitMax_ = spinCell(8, 0., 100.)), 1,
               1);  // REVIEW reasonable limit
  g->addWidget(label("min"), 2, 0);
  g->addWidget((gammaLimitMin_ = spinCell(8, 0., 100.)), 2,
               1);  // REVIEW reasonable limit
  g->addWidget(label(" "), 3, 0);
  g->setColumnStretch(4, 1);

  g = gridLayout();
  gbAxes_->setLayout(g);

  auto tags = core::ReflectionInfo::dataTags();
  g->addWidget((xAxis_ = comboBox(tags)), 0, 0);
  g->addWidget(label("x"), 0, 1);
  g->addWidget((yAxis_ = comboBox(tags)), 1, 0);
  g->addWidget(label("y"), 1, 1);
  g->setRowStretch(2, 1);

  g = gridLayout();
  gbPresets_->setLayout(g);

  g->addWidget((rbPresetAll_  = radioButton("all")),  0, 0);
  g->addWidget((rbPresetNone_ = radioButton("none")), 1, 0);
  g->addWidget(radioButton("..."),  2, 0);

  g->setRowStretch(3, 1);

  // values

  stepGamma_->setValue(5);

  gammaLimitMax_->setValue(0);
  gammaLimitMin_->setValue(0);

  auto gammaEnable = [this](bool on) {
    gammaLimitMax_->setEnabled(on);
    gammaLimitMin_->setEnabled(on);
  };

  gammaEnable(false);

  connect(cbGammaLimit_, &QCheckBox::toggled, [gammaEnable](int on) {
    gammaEnable(on);
  });
}

//------------------------------------------------------------------------------

SaveDiagramsWidget::SaveDiagramsWidget() {
}

//------------------------------------------------------------------------------

OutDiagramsWindow::OutDiagramsWindow(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, parent)
{
  params_ = new OutDiagramsParams(hub);

  auto *tabs = new panel::TabsPanel(hub);
  setWidgets(params_, tabs);

  tableData_ = new OutDiagramsTableWidget(hub, core::ReflectionInfo::dataTags(),
                                          core::ReflectionInfo::dataCmps());

  connect(params_->rbPresetAll_, &QRadioButton::clicked,
          tableData_, &OutDiagramsTableWidget::presetAll);
  connect(params_->rbPresetNone_, &QRadioButton::clicked,
          tableData_, &OutDiagramsTableWidget::presetNone);

  diagramsWidget_ = new DiagramsWidget();

  connect(params_->xAxis_,
    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, &thisClass::plot);
  connect(params_->yAxis_,
    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, &thisClass::plot);

  tabs->addTab("Points").box->addWidget(tableData_);
  tabs->addTab("Diagram").box->addWidget(diagramsWidget_);

  auto saveWidget = new SaveDiagramsWidget();
  tabs->addTab("Save").box->addWidget(saveWidget);

  params_->rbPresetAll_->click();

  plot();
}

void OutDiagramsWindow::calculate() {
  auto &table = tableData_->table();
  table.clear();

  core::deg gammaStep  = params_->stepGamma_->value();

  auto  index       = params_->reflection_->currentIndex();
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty()) return;

  Progress progress(hub_.numCollectedDatasets());

  reflInfos_ = hub_.makeReflectionInfos(*reflections.at(index), gammaStep,
                                        core::Range(), &progress);
  for (auto const &r : reflInfos_)
    table.addRow(r.data());

  table.sortData();

  diagramsWidget_->set(reflInfos_);
  plot();
}

void OutDiagramsWindow::plot() {
  diagramsWidget_->plot(params_->xAxis_->currentIndex(),
                        params_->yAxis_->currentIndex());
}

//------------------------------------------------------------------------------
}}
// eof
