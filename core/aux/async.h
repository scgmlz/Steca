//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/aux/async.h
//! @brief     Defines class TakesLongTime
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef ASYNC_H
#define ASYNC_H

#include <QString>
class QProgressBar;

//! Show 'waiting' cursor, and optionally a progress bar.

class TakesLongTime final {
public:
    TakesLongTime(const QString& taskName, int totalSteps=0, QProgressBar* bar=staticBar_);
    ~TakesLongTime();
    void step();
    static void registerProgressBar(class QProgressBar* bar) { staticBar_ = bar; };
private:
    const QString taskName_;
    int total_, i_;
    static QProgressBar* staticBar_;
    QProgressBar* bar_;
};

#endif // ASYNC_H
