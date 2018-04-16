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
#include "qcr/engine/debug.h"
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QApplication>

TakesLongTime::TakesLongTime(const QString& taskName)
    : taskName_(taskName)
{
    qApp->setOverrideCursor(Qt::WaitCursor);
    qDebug() << "Long time task began: " << taskName_;
}

TakesLongTime::~TakesLongTime()
{
    qDebug() << "Long time task ended: " << taskName_;
    qApp->restoreOverrideCursor();
}

Progress::Progress(QProgressBar* bar, const QString& taskName, int totalSteps)
    : bar_(bar)
    , taskName_(taskName)
    , total_(totalSteps)
    , i_(0)
{
    if (bar_) {
        bar_->setRange(0, total_);
        bar_->setValue(0);
        bar_->show();
    }
    qDebug() << "Task with progress bar [" << taskName_ << "] began";
}

Progress::~Progress()
{
    qDebug() << "Task with progress bar [" << taskName_ << "] ended";
    if (bar_)
        bar_->hide();
}

void Progress::step()
{
    i_ = qMin(i_+1, total_);
    if (bar_)
        bar_->setValue(i_);
}
