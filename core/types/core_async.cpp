// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_async.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_async.h"
#include <QtWidgets/QProgressDialog>

//------------------------------------------------------------------------------

TakesLongTime::TakesLongTime() {
  if (handler) handler(true);
}

TakesLongTime::~TakesLongTime() {
  if (handler) handler(false);
}

void (*TakesLongTime::handler)(bool) = nullptr;

//------------------------------------------------------------------------------

Progress::Progress(uint total)
: total_(total), pd_(nullptr)
{
  pd_ = new QProgressDialog(nullptr, Qt::FramelessWindowHint);
  pd_->setWindowModality(Qt::ApplicationModal);
  pd_->setCancelButtonText(EMPTY_STR);
  pd_->setRange(0, total_);
  pd_->show();
}

Progress::~Progress() {
  delete pd_;
}

void Progress::setProgress(uint i) {
  pd_->setValue(qBound(0u, i, total_));
}

void Progress::step() {
  pd_->setValue(pd_->value() + 1);
}

// eof
