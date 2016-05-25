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
  void sortColumns(uint xIndex, uint yIndex, qreal_vec& xs, qreal_vec& ys, uint_vec& is);
  int reflInfosCount() { return rs_.count(); }

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
  core::Range rgeX, rgeY;

  for_i (count) {
    auto row = rs_.at(i).data();
    rgeX.extendBy((xs[i] = row.at(xIndex).toDouble()));
    rgeY.extendBy((ys[i] = row.at(yIndex).toDouble()));

  }

  if (!count || rgeX.isEmpty() || rgeY.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
    return;
  }

  sortColumns(xIndex, yIndex, xs, ys, is);

  xAxis->setRange(rgeX.min, rgeX.max);
  yAxis->setRange(rgeY.min, rgeY.max);
  xAxis->setVisible(true);
  yAxis->setVisible(true);

  graph_->setData(xs, ys);

  replot();
}

void DiagramsWidget::sortColumns(uint xIndex, uint yIndex,
                                 qreal_vec& xs, qreal_vec& ys, uint_vec& is) {
  uint count = rs_.count();

  for_i (count)
    is[i] = i;

  std::sort(is.begin(), is.end(), [&xs,&ys](uint i1,uint i2) {
    return xs.at(i1) < xs.at(i2);
  });

  qreal_vec r(count);

  for_i (count) r[i] = xs.at(is[i]);
  xs = r;

  for_i (count) r[i] = ys.at(is[i]);
  ys = r;

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
  auto gb = new QGroupBox(this);
  auto g = gridLayout();
  g->addWidget(currentDiagram_ = radioButton("Current diagram"),0,0);
  g->addWidget(allData_ = radioButton("All data"),1,0);
  g->addWidget(fileTypes_ = comboBox({"*.txt","*.dat",".csv"}));
  gb->setLayout(g);
  subGl_->addWidget(gb,0,0);
  subGl_->setRowMinimumHeight(1,10);
}

//------------------------------------------------------------------------------

OutDiagramsWindow::OutDiagramsWindow(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, parent)
{
  params_ = new OutDiagramsParams(hub);
  saveWidget_ = new SaveDiagramsWidget();

  auto *tabs = new panel::TabsPanel(hub);
  setWidgets(params_, tabs);

  tableData_ = new OutDiagramsTableWidget(hub, core::ReflectionInfo::dataTags(),
                                          core::ReflectionInfo::dataCmps());

  connect(params_->rbPresetAll_, &QRadioButton::clicked,
          tableData_, &OutDiagramsTableWidget::presetAll);
  connect(params_->rbPresetNone_, &QRadioButton::clicked,
          tableData_, &OutDiagramsTableWidget::presetNone);
  connect(saveWidget_->saveOutput_,&QAction::triggered, [this](){
    if (saveDiagramOutput())
      saveWidget_->fileName_->clear();
  });

  diagramsWidget_ = new DiagramsWidget();

  connect(params_->xAxis_,
    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, &thisClass::plot);
  connect(params_->yAxis_,
    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
    this, &thisClass::plot);

  tabs->addTab("Points").box->addWidget(tableData_);
  tabs->addTab("Diagram").box->addWidget(diagramsWidget_);

  tabs->addTab("Save").box->addWidget(saveWidget_);

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
    table.addRow(r.data(), false);

  table.sortData();

  diagramsWidget_->set(reflInfos_);
  plot();
}

void OutDiagramsWindow::plot() {
  diagramsWidget_->plot(params_->xAxis_->currentIndex(),
                        params_->yAxis_->currentIndex());
}

bool OutDiagramsWindow::saveDiagramOutput() {
  str_lst fileTags = {".txt",".dat",".csv"};
  str_lst separators = {",", " ", ";"};
  str p = saveWidget_->dirPath_->text();
  str n = saveWidget_->fileName_->text();
  if (p.isEmpty() || p.isNull()) return false;
  if (n.isNull() || n.isEmpty()) return false;
  str fileName = QString(n);
  str filePath = QDir(p).filePath(fileName);
  auto s = saveWidget_->fileTypes_->currentIndex();

  if (saveWidget_->currentDiagram_->isChecked()) {
    writeCurrentDiagramOutputFile(filePath,separators[s], fileTags[s]);
    return true;
  } else {
    writeAllDataOutputFile(filePath,separators[s], fileTags[s]);
    return true;
  }
}

void OutDiagramsWindow::writeCurrentDiagramOutputFile(str filePath, str separator, str fileTag) {
  QFile file(filePath + fileTag);
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);
  auto &table = tableData_->table();
  auto xIndex = params_->xAxis_->currentIndex();
  auto yIndex = params_->yAxis_->currentIndex();
  uint count = diagramsWidget_->reflInfosCount();
  qreal_vec xs(count), ys(count); uint_vec is(count);
  for_i (count) {
    xs[i] = table.row(i).at(xIndex).toDouble(); // TODO check type first
    ys[i] = table.row(i).at(yIndex).toDouble();
  }

  diagramsWidget_->sortColumns(xIndex, yIndex,xs,ys,is);

  for_i (count) {
    stream << xs.at(i) << separator << ys.at(i) << '\n';
  }

  file.close();
}

void OutDiagramsWindow::writeAllDataOutputFile(str filePath, str separator, str fileTag) {
  QFile file(filePath + fileTag);
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);

  auto &table = tableData_->table();
  auto colTitles = table.colTitles();
  for_i (colTitles.count())
    stream << colTitles.at(i) << separator;

  stream << '\n';

  for_i (reflInfos_.count()) {
    auto &row = table.row(i);
    for_i (row.count()) {
      auto entry = row.at(i);
      if (entry.canConvert(QMetaType::QString))
        stream << entry.toString() << " ";
      else if (entry.canConvert(QMetaType::QDate))
        stream << entry.toString();
      else if (entry.canConvert(QMetaType::QReal))
        stream << entry.toDouble();
    }
    stream << '\n';
  }

  file.close();
}

//------------------------------------------------------------------------------
}}
// eof
