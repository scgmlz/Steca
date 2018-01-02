// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/colors.cpp
//! @brief     Implements ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "colors.h"
#include <qmath.h>

namespace gui {

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
        return qRgb(int(0xff - (0xff * (inten - mid) * 4)), 0xff, int(0xff * (inten - mid) * 4));
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
        inten_t limit;
        int r, g, b;
    };

    static vec<lc_t> lc = {
        { 0.00f, 255, 255, 255 }, { 0.10f, 0, 0, 255 },   { 0.20f, 0, 152, 255 },
        { 0.30f, 0, 190, 0 },     { 0.55f, 255, 255, 1 }, { 1.00f, 255, 0, 1 },
    };

    value = qBound(0.0f, value, 1.0f);
    uint count = lc.count(), i;
    for (i = 1; i < count; ++i)
        if (value < lc.at(i).limit)
            break;

    auto& lc1 = lc.at(i - 1);
    auto& lc2 = lc.at(qMin(i, count - 1));

    auto frac = (value - lc1.limit) / (lc2.limit - lc1.limit);

    int r = lc1.r + int((lc2.r - lc1.r) * frac);
    int g = lc1.g + int((lc2.g - lc1.g) * frac);
    int b = lc1.b + int((lc2.b - lc1.b) * frac);

    return qRgb(r, g, b);
}
}
