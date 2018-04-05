//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/algo/project_intensity.cpp
//! @brief     Implements function projectIntensity
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "project_intensity.h"
#include "core/session.h"
#include <qmath.h>

//! Increments intens and counts.
void algo::projectIntensity(
    QVector<float>& intens, QVector<int>& counts,
    const Measurement& measurement, const Range& rgeGma, deg minTth, deg deltaTth)
{
    const shp_AngleMap& angleMap = gSession->angleMap(measurement);
    ASSERT(!angleMap.isNull());
    const AngleMap& map = *angleMap;

    const QVector<int>* gmaIndexes = nullptr;
    int gmaIndexMin = 0, gmaIndexMax = 0;
    map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    ASSERT(gmaIndexes);
    ASSERT(gmaIndexMin <= gmaIndexMax);
    ASSERT(gmaIndexMax <= gmaIndexes->count());

    ASSERT(intens.count() == counts.count());
    int count = intens.count();

    ASSERT(0 < deltaTth);

    for (int i = gmaIndexMin; i < gmaIndexMax; ++i) {
        int ind = gmaIndexes->at(i);
        float inten = measurement.image().inten(ind);
        if (qIsNaN(inten))
            continue;

        float corr
            = gSession->corrset().isActive() ? gSession->corrset().intensCorr()->inten(ind) : 1;
        if (qIsNaN(corr))
            continue;
        inten *= corr;

        // bin index
        deg tth = map.at(ind).tth;
        int ti = qFloor((tth - minTth) / deltaTth);
        ASSERT(ti <= count);
        ti = qMin(ti, count - 1); // it can overshoot due to floating point calculation

        intens[ti] += inten;
        counts[ti] += 1;
    }
}
