// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      colors.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "colors.h"

namespace gui {
//------------------------------------------------------------------------------

QRgb intenImage(qreal inten, qreal maxInten) {
  if (qIsNaN(inten)) return qRgb(0x00, 0xff, 0xff);
  if (qIsInf(inten)) return qRgb(0xff, 0xff, 0xff);

  if (qIsNaN(maxInten) || maxInten <= 0) return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  if (inten < 0.25)
    return qRgb(0xff * inten * 4, 0, 0);
  if (inten < 0.5)
    return qRgb(0xff, 0xff * (inten - 0.25) * 4, 0);
  if (inten < 0.75)
    return qRgb(0xff - (0xff * (inten - 0.5) * 4), 0xff,
                (0xff * (inten - 0.5) * 4));
  return qRgb(0xff * (inten - 0.75) * 4, 0xff, 0xff);
}

QRgb intenGraph(qreal inten, qreal maxInten) {
  if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
    return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  return qRgb(0xff * (1 - inten / 3), 0, 0);
}

//------------------------------------------------------------------------------
}
// eof
