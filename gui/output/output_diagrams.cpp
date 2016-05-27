// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_diagrams.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "output_diagrams.h"
#include "thehub.h"
#include "types/core_async.h"
#include <QDir>

namespace gui { namespace output {
//------------------------------------------------------------------------------

DiagramsParams::DiagramsParams(TheHub& hub) : super(hub) {
  box_->addWidget((gpAxes_ = new panel::GridPanel(hub, "Reflection")));
  auto g = gpAxes_->grid();

  auto tags = core::ReflectionInfo::dataTags();
  g->addWidget((xAxis = comboBox(tags)), 0, 0);
  g->addWidget(label("x"), 0, 1);
  g->addWidget((yAxis = comboBox(tags)), 1, 0);
  g->addWidget(label("y"), 1, 1);

  g->setRowStretch(g->rowCount(), 1);
}

//------------------------------------------------------------------------------

static void sortColumns(qreal_vec& xs, qreal_vec& ys, uint_vec& is) {
  EXPECT(xs.count() == ys.count() && xs.count() == is.count())

  uint count = xs.count();

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

TabPlot::TabPlot() {
  graph_ = addGraph();
}

void TabPlot::set(core::ReflectionInfos rs) {
  rs_ = rs;
}

void TabPlot::plot(uint xIndex, uint yIndex) {
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

  sortColumns(xs, ys, is);

  xAxis->setRange(rgeX.min, rgeX.max);
  yAxis->setRange(rgeY.min, rgeY.max);
  xAxis->setVisible(true);
  yAxis->setVisible(true);

  graph_->setData(xs, ys);

  replot();
}

//------------------------------------------------------------------------------

static str_lst const fileTags {"*.txt","*.dat",".csv"};

TabDiagramsSave::TabDiagramsSave(TheHub& hub, Params& params)
: super(hub, params)
{
  auto gp = new panel::GridPanel(hub);
  grid_->addWidget(gp, grid_->rowCount(), 0);

  auto g = gp->grid();
  g->addWidget(currentDiagram_ = radioButton("Current diagram"),0,0);
  g->addWidget(allData_ = radioButton("All data"),1,0);
  g->addWidget(fileTypes_ = comboBox(fileTags));
}

uint TabDiagramsSave::currType() const {
  return fileTypes_->currentIndex();
}

bool TabDiagramsSave::currDiagram() const {
  return currentDiagram_->isChecked();
}

//------------------------------------------------------------------------------

DiagramsFrame::DiagramsFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new DiagramsParams(hub), parent)
{
//  saveWidget_ = new TabDiagramsSave(hub);

//  auto *tabs = new panel::TabsPanel(hub);
//  setWidgets(params_, tabs);

//  tableData_ = new OutDiagramsTableWidget(hub, core::ReflectionInfo::dataTags(),
//                                          core::ReflectionInfo::dataCmps());

//  connect(params_->rbPresetAll_, &QRadioButton::clicked,
//          tableData_, &OutDiagramsTableWidget::presetAll);
//  connect(params_->rbPresetNone_, &QRadioButton::clicked,
//          tableData_, &OutDiagramsTableWidget::presetNone);
//  connect(saveWidget_->saveOutput_,&QAction::triggered, [this](){
//    if (saveDiagramOutput())
//      saveWidget_->fileName_->clear();
//  });

//  diagramsWidget_ = new TabPlot();

//  connect(params_->xAxis_,
//    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
//    this, &thisClass::plot);
//  connect(params_->yAxis_,
//    static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
//    this, &thisClass::plot);

//  tabs->addTab("Points").box_->addWidget(tableData_);
//  tabs->addTab("Diagram").box_->addWidget(diagramsWidget_);

//  tabs->addTab("Save").box_->addWidget(saveWidget_);

//  params_->rbPresetAll_->click();

  //  plot();
}

void DiagramsFrame::displayReflection(uint reflIndex, bool interpolated) {
  super::displayReflection(reflIndex, interpolated);
  tabPlot_->set(calcPoints_[reflIndex]);
  plot();
}

void DiagramsFrame::plot() {
  auto ps = static_cast<DiagramsParams*>(params_);
  tabPlot_->plot(ps->xAxis->currentIndex(),
                 ps->yAxis->currentIndex());
}

//------------------------------------------------------------------------------

bool DiagramsFrame::saveDiagramOutput() {
  auto ts = static_cast<TabDiagramsSave*>(tabSave_);

  str p = ts->path();
  str n = ts->fileName();

  if (p.isEmpty() || n.isEmpty())
    return false;

  str fileName = n;
  str filePath = QDir(p).filePath(fileName);

  auto s = ts->currType();

  str_lst separators = {",", " ", ";"};
  if (ts->currDiagram()) {
    writeCurrentDiagramOutputFile(filePath, separators[s], fileTags[s]);
    return true;
  } else {
    writeAllDataOutputFile(filePath, separators[s], fileTags[s]);
    return true;
  }
}

void DiagramsFrame::writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator, rcstr fileTag) {
  QFile file(filePath + fileTag);
  file.open(QIODevice::WriteOnly);  // TODO if (file.open ...

  auto ps = static_cast<DiagramsParams*>(params_);

  auto xIndex = ps->xAxis->currentIndex();
  auto yIndex = ps->yAxis->currentIndex();

  uint count = calcPoints_.count();
  qreal_vec xs(count), ys(count);
  uint_vec is(count);

  for_i (count) {
    xs[i] = table_->row(i).at(xIndex).toDouble(); // TODO check type first
    ys[i] = table_->row(i).at(yIndex).toDouble();
  }

  sortColumns(xs, ys, is);

  QTextStream stream(&file);
  for_i (count) {
    stream << xs.at(i) << separator << ys.at(i) << '\n';
  }
}

void DiagramsFrame::writeAllDataOutputFile(rcstr filePath, rcstr separator, rcstr fileTag) {
  QFile file(filePath + fileTag);
  file.open(QIODevice::WriteOnly); // TODO if ...

  QTextStream stream(&file);

  auto headers = table_->headers();

  for_i (headers.count())
    stream << headers.at(i) << separator;

  stream << '\n';

  for_i (calcPoints_.count()) {
    auto &row = table_->row(i);

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
}

//------------------------------------------------------------------------------
}}
// eof
