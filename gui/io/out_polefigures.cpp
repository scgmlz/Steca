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
#include "colors.h"
#include "core_polefigure.h"
#include "core_reflection.h"
#include "core_reflection_info.h"
#include "thehub.h"
#include "types/core_async.h"
#include <QPainter>
#include <QTextStream>

namespace gui { namespace io {
//------------------------------------------------------------------------------

using deg = core::deg;
using rad = core::rad;

class PoleWidget : public QWidget {
  SUPER(PoleWidget, QWidget)
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
  QPainter * p_;
  QPointF    c_;
  qreal      r_, alphaMax_;
  QCheckBox *cbFullSphere_;
};

PoleWidget::PoleWidget() {
  (cbFullSphere_ = check("Full sphere"))->setParent(this);
  connect(cbFullSphere_, &QCheckBox::toggled, [this](bool) { update(); });
}

void PoleWidget::set(core::ReflectionInfos rs) {
  rs_ = rs;
  update();
}

void PoleWidget::paintEvent(QPaintEvent*) {
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

QPointF PoleWidget::p(deg alpha, deg beta) const {
  qreal r = r_ * alpha / alphaMax_;

  rad betaRad = beta.toRad();
  return QPointF(r * cos(betaRad), -r * sin(betaRad));
}

void PoleWidget::circle(QPointF c, qreal r) {
  p_->drawEllipse(c, r, r);
}

void PoleWidget::paintGrid() {
  alphaMax_ = cbFullSphere_->isChecked() ? 180 : 90;

  QPen penMajor(Qt::gray), penMinor(Qt::lightGray);

  for (int alpha = 10; alpha <= alphaMax_; alpha += 10) {
    qreal r = r_ / alphaMax_ * alpha;
    p_->setPen(!(alpha % 90) ? penMajor : penMinor);
    circle(c_, r);
  }

  for (int beta = 0; beta < 360; beta += 10) {
    p_->setPen(!(beta % 30) ? penMajor : penMinor);
    p_->drawLine(p(10, beta), p(alphaMax_, beta));
  }
}

void PoleWidget::paintInfo() {
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

class OutPoleFiguresParams : public panel::BoxPanel {
  SUPER(OutPoleFiguresParams, panel::BoxPanel)
  friend class OutPoleFiguresWindow;
public:
  OutPoleFiguresParams(TheHub&);

private:
  QGroupBox *gbReflection_,   *gbInterpolation_, *gbGammaLimit_, *gbPresets_;
  QCheckBox *cbInterpolated_, *cbGammaLimit_;
  QRadioButton *rbPresetAll_, *rbPresetNone_;

  QComboBox *cbReflection_;
  QDoubleSpinBox
      *stepAlpha_, *stepBeta_, *idwRadius_, *averagingRadius_,
      *gammaLimitMax_, *gammaLimitMin_;
  QSpinBox *threshold_;
};

OutPoleFiguresParams::OutPoleFiguresParams(TheHub &hub)
: super("", hub, Qt::Horizontal)
{
  box_->addWidget((gbReflection_    = new QGroupBox()));
  box_->addWidget((gbInterpolation_ = new QGroupBox()));
  box_->addWidget((gbGammaLimit_    = new QGroupBox()));
  box_->addWidget((gbPresets_       = new QGroupBox()));

  auto g = gridLayout();
  gbReflection_->setLayout(g);

  g->addWidget(label("Reflection"), 0, 0);
  g->addWidget((cbReflection_ = comboBox(hub_.reflectionsModel.names())), 0, 1);
  g->addWidget(label("α step"), 1, 0);
  g->addWidget((stepAlpha_ = spinCell(6, 1., 30.)), 1, 1);
  g->addWidget(label("β step"), 2, 0);
  g->addWidget((stepBeta_ = spinCell(6, 1., 30.)), 2, 1);
  g->addWidget(label(" "), 3, 0);
  g->setColumnStretch(4, 1);

  g = gridLayout();
  gbInterpolation_->setLayout(g);

  g->addWidget((cbInterpolated_ = check("Interpolated")), 0, 0);
  g->addWidget(label("Averaging radius"), 1, 0);  // REVIEW naming
  g->addWidget((averagingRadius_ = spinCell(8., 0., 100.)), 1,
               1);                          // REVIEW reasonable limit
  g->addWidget(label("Idw radius"), 2, 0);  // REVIEW naming
  g->addWidget((idwRadius_ = spinCell(8, 0., 100.)), 2,
               1);  // REVIEW reasonable limit
  g->addWidget(label("Threshold"), 3, 0);
  g->addWidget((threshold_ = spinCell(4, 0, 100)), 3, 1);
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
  gbPresets_->setLayout(g);

  g->addWidget((rbPresetAll_  = radioButton("all")),  0, 0);
  g->addWidget((rbPresetNone_ = radioButton("none")), 1, 0);
  g->addWidget(radioButton("..."),  2, 0);

  g->setRowStretch(3, 1);

  // values

  stepAlpha_->setValue(5);
  stepBeta_->setValue(5);

  averagingRadius_->setValue(0);
  idwRadius_->setValue(0);
  threshold_->setValue(100);

  gammaLimitMax_->setValue(0);
  gammaLimitMin_->setValue(0);

  auto interpolEnable = [this](bool on) {
    stepAlpha_->setEnabled(on);
    averagingRadius_->setEnabled(on);
    idwRadius_->setEnabled(on);
    threshold_->setEnabled(on);
  };

  auto gammaEnable = [this](bool on) {
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

SavePoleFiguresWidget::SavePoleFiguresWidget() {

  auto gl = gridLayout();
  gl->addWidget(gbRefl_ = new QGroupBox(),0,0);
  gl->setRowMinimumHeight(2,10);
  gl->addWidget(gbInfos_ = new QGroupBox(),0,1);

  auto g = gridLayout();
  gbRefl_->setLayout(g);
  g->addWidget(label("Output files for "),0,0);
  g->setRowMinimumHeight(1,10);
  g->addWidget(selectedRefl_ = radioButton("Selected Reflection"), 2, 0);
  g->addWidget(allRefl_ = radioButton("All Reflections"), 3, 0);
  g->setRowStretch(4,1);

  g = gridLayout();
  g->addWidget(outputInten_     = check("Intensity Pole Figure"),     2,0);
  g->addWidget(outputTth_       = check("Peak Position Pole Figure"), 3,0);
  g->addWidget(outputFWHM_      = check("TWHM Pole Figure"),          4,0);
  g->setRowMinimumHeight(5,10);
  g->setColumnStretch(5,1);
  g->setRowStretch(5,1);
  gbInfos_->setLayout(g);
  box_->addStretch(1);

  subGl_->addLayout(gl,0,0);

  selectedRefl_->setChecked(true);
  outputInten_->setChecked(true);

}

//------------------------------------------------------------------------------

OutPoleFiguresWindow::OutPoleFiguresWindow(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, parent)
{
  params_ = new OutPoleFiguresParams(hub);
  saveWidget_ = new SavePoleFiguresWidget();
  poleWidget_ = new PoleWidget();

  auto *tabs = new panel::TabsPanel(hub);
  setWidgets(params_, tabs);

  tableData_ = new OutPoleFiguresTableWidget(hub, core::ReflectionInfo::dataTags(),
                                  core::ReflectionInfo::dataCmps());

  connect(params_->rbPresetAll_, &QRadioButton::clicked,
          tableData_, &OutPoleFiguresTableWidget::presetAll);
  connect(params_->rbPresetNone_, &QRadioButton::clicked,
          tableData_, &OutPoleFiguresTableWidget::presetNone);
  connect(params_->cbReflection_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[this](int index){
    display(index);
  });
  connect(saveWidget_->saveOutput_,&QAction::triggered,[this](){
    if (savePoleFigureOutput())
      saveWidget_->fileName_->clear();
  });

  tabs->addTab("Points").box->addWidget(tableData_);
  tabs->addTab("Graph").box->addWidget(poleWidget_);
  tabs->addTab("Save").box->addWidget(saveWidget_);

  params_->rbPresetAll_->click();
}

void OutPoleFiguresWindow::calculate() {
  reflInfos_.clear();

  auto &reflections = hub_.reflections();
  if (reflections.isEmpty()) return;

  uint reflCount = reflections.count();

  bool interpolate = params_->cbInterpolated_->isChecked();
  deg  alphaStep   = params_->stepAlpha_->value();
  deg  betaStep    = params_->stepBeta_->value();

  uint interpolateCount = interpolate
      ? core::pole::numAlphas(alphaStep) * core::pole::numBetas(betaStep)
      : 0;

  Progress progress(reflCount * (hub_.numCollectedDatasets() + interpolateCount));

  for_i (reflCount) {
    reflInfos_.append(hub_.makeReflectionInfos(*reflections[i], betaStep,
                      core::Range(), &progress));
    if (interpolate) {
      qreal treshold  = (qreal)params_->threshold_->value() / 100.0;
      qreal avgRadius = params_->averagingRadius_->value();
      qreal idwRadius = params_->idwRadius_->value();
      // REVIEW gui input for averagingAlphaMax?
      reflInfos_[i] = core::pole::interpolate(
                      reflInfos_.at(i), alphaStep, betaStep, 10, avgRadius,
                      idwRadius, treshold, &progress);
    }
  }

  auto  index = params_->cbReflection_->currentIndex();
  display(index);
}

void OutPoleFiguresWindow::display(int index) {
  auto &table = tableData_->table();
  table.clear();

  for (auto const &r : reflInfos_[index])
    table.addRow(r.data());

  table.sortData();
  poleWidget_->set(reflInfos_[index]);
}

bool OutPoleFiguresWindow::savePoleFigureOutput() {
  auto &reflections = hub_.reflections();
  if (reflections.isEmpty()) return false;
  bool check = false;
  if (saveWidget_->selectedRefl_->isChecked()) {
    auto index = params_->cbReflection_->currentIndex();
    check = writePoleFigureOutputFiles(index);
  } else {
    for_i(reflections.count()) {
      check = writePoleFigureOutputFiles(i);
    }
  }
  return check;
}

static str const OUT_FILE_TAG(".refl%1");
static int const MAX_LINE_LENGTH_POL(9);

bool OutPoleFiguresWindow::writePoleFigureOutputFiles(int index) {
  auto refl = hub_.reflections()[index];
  auto reflInfo = reflInfos_[index];
  str p = saveWidget_->dirPath_->text();
  str n = saveWidget_->fileName_->text();
  if (p.isEmpty() || p.isNull()) return false;
  if (n.isNull() || n.isEmpty()) return false;
  str fileName = QString(n + OUT_FILE_TAG).arg(index);
  str filePath = QDir(p).filePath(fileName);

  bool check = false;
  QVector<qreal> output;
  if (saveWidget_->outputInten_->isChecked()) {
    for_i (reflInfo.count())
      output.append((qreal)reflInfo.at(i).inten());
    auto intenFilePath = filePath + ".inten";
    writeListFile(intenFilePath, reflInfo, output);
    writePoleFile(intenFilePath, reflInfo, output);
    writeErrorMask(intenFilePath, reflInfo, output);
    check = true;
  }
  output.clear();
  if (saveWidget_->outputFWHM_->isChecked()) {
    for_i (reflInfo.count())
      output.append((qreal)reflInfo.at(i).fwhm());
    auto fwhmFilePath = filePath + ".fwhm";
    writeListFile(fwhmFilePath, reflInfo, output);
    writePoleFile(fwhmFilePath, reflInfo, output);
    check = true;
  }
  output.clear();
  if (saveWidget_->outputTth_->isChecked()) {
    for_i (reflInfo.count())
      output.append((qreal)reflInfo.at(i).tth());
    auto tthFilePath = filePath + ".tth";
    writeListFile(tthFilePath, reflInfo, output);
    writePoleFile(tthFilePath, reflInfo, output);
    check = true;
  }
  return check;
}

void OutPoleFiguresWindow::writeErrorMask(str filePath, core::ReflectionInfos reflInfo, QVector<qreal> output) {
  QFile file(filePath + ".errorMask");
  file.open(QIODevice::WriteOnly);
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
  file.close();
}

void OutPoleFiguresWindow::writePoleFile(str filePath, core::ReflectionInfos reflInfo, QVector<qreal> output) {
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
  file.close();
}

void OutPoleFiguresWindow::writeListFile(str filePath, core::ReflectionInfos reflInfo, QVector<qreal> output) {
  QFile file(filePath + ".lst");
  file.open(QIODevice::WriteOnly);
  QTextStream stream(&file);

  for_i(reflInfo.count()) {
    stream << (qreal)reflInfo.at(i).alpha() << " " << (qreal)reflInfo.at(i).beta() << " " << output.at(i) << '\n';
  }

  file.close();
}

//------------------------------------------------------------------------------
}}
// eof
