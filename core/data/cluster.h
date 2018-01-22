// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/cluster.h
//! @brief     Defines class Cluster
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CLUSTER_H
#define CLUSTER_H

#include "core/typ/angles.h"
#include "core/data/image.h"
#include "core/data/measurement.h"
#include <QSharedPointer> // no auto rm

//! A group of one or more Measurement's, with associated information.

class Cluster final {
public:
    Cluster() = delete;
    Cluster(Cluster&) = delete;
    Cluster(const class Datafile& file, const int offset,
            const QVector<const Measurement*>& measurements);

    const class Datafile& file() const { return file_; }
    const int offset() const { return offset_; }
    const int totalOffset() const;
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
    const class Datafile& file_;
    const int offset_; //! index of first Measurement in file_
    QVector<const Measurement*> members_;
    shp_Metadata md_; //!< averaged Metadata, cached, computed only once

    void compute_metadata() const;
};

typedef QSharedPointer<Cluster> shp_Cluster;

Q_DECLARE_METATYPE(shp_Cluster)

#endif // CLUSTER_H
