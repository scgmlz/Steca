// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/debug.h
//! @brief     Defines preprocessor macros and functions for debugging
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef DEBUG_H
#define DEBUG_H

#include <QtGlobal> // protect
#include <QDebug>
#include <QtDebug>

// TRace:
#define TR(what) { qDebug() << what; }

// WaTch: same as TR, also prints stringized version (what is being printed)
#define WT(what) TR(#what ":" << what)

namespace debug {
    void ensure(bool cond, const char* text="assertion failed");
}

#endif // DEBUG_H
