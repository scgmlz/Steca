// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      output_diagrams.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "output_diagrams.h"
#include "thehub.h"
#include "typ/typ_async.h"
#include <QDir>

namespace gui { namespace output {
//------------------------------------------------------------------------------

DiagramsParams::DiagramsParams(TheHub& hub) : super(hub) {
  box_->addWidget((gpAxes_ = new panel::GridPanel(hub, "Diagram")));
  rbCalc->hide();
  rbInterp->hide();
  gpInterpolation_->hide();
  rbInterp->setChecked(false);
  rbInterp->hide();
  rbCalc->setChecked(true);

  auto tags = calc::ReflectionInfo::dataTags();
  for_i (data::Metadata::numAttributes(false) - data::Metadata::numAttributes(true))
    tags.removeLast(); // remove all tags that are not numbers

  auto g = gpAxes_->grid();
  g->addWidget((xAxis = comboBox(tags)), 0, 0);
  g->addWidget(label("x"), 0, 1);
  g->addWidget((yAxis = comboBox(tags)), 1, 0);
  g->addWidget(label("y"), 1, 1);

  g->setRowStretch(g->rowCount(), 1);
}

//------------------------------------------------------------------------------

// sorts xs and ys the same way, by (x,y)
static void sortColumns(qreal_vec& xs, qreal_vec& ys, uint_vec& is) {
  EXPECT(xs.count() == ys.count())

  uint count = xs.count();

  is.resize(count);
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

  for_i (count) r[i] = xs.at(is.at(i));
  xs = r;

  for_i (count) r[i] = ys.at(is.at(i));
  ys = r;
}

//------------------------------------------------------------------------------

TabPlot::TabPlot() {
  graph_    = addGraph();
  graphAdd_ = addGraph();
  graphSub_ = addGraph();
}

void TabPlot::plot(qreal_vec::rc xs,    qreal_vec::rc ys,
                   qreal_vec::rc ysAdd, qreal_vec::rc ysSub) {
  EXPECT(xs.count() == ys.count())

  uint count = xs.count();

  graph_->clearData();
  graphAdd_->clearData();
  graphSub_->clearData();

  typ::Range rgeX, rgeY;

  for_i (count) {
    rgeX.extendBy(xs.at(i));
    rgeY.extendBy(ys.at(i));
  }

  if (!count || rgeX.isEmpty() || rgeY.isEmpty()) {
    xAxis->setVisible(false);
    yAxis->setVisible(false);
    replot();
    return;
  }

  xAxis->setRange(rgeX.min, rgeX.max);
  yAxis->setRange(rgeY.min, rgeY.max);
  xAxis->setVisible(true);
  yAxis->setVisible(true);

  graph_->setPen(QPen(Qt::blue));
  graph_->addData(xs.sup(), ys.sup());

  graphAdd_->setPen(QPen(Qt::green));
  graphAdd_->addData(xs.sup(), ysAdd.sup());

  graphSub_->setPen(QPen(Qt::red));
  graphSub_->addData(xs.sup(), ysSub.sup());

  replot();
}

//------------------------------------------------------------------------------

TabDiagramsSave::TabDiagramsSave(TheHub& hub, Params& params)
: super(hub, params)
{
  auto gp = new panel::GridPanel(hub);
  grid_->addWidget(gp, grid_->rowCount(), 0);

  auto g = gp->grid();
  g->addWidget(currentDiagram_ = radioButton("Current diagram"),0,0);
  g->addWidget(allData_        = radioButton("All data"),1,0);
  g->addWidget(fileTypes_      = comboBox(fileTags),2,0);
  g->setColumnStretch(1,1);

  currentDiagram_->setChecked(true);
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
  btnInterpolate_->hide();

  tabPlot_ = new TabPlot();
  tabs_->addTab("Diagram").box().addWidget(tabPlot_);

  connect(params()->xAxis, slot(QComboBox,currentIndexChanged,int), [this]() {
    recalculate();
  });

  connect(params()->yAxis, slot(QComboBox,currentIndexChanged,int), [this]() {
    recalculate();
  });

  tabSave_ = new TabDiagramsSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);
  tabSave_->savedMessage("File has been saved.");

