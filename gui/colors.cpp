// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      colors.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
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

  return qRgb(0, 0, 0xff * (1 - inten / 3));
}

//------------------------------------------------------------------------------
}
// eof
