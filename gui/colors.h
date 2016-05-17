// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      colors.h
//! @brief     Colors, scales, and palletes.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef COLORS_H
#define COLORS_H

#include "types/core_defs.h"
#include <QRgb>

namespace gui {
//------------------------------------------------------------------------------

QRgb intenImage(qreal inten, qreal maxInten = 1);
QRgb intenGraph(qreal inten, qreal maxInten = 1);

//------------------------------------------------------------------------------
}
#endif // COLORS_H
