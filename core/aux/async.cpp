//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/aux/async.cpp
//! @brief     Implements class TakesLongTime
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/aux/async.h"
#include "qcr/base/debug.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/QProgressBar>

QProgressBar* TakesLongTime::staticBar_ = nullptr;

TakesLongTime::TakesLongTime(const QString& taskName, int totalSteps, QProgressBar* bar)
    : taskName_(taskName)
    , total_(totalSteps)
    , i_(0)
    , bar_(bar)
{
    if (bar_ && total_>1) {
        bar_->setRange(0, total_);
        bar_->setValue(0);
        bar_->show();
    }
    qApp->setOverrideCursor(Qt::WaitCursor);
    qDebug() << "Long time task began: " << taskName_;
}

TakesLongTime::~TakesLongTime()
{
    qDebug() << "Long time task ended: " << taskName_;
    qApp->restoreOverrideCursor();
    if (bar_)
        bar_->hide();
}

void TakesLongTime::step()
{
    i_ = qMin(i_+1, total_);
    if (bar_)
        bar_->setValue(i_);
}
