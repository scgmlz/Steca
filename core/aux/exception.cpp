//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/aux/exception.cpp
//! @brief     Implements class Exception
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/aux/exception.h"
#include "qcr/base/debug.h"

Exception::Exception(const QString& msg) noexcept
    : msg_(msg)
{
    qWarning() << "Exception: " << msg;
}
