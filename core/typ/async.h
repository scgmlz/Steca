//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/async.h
//! @brief     Defines classes TakesLongTime and Progress
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

//! As long as an instance of this class exists, we see the 'waiting' cursor.

class TakesLongTime final {
public:
    TakesLongTime();
    ~TakesLongTime();
};

//! Manages the progress bar.

class Progress {
public:
    Progress() = delete;
    Progress(Progress&) = delete;
    Progress(class QProgressBar*, const QString& taskName, int totalSteps);
    ~Progress();

    void step();

private:
    const QString taskName_;
    int total_, i_;
    QProgressBar* bar_;
};

#endif // ASYNC_H
