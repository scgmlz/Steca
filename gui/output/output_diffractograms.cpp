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

//------------------------------------------------------------------------------
}}
// eof
