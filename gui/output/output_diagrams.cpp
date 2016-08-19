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

namespace gui { namespace output {
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

  for_i (count)
    r[i] = xs.at(is.at(i));

  xs = r;

  for_i (count)
    r[i] = ys.at(is.at(i));

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
: super(hub, params, true)
{
  auto gp = new panel::GridPanel(hub, "To save");
  grid_->addWidget(gp, grid_->rowCount(), 0, 1, 2);
  grid_->addRowStretch();

  auto g = gp->grid();
  g->addWidget((currentDiagram_ = radioButton("Current diagram")));
  g->addWidget((allData_        = radioButton("All data")));
  g->addWidget(textButton(actSave), 1, 1);
  g->setColumnStretch(0, 1);

  currentDiagram_->setChecked(true);
}

uint TabDiagramsSave::currType() const {
  return fileTypes_->currentIndex();
}

bool TabDiagramsSave::currDiagram() const {
  return currentDiagram_->isChecked();
}

//------------------------------------------------------------------------------

static const Params::ePanels PANELS = Params::ePanels(
    Params::REFLECTION | Params::GAMMA | Params::DIAGRAM);

DiagramsFrame::DiagramsFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new Params(hub, PANELS), parent)
{
  btnInterpolate_->hide();

  tabPlot_ = new TabPlot();
  tabs_->addTab("Diagram").box().addWidget(tabPlot_);

  ENSURE(params_->panelDiagram)
  auto pd = params_->panelDiagram;

  connect(pd->xAxis, slot(QComboBox,currentIndexChanged,int), [this]() {
    recalculate();
  });

  connect(pd->yAxis, slot(QComboBox,currentIndexChanged,int), [this]() {
    recalculate();
  });

  tabSave_ = new TabDiagramsSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);

  connect(tabSave_->actSave, &QAction::triggered, [this]() {
    logSuccess(saveDiagramOutput());
  });

  recalculate();
}

DiagramsFrame::eReflAttr DiagramsFrame::xAttr() const {
  ENSURE(params_->panelDiagram)
  return eReflAttr(params_->panelDiagram->xAxis->currentIndex());
}

DiagramsFrame::eReflAttr DiagramsFrame::yAttr() const {
  ENSURE(params_->panelDiagram)
  return eReflAttr(params_->panelDiagram->yAxis->currentIndex());
}

void DiagramsFrame::displayReflection(uint reflIndex, bool interpolated) {
  super::displayReflection(reflIndex, interpolated);
  rs_ = calcPoints_.at(reflIndex);
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
  str path = tabSave_->filePath(true);
  if (path.isEmpty())
    return false;

  str separator = tabSave_->separator();

  if (tabSave_->currDiagram())
    writeCurrentDiagramOutputFile(path, separator);
  else
    writeAllDataOutputFile(path, separator);

  return true;
}

void DiagramsFrame::writeCurrentDiagramOutputFile(rcstr filePath, rcstr separator) {
  WriteFile file(filePath);

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

void DiagramsFrame::writeAllDataOutputFile(rcstr filePath, rcstr separator) {
  WriteFile file(filePath);
  QTextStream stream(&file);

  auto headers = table_->headers();

  for_i (headers.count())
    stream << headers.at(to_u(i)) << separator;

  stream << '\n';

  for_i (calcPoints_.at(getReflIndex()).count()) {
    auto &row = table_->row(i);

    for_i (row.count()) {
      QVariant const& var = row.at(i);
      if (typ::isReal(var))
        stream << var.toDouble();
      else
        stream << var.toString();

      stream << separator;
    }

    stream << '\n';
  }
}

//------------------------------------------------------------------------------
}}
// eof
