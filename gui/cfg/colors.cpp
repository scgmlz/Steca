// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/cfg/colors.cpp
//! @brief     Implements color maps in namespace colormap
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/cfg/colors.h"
#include <qmath.h>

QRgb colormap::intenImage(inten_t inten, inten_t maxInten, bool curved) {
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
        return qRgb(int(0xff - (0xff * (inten - mid) * 4)), 0xff, int(0xff * (inten - mid) * 4));
    return qRgb(int(0xff * (inten - high) * 4), 0xff, 0xff);
}

QColor colormap::intenGraph(inten_t inten, inten_t maxInten) {
    if (!qIsFinite(inten) || qIsNaN(maxInten) || maxInten <= 0)
        return { qRgb(0x00, 0x00, 0x00) };

    inten /= maxInten;

    return { qRgb(0, 0, int(0xff * (1 - inten / 3))) };
}
