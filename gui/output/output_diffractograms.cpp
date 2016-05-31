// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      output_diffractograms.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "output_diffractograms.h"
#include "core_lens.h"
#include "thehub.h"
#include "types/core_async.h"
#include <QDir>

namespace gui { namespace output {
//------------------------------------------------------------------------------

DiffractogramsParams::DiffractogramsParams(TheHub& hub) : super(hub) {
  gpRefl_->hide();
  gpInterpolation_->hide();
}

//------------------------------------------------------------------------------

TabDiffractogramsSave::TabDiffractogramsSave(TheHub& hub, Params& params)
: super(hub, params)
{
  auto vb = vbox();
  grid_->addLayout(vb, grid_->rowCount(), 0);

  vb->addWidget(rbCurrent_       = radioButton("Current diffractogram"));
  vb->addWidget(rbAllSequential_ = radioButton("All diffractograms to sequentially numbered files"));
  vb->addWidget(rbAll_           = radioButton("All diffractograms"));

  vb->addStretch();

  rbAll_->setChecked(true);
}

//------------------------------------------------------------------------------

DiffractogramsFrame::DiffractogramsFrame(TheHub &hub, rcstr title, QWidget *parent)
: super(hub, title, new DiffractogramsParams(hub), parent)
{
  tabs_->removeTab(0);
  btnCalculate_->hide();
  btnInterpolate_->hide();

  tabSave_ = new TabDiffractogramsSave(hub, *params_);
  tabs_->addTab("Save").box().addWidget(tabSave_);

  connect(tabSave_->actSave(),&QAction::triggered,[this]() {
    // if (saveDiagramOutput()) ...
      tabSave_->fileName().clear();
  });
}

void DiffractogramsFrame::outputCurves(core::rcRange rgeGamma, qreal gammaStep, core::rcDataset dataset) {
  auto lens = hub_.lens(dataset);
  auto &map = lens->angleMap();

  core::Range rge = map.rgeGamma();
  if (rgeGamma.isValid())
    rge = rge.intersect(rgeGamma);

  qreal step = gammaStep;
  for_i (rge.numSlices(step)) {
    qreal min = rge.min + i * step;
    core::Range gammaStripe(min, min + step);

    // here is the curve; metadata in dataset + gammaStripe
    auto curve = lens->makeCurve(gammaStripe, map.rgeTth());
  }
}

void DiffractogramsFrame::outputCurve(core::rcDataset dataset) {
  auto lens = hub_.lens(dataset);
  auto &map = lens->angleMap();

  // here is the curve; metadata in dataset + rgeGamma
  auto curve = lens->makeCurve(map.rgeGamma(), map.rgeTth());
}

void DiffractogramsFrame::outputCurrDiffractogram() {
  auto dataset = hub_.selectedDataset();
  if (dataset)
    outputCurve(*dataset);
}

void DiffractogramsFrame::outputAllDiffractograms() {
  core::deg gammaStep = params_->stepGamma->value();

  core::Range rgeGamma;
  if (params_->cbLimitGamma->isChecked())
    rgeGamma.safeSet(params_->limitGammaMin->value(),
                     params_->limitGammaMax->value());

  auto &datasets = hub_.collectedDatasets();
  Progress progress(datasets.count(), pb_);

  for (core::shp_Dataset dataset: datasets) {
    progress.step();
    outputCurves(rgeGamma, gammaStep, *dataset);
  }
}

//------------------------------------------------------------------------------
}}
// eof
