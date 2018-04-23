//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/convert.cpp
//! @brief     Implements functions TO_INT, TO_DOUBLE
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "convert.h"
#include "qcr/engine/qcrexception.h"

int TO_INT(const QString& s)
{
    int ret;
    bool ok;
    ret = s.toInt(&ok);
    if (!ok)
        throw QCRException("Expected an integer number, found string '" + s + "'");
    return ret;
}

double TO_DOUBLE(const QString& s)
{
    double ret;
    bool ok;
    ret = s.toDouble(&ok);
    if (!ok)
        throw QCRException("Expected a floating-point number, found string '" + s + "'");
    return ret;
}
