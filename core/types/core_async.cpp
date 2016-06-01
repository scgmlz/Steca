// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_async.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Antti Soininen, Jan Burle, Rebecca Brydon
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_async.h"
#include <QtWidgets/QProgressBar>

//------------------------------------------------------------------------------

TakesLongTime::TakesLongTime() {
  if (handler) handler(true);
}

TakesLongTime::~TakesLongTime() {
  if (handler) handler(false);
}

void (*TakesLongTime::handler)(bool) = nullptr;

//------------------------------------------------------------------------------

Progress::Progress(uint total, QProgressBar* bar)
: total_(total), i_(0), bar_(bar)
{
  if (bar_) {
    bar_->setRange(0, total_);
    bar_->setValue(i_);
    bar_->show();
  }
}

Progress::~Progress() {
  if (bar_)
    bar_->hide();
}

void Progress::setProgress(uint i) {
  if (bar_)
    bar_->setValue((i_ = qBound(0u, i, total_)));
}

void Progress::step() {
  setProgress(i_ + 1);
}

// eof
