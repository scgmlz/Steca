// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/measurement.cpp
//! @brief     Implements class Measurement
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/session.h"
#include <qmath.h>

Measurement::Measurement(
    const int position, const Metadata& md, const size2d& size, const inten_vec& intens)
    : position_(position)
    , md_(new Metadata(md))
    , image_(new Image(size))
{
    ASSERT(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

Range Measurement::rgeGma() const {
    return gSession->angleMap(*this)->rgeGma();
}

Range Measurement::rgeGmaFull() const {
    return gSession->angleMap(*this)->rgeGmaFull();
}

Range Measurement::rgeTth() const {
    return gSession->angleMap(*this)->rgeTth();
}

Range Measurement::rgeInten() const {
    return image_->rgeInten();
}

size2d Measurement::imageSize() const {
    return image_->size();
}

void Measurement::collectIntens(
    const Image* intensCorr, inten_vec& intens, vec<int>& counts,
    const Range& rgeGma, deg minTth, deg deltaTth) const {

    const shp_AngleMap& angleMap = gSession->angleMap(*this);
    ASSERT(!angleMap.isNull());
    const AngleMap& map = *angleMap;

    vec<int> const* gmaIndexes = nullptr;
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
        inten_t inten = image_->inten(ind);
        if (qIsNaN(inten))
            continue;

        inten_t corr = intensCorr ? intensCorr->inten(ind) : 1;
        if (qIsNaN(corr))
            continue;

        inten *= corr;

        deg tth = map.at(ind).tth;

        // bin index
        int ti = qFloor((tth - minTth) / deltaTth);
        ASSERT(ti <= count);
        ti = qMin(ti, count - 1); // it can overshoot due to floating point calculation

        intens[ti] += inten;
        counts[ti] += 1;
    }
}

deg Measurement::midTth() const { return md_->motorTth; }

qreal Measurement::monitorCount() const { return md_->monitorCount; }
qreal Measurement::deltaMonitorCount() const { return md_->deltaMonitorCount; }
qreal Measurement::deltaTime() const { return md_->deltaTime; }

deg Measurement::omg() const { return md_->motorOmg; }
deg Measurement::phi() const { return md_->motorPhi; }
deg Measurement::chi() const { return md_->motorChi; }
