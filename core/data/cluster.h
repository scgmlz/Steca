//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/cluster.h
//! @brief     Defines classes Sequence, Cluster
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef CLUSTER_H
#define CLUSTER_H

#include "core/typ/cached.h"
#include "core/raw/measurement.h"
class Curve;

//! A group of one or more Measurement|s.

//! Base class of Cluster, and also used to hold _all_ loaded Measurements.
//!
//! Measurement|s are always owned by Datafile|s; here they are accessed through const pointers.

class Sequence {
public:
    Sequence() = delete;
    Sequence(const Sequence&) = delete;
    Sequence(const std::vector<const Measurement*>& measurements);

    const int count() const { return members_.size(); }
    const Measurement* first() const { return members_.front(); }
    const Measurement* at(int i) const { return members_.at(i); }
    const std::vector<const Measurement*>& members() const { return members_; }

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;
    Range rgeInten() const;
    double normFactor() const;

    const Metadata& avgMetadata() const { return metadata_; }

    size2d imageSize() const;

private:
    double avgMonitorCount() const;
    double avgDeltaMonitorCount() const;
    double avgTime() const;
    double avgDeltaTime() const;

    const std::vector<const Measurement*> members_; //!< points to Dataset:vec<Datafile>:vec<M'ments>
    const Metadata metadata_; //!< averaged Metadata

    Metadata computeAvgMetadata() const;
};

class Cluster;
class GammaSector;

Curve recomputeSectorDfgram(const GammaSector* gSector);

class GammaSector {
public:
    GammaSector() = delete;
    GammaSector(const Cluster* const o, int i, int n) : owningCluster_(o), i_(i), n_(n) {}
    GammaSector(const GammaSector& rhs) = delete;
    GammaSector(GammaSector&& rhs)
        : cachedCurve_(this, rhs.cachedCurve_)
        , owningCluster_(rhs.owningCluster_)
        , i_(rhs.i_), n_(rhs.n_)
    {}

    void init();

    const Curve& curve() const { return cachedCurve_.get(); }

private:
    CachedPayload<GammaSector, Curve, recomputeSectorDfgram> cachedCurve_;
    const Cluster* const owningCluster_;
    int i_;
    int n_;
    friend Curve recomputeSectorDfgram(const GammaSector* const gSector);
};

//! A group of one or more Measurement's, with associated information.

class Cluster : public Sequence {
public:
    Cluster() = delete;
    Cluster(const Cluster&) = delete;
    Cluster(const std::vector<const Measurement*>& measurements,
            const class Datafile& file, const int index, const int offset);

    void setActivated(bool on);

    const class Datafile& file() const { return file_; }
    int index() const { return index_; }
    int offset() const { return offset_; }
    int totalOffset() const;
    bool isIncomplete() const;
    bool isActivated() const { return activated_; }
    Curve segmentalDfgram(int, int) const;

    CachingVector<Cluster, GammaSector> gSectors; //! One Curve per gamma section
    GammaSector& currentGammaSector() const;

private:
    const class Datafile& file_;
    const int index_; //!< index in total list of Cluster|s
    const int offset_; //!< index of first Measurement in file_
    bool activated_ {true}; //!< checked in list, selected for use
};

#endif // CLUSTER_H
