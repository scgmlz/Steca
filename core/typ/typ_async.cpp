// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_async.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ_async.h"
#include <QtWidgets/QProgressBar>

TakesLongTime::TakesLongTime() {
    if (handler)
        handler(true);
}

TakesLongTime::~TakesLongTime() {
    if (handler)
        handler(false);
}

void (*TakesLongTime::handler)(bool) = nullptr;

Progress::Progress(uint mulTotal, QProgressBar* bar)
    : total_(0), mulTotal_(mulTotal), i_(0), bar_(bar) {
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
