// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_polefigures.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "output_polefigures.h"
#include "colors.h"
#include "thehub.h"

#include <QDir>
#include <QPainter>
#include <QTextStream>

namespace gui { namespace output {
//------------------------------------------------------------------------------

TabGraph::TabGraph(TheHub& hub, Params& params)
: super(hub, params)
{
  connect(params_.averagingRadius, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), [this]() {
    update();
  });
}

void TabGraph::set(core::ReflectionInfos rs) {
  rs_ = rs;
  update();
}

void TabGraph::paintEvent(QPaintEvent*) {
  int w = size().width(), h = size().height();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.translate(w / 2, h / 2);

  p_ = &painter;
  c_ = QPointF(0, 0);
  r_ = qMin(w, h) / 2;

  paintGrid();
  paintInfo();
}

QPointF TabGraph::p(deg alpha, deg beta) const {
  qreal r = r_ * alpha / alphaMax_;

  rad betaRad = beta.toRad();
  return QPointF(r * cos(betaRad), -r * sin(betaRad));
}

void TabGraph::circle(QPointF c, qreal r) {
  p_->drawEllipse(c, r, r);
}

void TabGraph::paintGrid() {
  QPen penMajor(Qt::gray), penMinor(Qt::lightGray);

  for (int alpha = 10; alpha <= alphaMax_; alpha += 10) {
    qreal r = r_ / alphaMax_ * alpha;
    p_->setPen(!(alpha % 30) ? penMajor : penMinor);
    circle(c_, r);
  }

  for (int beta = 0; beta < 360; beta += 10) {
    p_->setPen(!(beta % 30) ? penMajor : penMinor);
    p_->drawLine(p(10, beta), p(alphaMax_, beta));
  }

  QPen penMark(Qt::darkGreen);
  p_->setPen(penMark);
  circle(c_, r_ * params_.averagingRadius->value() / alphaMax_);
}

void TabGraph::paintInfo() {
  qreal rgeMax = rs_.rgeInten().max;

  for (auto const &r : rs_) {
    qreal inten = r.inten();

    if (qIsFinite(inten)) { // nan comes from interpolartion
      inten /= rgeMax;
      auto color = QColor(intenGraph(inten));
      p_->setPen(color);
      p_->setBrush(color);
      circle(p(r.alpha(), r.beta()), inten * r_ / 60);
    }
  }
}

//------------------------------------------------------------------------------

TabPoleFiguresSave::TabPoleFiguresSave(TheHub& hub, Params& params)
: super(hub,params)
{
  auto hb = hbox();
  grid_->addLayout(hb, grid_->rowCount(), 0);

  auto p1 = new panel::BoxPanel(hub, "Reflection", Qt::Vertical);
  auto p2 = new panel::BoxPanel(hub, "I", Qt::Vertical);

  hb->addWidget(p1);
  hb->addWidget(p2);

  grid_->setRowStretch(grid_->rowCount(), 1);

  {
    auto b = p1->box();

    b->addWidget(label("Output files for:"));
    b->addWidget(rbSelectedRefl_ = radioButton("Selected Reflection"));
    b->addWidget(rbAllRefls_      = radioButton("All Reflections"));

    b->addStretch();
  }

  {
    auto b = p2->box();

    b->addWidget(outputInten_ = check("Intensity Pole Figure"));
    b->addWidget(outputTth_   = check("Peak Position Pole Figure"));
    b->addWidget(outputFWHM_  = check("TWHM Pole Figure"));

    b->addStretch();
  }

  rbSelectedRefl_->setChecked(true);
  outputInten_->setChecked(true);
}

bool TabPoleFiguresSave::onlySelectedRefl() const {
  return rbSelectedRefl_->isChecked();
}

bool TabPoleFiguresSave::outputInten() const{
  return outputInten_->isChecked();
}

bool TabPoleFiguresSave::outputTth() const {
  return outputTth_->isChecked();
}

bool TabPoleFiguresSave::outputFWHM() const {
  return outputFWHM_->isChecked();
}

//------------------------------------------------------------------------------

PoleFiguresFrame::PoleFiguresFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new PoleFiguresParams(hub), parent)
{
  tabGraph_ = new TabGraph(hub, *params_);
  tabs_->addTab("Graph").box().addWidget(tabGraph_);

  tabSave_ = new TabPoleFiguresSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);

  connect(tabSave_->actSave(), &QAction::triggered, [this]() {
    if (savePoleFigureOutput())
      tabSave_->clearFilename();
  });
}

