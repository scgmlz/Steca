//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/sector.h
//! @brief     Defines class Sector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef SECTOR_H
#define SECTOR_H

#include "core/typ/cached.h"
#include "core/data/dfgram.h"

class Cluster;

//! Element of vector Cluster::sectors, holds diffractogram for given Cluster and gamma range.

class Sector {
public:
    Sector() = delete;
    Sector(const CachingVector<Cluster, Sector>* const v, int i)
        : cachedDfgram_( [v,i]()->Dfgram{return recomputeSectorDfgram(&v->get(i));} )
        , owningVector_(v)
        , i_(i)
        { init(); } // TODO try with Kached::get(parent) instead
    Sector(const Sector& rhs) = delete;
    Sector(Sector&& rhs) = default;
    void init();

    const Dfgram& dfgram() const { return cachedDfgram_.get(); }
    const Curve& curve() const { return cachedDfgram_.get().curve; }

private:
    Cached<Dfgram> cachedDfgram_;
    const CachingVector<Cluster, Sector>* const owningVector_;
    int i_;
    friend Dfgram recomputeSectorDfgram(const Sector* const sector);
};

Dfgram recomputeSectorDfgram(const Sector* sector);

#endif // SECTOR_H
