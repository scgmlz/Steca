// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/types.h
//! @brief     Implements types TO_(INT|DOUBLE)
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "types.h"

int TO_INT(const QString& s) {
    int ret;
    bool ok;
    ret = s.toInt(&ok);
    if (!ok)
        THROW("Expected an integer number, found string '" + s + "'");
    return ret;
}

double TO_DOUBLE(const QString& s) {
    double ret;
    bool ok;
    ret = s.toDouble(&ok);
    if (!ok)
        THROW("Expected a floating-point number, found string '" + s + "'");
    return ret;
}
