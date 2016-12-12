/*******************************************************************************
 * REVIEW: STeCa2 - StressTextureCalculator ver. 2
 *
 * Copyright (C) 2016 Forschungszentrum Jülich GmbH 2016
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the COPYING and AUTHORS files for more details.
 ******************************************************************************/

#include "colors.h"
#include "lib/typ/typ_vec.h"

#include <QColor>
#include <qmath.h>

namespace gui {
//------------------------------------------------------------------------------

QRgb intenImage(inten_t inten, inten_t maxInten, bool curved) {
  if (qIsNaN(inten))
    return qRgb(0x00, 0xff, 0xff);
  if (qIsInf(inten))
    return qRgb(0xff, 0xff, 0xff);

  if (qIsNaN(maxInten) || maxInten <= 0)
    return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  if (curved && inten > 0)
    inten = qPow(inten, .6f);

  inten_t const low = .25f, mid = .5f, high = .75f;
  if (inten < low)
    return qRgb(int(0xff * inten * 4), 0, 0);
  if (inten < mid)
    return qRgb(0xff, int(0xff * (inten - low) * 4), 0);
  if (inten < high)
    return qRgb(int(0xff - (0xff * (inten - mid) * 4)), 0xff,
                int(0xff * (inten - mid) * 4));
  return qRgb(int(0xff * (inten - high) * 4), 0xff, 0xff);
}

QRgb intenGraph(inten_t inten, inten_t maxInten) {
  if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
    return qRgb(0x00, 0x00, 0x00);

  inten /= maxInten;

  return qRgb(0, 0, int(0xff * (1 - inten / 3)));
}

QRgb heatmapColor(inten_t value) {
  struct lc_t {
    inten_t limit; int r, g, b;
  };

  static typ::vec<lc_t> lc = {
    {0.00f, 255, 255, 255},
    {0.10f, 0,   0,   255},
    {0.20f, 0,   152, 255},
    {0.30f, 0,   190, 0  },
    {0.55f, 255, 255, 1  },
    {1.00f, 255, 0,   1  },
  };

  value = qBound(0.0f, value, 1.0f);
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
