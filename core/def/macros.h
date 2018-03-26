// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/def/macros.h
//! @brief     Defines macro THROWS, sets some compiler pragmas
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MACROS_H
#define MACROS_H

#include <QtGlobal> // to define Q_OS_WIN

// exception specification macro
#ifdef Q_OS_WIN
    #define THROWS
#else
    #define THROWS noexcept(false)
#endif

#endif // MACROS_H
