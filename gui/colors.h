// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/colors.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef COLORS_H
#define COLORS_H

#include "typ/typ_types.h"

#include <QRgb>

namespace gui {

QRgb intenImage(inten_t inten, inten_t maxInten, bool curved);
QRgb intenGraph(inten_t inten, inten_t maxInten);
QRgb heatmapColor(inten_t value);
}
#endif // COLORS_H
