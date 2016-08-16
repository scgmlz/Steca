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
#include "typ/typ_vec.h"

#include <QColor>

namespace gui {
//------------------------------------------------------------------------------

QRgb intenImage(qreal inten, qreal maxInten) {
  if (qIsNaN(inten))
    return qRgb(0x00, 0xff, 0xff);
  if (qIsInf(inten))
    return qRgb(0xff, 0xff, 0xff);

  if (qIsNaN(maxInten) || maxInten <= 0)
    return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  if (inten < 0.25)
    return qRgb(int(0xff * inten * 4), 0, 0);
  if (inten < 0.5)
    return qRgb(0xff, int(0xff * (inten - 0.25) * 4), 0);
  if (inten < 0.75)
    return qRgb(int(0xff - (0xff * (inten - 0.5) * 4)), 0xff,
                int(0xff * (inten - 0.5) * 4));
  return qRgb(int(0xff * (inten - 0.75) * 4), 0xff, 0xff);
}

QRgb intenGraph(qreal inten, qreal maxInten) {
  if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
    return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  return qRgb(0, 0, int(0xff * (1 - inten / 3)));
}

QRgb heatmapColor(qreal value) {
  struct lc_t {
    qreal limit; int r, g, b;
  };

  static typ::vec<lc_t> lc = {
    {0.00, 255, 255, 255},
    {0.10, 0,   0,   255},
    {0.20, 0,   152, 255},
    {0.30, 0,   190, 0, },
    {0.55, 255, 255, 1, },
    {1.00, 255, 0,   1, },
  };

  value = qBound(0.0, value, 1.0);
  uint count = lc.count(), i;
  for (i = 1; i < count; ++i)
    if (value < lc.at(i).limit)
      break;

  auto& lc1 = lc.at(i-1);
  auto& lc2 = lc.at(qMin(i, count-1));

  auto  frac = (value - lc1.limit) / (lc2.limit - lc1.limit);

  int r = lc1.r + int((lc2.r - lc1.r) * frac);
  int g = lc1.g + int((lc2.g - lc1.g) * frac);
  int b = lc1.b + int((lc2.b - lc1.b) * frac);

  return qRgb(r, g, b);
}

//------------------------------------------------------------------------------
}
// eof
