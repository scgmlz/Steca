// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/exception.cpp
//! @brief     Implements class Exception
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "exception.h"
#include <QtGlobal> // otherwise, strange bugs result (file loading fails silently, ...)

#ifdef QT_NO_EXCEPTIONS
#error needs exception handling
#endif
