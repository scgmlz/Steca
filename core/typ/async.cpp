//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/async.cpp
//! @brief     Implements classes TakesLongTime and Progress
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/typ/async.h"
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QApplication>

TakesLongTime::TakesLongTime()
{
    qApp->setOverrideCursor(Qt::WaitCursor);
}

TakesLongTime::~TakesLongTime()
{
    qApp->restoreOverrideCursor();
}

Progress::Progress(int mulTotal, QProgressBar* bar)
    : total_(0)
    , mulTotal_(mulTotal)
    , i_(0)
    , bar_(bar)
{
    setTotal(1);
    if (bar_) {
        bar_->setRange(0, total_);
        bar_->setValue(0);
        bar_->show();
    }
}

Progress::~Progress()
{
    if (bar_)
        bar_->hide();
}

void Progress::setTotal(int total)
{
    total_ = total * mulTotal_;
}

void Progress::setProgress(int i)
{
    if (bar_) {
        bar_->setRange(0, total_);
        bar_->setValue((i_ = qBound(0, i, total_)));
    }
}

void Progress::step()
{
    setProgress(i_ + 1);
}