#ifdef DEVELOPMENT_JAN
void PoleFiguresFrame::show() {
  super::show();
  params_->stepAlpha->setValue(10);
  params_->stepBeta->setValue(10);
  params_->stepGamma->setValue(1.2);
  params_->idwRadius->setValue(20);
  params_->averagingRadius->setValue(20);
  calculate();
}
#endif

void PoleFiguresFrame::displayReflection(uint reflIndex, bool interpolated) {
  super::displayReflection(reflIndex, interpolated);
  tabGraph_->set((interpolated ? interpPoints_ : calcPoints_)[reflIndex]);
}

bool PoleFiguresFrame::savePoleFigureOutput() {
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty())
    return false;

  if (tabSave_->onlySelectedRefl())
    return writePoleFigureOutputFiles(params_->currReflIndex());

  // all reflections
  bool check = true;

  for_i (reflections.count())
    if (!writePoleFigureOutputFiles(i))
      check = false;  // some failed

  return check;
}

static str const OUT_FILE_TAG(".refl%1");
static int const MAX_LINE_LENGTH_POL(9);

bool PoleFiguresFrame::writePoleFigureOutputFiles(uint index) {
  auto refl = hub_.reflections()[index];
  auto reflInfo = interpPoints_[index];

  str p = tabSave_->path();
  str n = tabSave_->fileName();

  if (p.isEmpty() || n.isEmpty())
    return false;

  str fileName = str(n + OUT_FILE_TAG).arg(index + 1);
  str filePath = QDir(p).filePath(fileName);

  bool check = false;

  if (tabSave_->outputInten()) {
    qreal_vec output;
    for_i (reflInfo.count())
      output.append(reflInfo.at(i).inten());

    auto intenFilePath = filePath + ".inten";
    writeListFile(intenFilePath, reflInfo, output);
    writePoleFile(intenFilePath, reflInfo, output);
    writeErrorMask(intenFilePath, reflInfo, output);

    check = true;
  }

  if (tabSave_->outputTth()) {
    qreal_vec output;
    for_i (reflInfo.count())
      output.append(reflInfo.at(i).tth());

    auto tthFilePath = filePath + ".tth";
    writeListFile(tthFilePath, reflInfo, output);
    writePoleFile(tthFilePath, reflInfo, output);

    check = true;
  }

  if (tabSave_->outputFWHM()) {
    qreal_vec output;
    for_i (reflInfo.count())
      output.append(reflInfo.at(i).fwhm());

    auto fwhmFilePath = filePath + ".fwhm";
    writeListFile(fwhmFilePath, reflInfo, output);
    writePoleFile(fwhmFilePath, reflInfo, output);

    check = true;
  }

  return check;
}

void PoleFiguresFrame::writeErrorMask(rcstr filePath, core::ReflectionInfos reflInfo, qreal_vec const& output) {
  QFile file(filePath + ".errorMask");
  file.open(QIODevice::WriteOnly); // TODO if (!file.open) ...

  QTextStream stream(&file);
  for(int j = 0, jEnd = reflInfo.count(); j < jEnd; j+=9) {
    for (int i = j; i < j + MAX_LINE_LENGTH_POL; i++) {
      if (qIsNaN(output.at(i)))
        stream << "0" << " ";
      else
        stream << "1" << " ";
    }
    stream << '\n';
  }
}

void PoleFiguresFrame::writePoleFile(rcstr filePath, core::ReflectionInfos reflInfo, qreal_vec const& output) {
  QFile file(filePath + ".pol");
  file.open(QIODevice::WriteOnly);

  QTextStream stream(&file);
  for(int j = 0, jEnd = reflInfo.count(); j < jEnd; j+=9) {
    for (int i = j; i < j + MAX_LINE_LENGTH_POL; i++) {
      if (qIsNaN(output.at(i)))
        stream << " -1 " << " ";
      else
        stream << output.at(i) << " ";
    }
    stream << '\n';
  }
}

void PoleFiguresFrame::writeListFile(rcstr filePath, core::ReflectionInfos reflInfo, qreal_vec const& output) {
  QFile file(filePath + ".lst");
  file.open(QIODevice::WriteOnly);

  QTextStream stream(&file);

  for_i (reflInfo.count()) {
    stream << (qreal)reflInfo.at(i).alpha() << " " << (qreal)reflInfo.at(i).beta() << " " << output.at(i) << '\n';
  }
}

//------------------------------------------------------------------------------
}}
// eof
