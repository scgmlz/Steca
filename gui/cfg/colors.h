// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/colors.h
//! @brief     Defines color maps in namespace colormap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef COLORS_H
#define COLORS_H

#include "core/typ/types.h"
#include <QColor>
#include <QRgb>

namespace colormap {
QRgb intenImage(inten_t inten, inten_t maxInten, bool curved);
QColor intenGraph(inten_t inten, inten_t maxInten);
}

#endif // COLORS_H