  connect(tabSave_->actSave(),&QAction::triggered,[this]() {
    if (saveDiagramOutput()) {
      tabSave_->showMessage();
    }
  });

  recalculate();
}

eReflAttr DiagramsFrame::xAttr() const {
  return (eReflAttr)params()->xAxis->currentIndex();
}

eReflAttr DiagramsFrame::yAttr() const {
  return (eReflAttr)params()->yAxis->currentIndex();
}

void DiagramsFrame::displayReflection(uint reflIndex, bool interpolated) {
  super::displayReflection(reflIndex, interpolated);

  rs_ = calcPoints_[reflIndex];
  recalculate();
}

void DiagramsFrame::recalculate() {

  uint count = rs_.count();

  xs_.resize(count);
  ys_.resize(count);

  uint xi = uint(xAttr());
  uint yi = uint(yAttr());

  for_i (count) {
    auto row = rs_.at(i).data();
    xs_[i] = row.at(xi).toDouble();
    ys_[i] = row.at(yi).toDouble();
  }

  uint_vec is;
  sortColumns(xs_, ys_, is);

  auto calcErrors = [this, is] (eReflAttr attr) {
    uint count = ys_.count();
    ysErrorAdd_.resize(count); ysErrorSub_.resize(count);
    for_i (count) {
      auto  row   = rs_.at(is.at(i)).data(); // access error over sorted index vec
      qreal sigma = row.at(uint(attr)).toDouble();
      qreal y = ys_.at(i);
      ysErrorAdd_[i] = y + sigma;
      ysErrorSub_[i] = y - sigma;
    }
  };

  switch (yAttr()) {
  case eReflAttr::INTEN:
    calcErrors(eReflAttr::SIGMA_INTEN);
    break;
  case eReflAttr::TTH:
    calcErrors(eReflAttr::SIGMA_TTH);
    break;
  case eReflAttr::FWHM:
    calcErrors(eReflAttr::SIGMA_FWHM);
    break;
  default:
    ysErrorAdd_.clear(); ysErrorSub_.clear();
    break;
  }

  tabPlot_->plot(xs_, ys_, ysErrorAdd_, ysErrorSub_);
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

  str_lst separators = ts->fileSeparators;
  if (ts->currDiagram())
    writeCurrentDiagramOutputFile(filePath, separators.at(s), ts->fileTags.at(s));
  else
    writeAllDataOutputFile(filePath, separators.at(s), ts->fileTags.at(s));

  return true;
}

void DiagramsFrame::writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator, rcstr fileTag) {
  WriteFile file(filePath + fileTag);

  QTextStream stream(&file);

  EXPECT(xs_.count() == ys_.count())
  EXPECT(ysErrorAdd_.count() == ysErrorSub_.count())
  EXPECT(ysErrorAdd_.isEmpty() || ysErrorAdd_.count() == ys_.count())

  bool writeErrors = !ysErrorAdd_.isEmpty();

  for_i (xs_.count()) {
    stream << xs_.at(i) << separator << ys_.at(i);
    if (writeErrors)
      stream << separator << ysErrorAdd_.at(i) << separator << ysErrorSub_.at(i);
    stream << '\n';
  }
}

void DiagramsFrame::writeAllDataOutputFile(rcstr filePath, rcstr separator, rcstr fileTag) {
  WriteFile file(filePath + fileTag);

  QTextStream stream(&file);

  auto headers = table_->headers();

  for_i (headers.count())
    stream << headers.at(to_u(i)) << separator;

  stream << '\n';

  for_i (calcPoints_.at(params_->currReflIndex()).count()) {
    auto &row = table_->row(i);

    for_i (row.count()) {
      auto entry = row.at(i);
      if (QVariant::String == entry.type())
        stream << entry.toString();
      if (QVariant::Date == entry.type())
        stream << entry.toString();
      if (QVariant::Double == entry.type())
        stream << entry.toDouble();

      stream << separator;
    }

    stream << '\n';
  }
}

//------------------------------------------------------------------------------
}}
// eof
