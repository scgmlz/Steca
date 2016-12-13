/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "typ/typ_async.h"
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

Progress::Progress(uint mulTotal, QProgressBar* bar)
: total_(0), mulTotal_(mulTotal), i_(0), bar_(bar)
{
  setTotal(1);

  if (bar_) {
    bar_->setRange(0, to_i(total_));
    bar_->setValue(0);
    bar_->show();
  }
}

Progress::~Progress() {
  if (bar_)
    bar_->hide();
}

void Progress::setTotal(uint total) {
  total_ = total * mulTotal_;
}

void Progress::setProgress(uint i) {
  if (bar_) {
    bar_->setRange(0, to_i(total_));
    bar_->setValue(to_i((i_ = qBound(0u, i, total_))));
  }
}

void Progress::step() {
  setProgress(i_ + 1);
}

//------------------------------------------------------------------------------
// eof
