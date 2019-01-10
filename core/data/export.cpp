//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/export.cpp
//! @brief     Implements
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "export.h"
#include "qcr/base/debug.h"
#include <qmath.h>

// Covered by test011_export.

QString data_export::numberedFileName(const QString& templatedName, int num, int maxNum) {
    if (!templatedName.contains("%d"))
        qFatal("path does not contain placeholder %%d");
    QString ret = templatedName;
    int nDigits = (int)log10((double)maxNum)+1;
    ret.replace("%d", QString("%1").arg(num, nDigits, 10, QLatin1Char('0')));
    return ret;
}
