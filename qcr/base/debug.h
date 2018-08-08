//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/base/debug.h
//! @brief     Defines preprocessor macros and functions for debugging
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef DEBUG_H
#define DEBUG_H

#include <QtDebug>
#include <QtGlobal> // protect

// TRace:
//#define TR(what) { qDebug() << what; }

// WaTch: same as TR, also prints stringized version (what is being printed)
//#define WT(what) TR(#what ":" << what)

#define ASSERT(cond) if (!(cond)) qFatal("assertion failed: " #cond)

#endif // DEBUG_H
