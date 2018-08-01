//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/gamma_sector.h
//! @brief     Defines class GammaSector
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef GAMMA_SECTOR_H
#define GAMMA_SECTOR_H

#include "core/typ/cached.h"
#include "core/data/dfgram.h"

class Cluster;

//! Element of vector Cluster::gSectors, holds diffractogram for given Cluster and gamma range.

class GammaSector {
public:
    GammaSector() = delete;
    GammaSector(const CachingVector<Cluster, GammaSector>* const v, int i)
        : cachedDfgram_( [v,i]()->Dfgram{return recomputeSectorDfgram(&v->get(i));} )
        , owningVector_(v)
        , i_(i)
        { init(); }
    GammaSector(const GammaSector& rhs) = delete;
    GammaSector(GammaSector&& rhs) = default;
    void init();

    const Dfgram& dfgram() const { return cachedDfgram_.get(); }
    const Curve& curve() const { return cachedDfgram_.get().curve; }

private:
    Kached<Dfgram> cachedDfgram_;
    const CachingVector<Cluster, GammaSector>* const owningVector_;
    int i_;
    friend Dfgram recomputeSectorDfgram(const GammaSector* const gSector);
};

Dfgram recomputeSectorDfgram(const GammaSector* gSector);

#endif // GAMMA_SECTOR_H
