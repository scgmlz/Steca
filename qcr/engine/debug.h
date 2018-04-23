//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/debug.h
//! @brief     Defines preprocessor macros and functions for debugging
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DEBUG_H
#define DEBUG_H

#include <QtGlobal> // protect
#include <QtDebug>

// TRace:
#define TR(what) { qDebug() << what; }

// WaTch: same as TR, also prints stringized version (what is being printed)
#define WT(what) TR(#what ":" << what)

#define ASSERT(cond) if (!(cond)) qFatal("assertion failed: " #cond)

#define EMITS(src,sig) qDebug()<<src<<" emits "<<#sig; emit sig;

#endif // DEBUG_H
