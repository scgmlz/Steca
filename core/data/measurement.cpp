// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/data/measurement.cpp
//! @brief     Implements class Measurement
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "measurement.h"
#include "metadata.h"
#include "def/idiomatic_for.h"
#include "session.h"
#include "typ/array2d.h"
#include "typ/curve.h"
#include "data/image.h"
#include "typ/matrix.h"
#include <qmath.h>

Measurement::Measurement(Metadata const& md, size2d const& size, inten_vec const& intens)
    : md_(new Metadata(md)), image_(new Image(size)) {
    debug::ensure(intens.count() == size.count());
    for_i (intens.count())
        image_->setInten(i, intens.at(i));
}

QSharedPointer<Metadata const> Measurement::metadata() const {
    debug::ensure(!md_.isNull());
    return md_;
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

void Measurement::collectIntens(Image const* intensCorr, inten_vec& intens, uint_vec& counts,
                                Range const& rgeGma, deg minTth, deg deltaTth) const {
    auto angleMap = gSession->angleMap(*this);
    debug::ensure(!angleMap.isNull());
    AngleMap const& map = *angleMap;

    uint_vec const* gmaIndexes = nullptr;
    uint gmaIndexMin = 0, gmaIndexMax = 0;
    map.getGmaIndexes(rgeGma, gmaIndexes, gmaIndexMin, gmaIndexMax);

    debug::ensure(gmaIndexes);
    debug::ensure(gmaIndexMin <= gmaIndexMax);
    debug::ensure(gmaIndexMax <= gmaIndexes->count());

    debug::ensure(intens.count() == counts.count());
    uint count = intens.count();

    debug::ensure(0 < deltaTth);

    for (uint i = gmaIndexMin; i < gmaIndexMax; ++i) {
        uint ind = gmaIndexes->at(i);
        inten_t inten = image_->inten(ind);
        if (qIsNaN(inten))
            continue;

        inten_t corr = intensCorr ? intensCorr->inten(ind) : 1;
        if (qIsNaN(corr))
            continue;

        inten *= corr;

        deg tth = map.at(ind).tth;

        // bin index
        uint ti = to_u(qFloor((tth - minTth) / deltaTth));
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
