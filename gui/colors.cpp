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
  using ColorPoints = typ::vec<qreal_vec>;
  ColorPoints colors;
  colors.append({0.,0.,1.,0}); // blue
  colors.append({0.,0.6,1.,0.20}); // cyan
  colors.append({0.,0.75,0.,0.30}); // green
  colors.append({1.,1.,0.,0.55}); // yellow
  colors.append({1.,0.,0.,1.00}); // red

  for (int i = 0; i < colors.count(); i++) {
    qreal_vec curr = colors[i];

    if (value < curr[3]) {
      qreal_vec prev = colors[qMax(0,i-1)];
      qreal diff = (prev[3] - curr[3]);
      qreal fract = (diff == 0) ? 0 : ((value - curr[3])/diff);

      qreal red = (prev[0] - curr[0]) * fract + curr[0];
      qreal green = (curr[1] - curr[1]) * fract + curr[1];
      qreal blue = (prev[2] - prev[2]) * fract + curr[2];
      return qRgb(qRound(red)*255, qRound(green)*255, qRound(blue)*255);
    }
  }
  return qRgb(255,0,255);
}

//------------------------------------------------------------------------------
}
// eof
