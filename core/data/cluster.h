// ************************************************************************** //
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
// ************************************************************************** //

#ifndef CLUSTER_H
#define CLUSTER_H

#include "core/data/measurement.h"
#include <QSharedPointer> // no auto rm

//! A group of one or more Measurement|s.

//! Base class of Cluster, and also used to hold _all_ loaded Measurements.
//!
//! Measurement|s are always owned by Datafile|s; here they are accessed through const pointers.

class Sequence {
public:
    Sequence() = delete;
    Sequence(Sequence&) = delete;
    Sequence(const QVector<const Measurement*>& measurements);

    const int count() const { return members_.size(); }
    const Measurement* first() const { return members_.first(); }
    const Measurement* at(int i) const { return members_.at(i); }
    const QVector<const Measurement*>& members() const { return members_; }

    deg omg() const;
    deg phi() const;
    deg chi() const;

    Range rgeGma() const;
    Range rgeGmaFull() const;
    Range rgeTth() const;
    Range rgeInten() const;

    shp_Metadata avgeMetadata() const;
    qreal avgMonitorCount() const;
    qreal avgDeltaMonitorCount() const;
    qreal avgDeltaTime() const;

    size2d imageSize() const;
    inten_vec collectIntens(const Image* intensCorr, const Range&) const;
    void calculateAlphaBeta(deg tth, deg gma, deg& alpha, deg& beta) const;

private:
    QVector<const Measurement*> members_;
    shp_Metadata md_; //!< averaged Metadata, cached, computed only once

    void compute_metadata() const;
};


//! A group of one or more Measurement's, with associated information.

class Cluster : public Sequence {
public:
    Cluster() = delete;
    Cluster(Cluster&) = delete;
    Cluster(const QVector<const Measurement*>& measurements,
            const class Datafile& file, const int index, const int offset);

    void setActivated(bool on);

    const class Datafile& file() const { return file_; }
    int index() const { return index_; }
    int offset() const { return offset_; }
    int totalOffset() const;
    bool isIncomplete() const;
    bool isActivated() const { return activated_; }

    Curve toCurve() const;
    Curve toCurve(const Range&) const;
    Curve toCurve(qreal) const;
    Curve toCurve(qreal, const Range&) const;
    qreal normFactor() const;
    qreal normFactor(eNorm norm) const;

private:
    const class Datafile& file_;
    const int index_; //!< index in total list of Cluster|s
    const int offset_; //!< index of first Measurement in file_
    bool activated_ {true}; //!< checked in list, selected for use
};

typedef QSharedPointer<Cluster> shp_Cluster;

Q_DECLARE_METATYPE(shp_Cluster)

#endif // CLUSTER_H
