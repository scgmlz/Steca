//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/sector.cpp
//! @brief     Implements class Sector
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

void Sector::init()
{
    const auto*const v = owningVector_;
    const int i = i_;
    QObject::connect(gSession, &Session::sigDetector,
                     [v, i]() { v->get(i).cachedDfgram_.invalidate(); });
}

Dfgram recomputeSectorDfgram(const Sector* const sector)
{
    // qDebug() << "recompute dfgram" << sector->owningCluster_->index() << "for sector i,n =" << sector->i_ << sector->n_;
    return sector->owningVector_->owner->segmentalDfgram(sector->i_);
}
