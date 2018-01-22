// ************************************************************************** //
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
// ************************************************************************** //

#ifndef DATASET_H
#define DATASET_H

#include "core/calc/lens.h"
#include "core/calc/reflection.h"
#include "core/calc/reflection_info.h"
#include "core/data/angle_map.h"
#include "core/data/rawfile.h"
#include "core/data/experiment.h"
#include "core/data/image.h"
#include "core/typ/async.h"
#include "core/typ/cache.h"
#include <QSharedPointer> // no auto rm
#include <vector>

//! A RawFile and associated information.

class Datafile final {

private:
public:
    Datafile() = delete;
    Datafile(Datafile&&) = default;
    Datafile& operator=(const Datafile&) = default;
    Datafile(const QSharedPointer<const Rawfile>& raw) : raw_(raw) {}

    int count() const { return raw_->count(); }
    QString name() const { return raw_->fileName(); }

    QSharedPointer<const Rawfile> raw_; //!< owned by this
    int index_; //!< index in files_
    int offset_;  //!< first index in total list of Measurement|s
};


//! Loaded Datafile|s. Does not include the correction file.

class Dataset final {
public:
    // Modifying methods:
    void clear();
    bool addGivenFiles(const QStringList& filePaths) THROWS;
    void removeFile(int i);
    void setHighlight(const Datafile&);
    void setBinning(int by);
    void assembleExperiment(const vec<int>);

    // Const methods:
    int countFiles() const { return files_.size(); }
//    int countClusters() const { return allClusters_ ? allClusters_->count() : 0; }
    const Datafile& file(int i) const { return files_[i]; }
    int offset(const Datafile& file) const { return file.offset_; }
    int highlight() const { return highlight_; }
    int binning() const { return binning_; }
    QJsonArray to_json() const;

    vec<int> const& filesSelection() const { return filesSelection_; }

    Experiment experiment_; // cluster collected ...

private:
    std::vector<Datafile> files_; //!< data files
//    QSharedPointer<Sequence> allClusters_ {nullptr}; // TODO precompute everything, then make this const

    int highlight_ {0}; //!< index of highlighted file
    vec<int> filesSelection_; // from these files
    int binning_ {1}; //!< bin so many measurements into one cluster

    void updateCache();

    bool hasFile(rcstr fileName) const;
};

#endif // DATASET_H
