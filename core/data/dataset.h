//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dataset.h
//! @brief     Defines class Dataset
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef DATASET_H
#define DATASET_H

#include "core/data/cluster.h"
#include "qcr/engine/cell.h"
#include <memory>

//! A Rawfile and associated information.

class Datafile {
public:
    Datafile() = delete;
    Datafile(Rawfile&& raw) : raw_(std::move(raw)) {}
    Datafile(const Datafile&) = delete;
    Datafile(Datafile&&) = default;
    Datafile& operator=(Datafile&&) = default;

    int numMeasurements() const { return raw_.numMeasurements(); }
    int index() const { return index_; }
    QString name() const { return raw_.fileName(); }
    Qt::CheckState activated() const;

    int offset_;  //!< first index in total list of Measurement|s

private:
    friend class Dataset;
    friend class HighlightedData;

    Rawfile raw_; //!< owned by this
    int index_; //!< index in files_
    std::vector<Cluster*> clusters_; //!< back links to Cluster|s made from this,
                                     //!< set by Dataset::updateClusters
};


//! Pointer to highlighted data.

class HighlightedData { // TODO ? mv to cluster.h ?
public:
    HighlightedData() {}
    HighlightedData(const HighlightedData&) = delete;

    void clear();
    void setFile(int);
    void setCluster(int);
    void reset();

    const Cluster* cluster() const { return current_; }
    Cluster* cluster() { return current_; }

private:
    Cluster* current_ { nullptr };
};


//! Loaded Datafile|s. Does not include the correction file.

class Dataset {
public:
    Dataset();
    Dataset(const Dataset&) = delete;

    void fromJson(const JsonObj& obj);
    void clear();
    void addGivenFiles(const QStringList& filePaths);
    void removeFile();
    void setClusterActivation(int index, bool on);
    void setFileActivation(int index, bool on);

    HighlightedData& highlight() { return highlight_; }
    const HighlightedData& highlight() const { return highlight_; }

    QJsonObject toJson() const;
    int countFiles() const { return files_.size(); }
    const Datafile& fileAt(int i) const { return files_.at(i); }
    int offset(const Datafile& file) const { return file.offset_; }
    bool hasIncomplete() const { return hasIncomplete_; }
    std::vector<const Cluster*> activeClustersList() const;

    QcrCell<int> binning {1};             //!< bin so many Measurement|s into one cluster
    QcrCell<bool> dropIncomplete {false}; //!< drop Clusters with less than 'binning' members.

    std::vector<std::unique_ptr<Cluster>> allClusters; //!< all Cluster|s are owned by this

private:
    std::vector<Datafile> files_; //!< loaded Datafile|s only live here
    // leave this a unique_ptr because other vectors backlink through Cluster* pointers

    bool hasIncomplete_ {false}; //!< current binning does result in at least one incomplete cluster

    HighlightedData highlight_; //!< wraps pointer to highlighted Datafile and Cluster

    void onFileChanged();
    void onClusteringChanged();
    void updateClusters();
    void updateActiveClusters();

    bool hasFile(const QString& fileName) const;
};

#endif // DATASET_H
