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

//! A group of one or more Measurement's

class Cluster final : public vec<shp_Measurement> {
public:
    Cluster() = delete;
    Cluster(Cluster&) = delete;
    Cluster(const vec<shp_Measurement>& measurements);


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
    shp_Metadata md_; //!< averaged Metadata, cached, computed only once

    void compute_metadata() const;
};

typedef QSharedPointer<Cluster> shp_Cluster;

Q_DECLARE_METATYPE(shp_Cluster)

#endif // CLUSTER_H
