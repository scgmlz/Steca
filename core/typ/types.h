// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/typ/types.h
//! @brief     Defines types float, float, QVector<float>, eNorm, TO_(INT|DOUBLE)
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYPES_H
#define TYPES_H

#include "core/typ/exception.h"

enum class eNorm {
    NONE,
    MONITOR,
    DELTA_MONITOR,
    DELTA_TIME,
    BACKGROUND,
};

int TO_INT(const QString&) THROWS;
double TO_DOUBLE(const QString&) THROWS;

#endif // TYPES_H
