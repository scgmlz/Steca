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

#include "measurement.h"
#include "core/session.h"
#include <qmath.h>

Measurement::Measurement(
    const class Rawfile* file, const int position, const Metadata& md, size2d const& size,
    inten_vec const& intens)
    : file_(file)
    , position_(position)
    , md_(new Metadata(md))
    , image_(new Image(size))
{
    debug::ensure(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

int Measurement::totalPosition() const {
    return gSession->dataset().offset(file_) + position_;
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
    debug::ensure(!angleMap.isNull());
    AngleMap const& map = *angleMap;

    vec<int> const* gmaIndexes = nullptr;
    int gmaIndexMin = 0, gmaIndexMax = 0;
    map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    debug::ensure(gmaIndexes);
    debug::ensure(gmaIndexMin <= gmaIndexMax);
    debug::ensure(gmaIndexMax <= gmaIndexes->count());

    debug::ensure(intens.count() == counts.count());
    int count = intens.count();

    debug::ensure(0 < deltaTth);

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
        debug::ensure(ti <= count);
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
