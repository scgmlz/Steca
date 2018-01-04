// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/def/numbers.cpp
//! @brief     Initializes NAN, INF
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "numbers.h"

static_assert(
    QT_VERSION >= QT_VERSION_CHECK(5, 4, 0), "written for Qt >= 5.4, have " QT_VERSION_STR);

qreal const NAN = Q_QNAN;
qreal const INF = Q_INFINITY;
