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
  box_->addWidget((gpAxes_ = new panel::GridPanel(hub, "Diagram")));
  auto g = gpAxes_->grid();
  gpInterpolation_->hide();
  auto tags = core::ReflectionInfo::dataTags();
  for_i (core::Metadata::numAttributes(false) - core::Metadata::numAttributes(true))
    tags.removeLast(); // remove all tags that are not numbers

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
    qreal x1 = xs.at(i1), x2 =xs.at(i2);
    if (x1 < x2)
      return true;
    if (x1 > x2)
      return false;
    return ys.at(i1) < ys.at(i2);
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

  sortColumns(xs, ys, is); // TODO for inten, tth, fwhm also sort the corressponding errorValue

  xAxis->setRange(rgeX.min, rgeX.max);
  yAxis->setRange(rgeY.min, rgeY.max);
  xAxis->setVisible(true);
  yAxis->setVisible(true);

  calculateErrors(yIndex,xs,ys,is);
  if (!yErrorAdd_.isEmpty() && !yErrorSub_.isEmpty()) {
    graph_->setPen(QPen(Qt::green));
    graph_->addData(xs,yErrorAdd_);
    graph_->setPen(QPen(Qt::red));
    graph_->addData(xs,yErrorSub_);
  }
  graph_->setPen(QPen(Qt::blue));
  graph_->addData(xs, ys);

  replot();
}

void TabPlot::calculateErrors(uint yIndex, qreal_vec xs, qreal_vec ys, uint_vec is) {
    auto calc = [this, xs, ys, is] (uint index) {
      for_i (xs.count()) {
        auto row = rs_.at(is[i]).data(); // acces error over sorted index vec
        auto sigma = row.at(index).toDouble();
        yErrorAdd_.append(ys[i] + sigma);
        yErrorSub_.append(ys[i] - sigma);
      }
    };

    yErrorAdd_.clear();
    yErrorSub_.clear();
    auto yTag = core::ReflectionInfo::reflStringTag(yIndex);
    if (core::ReflectionInfo::reflStringTag((uint)core::ReflectionInfo::eReflAttr::INTEN) == yTag) {
      calc((uint)core::ReflectionInfo::eReflAttr::SIGMA_INTEN);
    }
    else if (core::ReflectionInfo::reflStringTag((uint)core::ReflectionInfo::eReflAttr::TTH) == yTag) {
      calc((uint)core::ReflectionInfo::eReflAttr::SIGMA_TTH);
    }
    else if (core::ReflectionInfo::reflStringTag((uint)core::ReflectionInfo::eReflAttr::FWHM) == yTag) {
      calc((uint)core::ReflectionInfo::eReflAttr::SIGMA_FWHM);
    }

}

//------------------------------------------------------------------------------

static str_lst const fileTags {".txt",".dat",".csv"};

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
  tabPlot_ = new TabPlot();
  tabs_->addTab("Diagram").box().addWidget(tabPlot_);

  connect(static_cast<DiagramsParams*>(params_)->xAxis,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this, &thisClass::plot);
  connect(static_cast<DiagramsParams*>(params_)->yAxis,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),this, &thisClass::plot);

  tabSave_ = new TabDiagramsSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);

  connect(tabSave_->actSave(),&QAction::triggered,[this]() {
    if (saveDiagramOutput())
      tabSave_->clearFilename();
  });

  plot();
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
  WriteFile file(filePath + fileTag);

  auto ps = static_cast<DiagramsParams*>(params_);

  auto xIndex = ps->xAxis->currentIndex();
  auto yIndex = ps->yAxis->currentIndex();
  auto cIndex = params_->cbRefl->currentIndex();

  uint count = calcPoints_.at(cIndex).count();
  qreal_vec xs(count), ys(count);
  uint_vec is(count);

  for_i (count) {
    xs[i] = table_->row(i).at(xIndex).toDouble();
    ys[i] = table_->row(i).at(yIndex).toDouble();
  }

  sortColumns(xs, ys, is);
  QTextStream stream(&file);

  for_i (count) {
    stream << xs.at(i) << separator << ys.at(i);
    if (!tabPlot_->yErrorAdd_.isEmpty() && !tabPlot_->yErrorSub_.isEmpty())
      stream << separator << tabPlot_->yErrorAdd_.at(i) << separator << tabPlot_->yErrorSub_.at(i);
    stream << '\n';
  }

}

void DiagramsFrame::writeAllDataOutputFile(rcstr filePath, rcstr separator, rcstr fileTag) {
  WriteFile file(filePath + fileTag);

  QTextStream stream(&file);

  auto headers = table_->headers();

  for_i (headers.count())
    stream << headers.at(i) << separator;

  stream << '\n';

  for_i (calcPoints_.count()) {
    auto &row = table_->row(i);

    for_i (row.count()) {
      auto entry = row.at(i);
      if (QVariant::String == entry.type())
        stream << entry.toString() << " ";
      else if (QVariant::Date == entry.type())
        stream << entry.toString();
      else if (QVariant::Double == entry.type())
        stream << entry.toDouble();
    }
    stream << '\n';
  }
}

//------------------------------------------------------------------------------
}}
// eof
