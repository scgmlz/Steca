//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/sector.cpp
//! @brief     Implements class GammaSector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/sector.h"
#include "core/session.h"
#include "core/algo/collect_intensities.h"
#include "qcr/base/debug.h"
#include <qmath.h>

void GammaSector::init()
{
    const auto*const v = owningVector_;
    const int i = i_;
    QObject::connect(gSession, &Session::sigDetector,
                     [v, i]() { v->get(i).cachedDfgram_.invalidate(); });
}

Dfgram recomputeSectorDfgram(const GammaSector* const gSector)
{
    // qDebug() << "recompute dfgram" << gSector->owningCluster_->index() << "for sector i,n =" << gSector->i_ << gSector->n_;
    return gSector->owningVector_->owner->segmentalDfgram(gSector->i_);
}
