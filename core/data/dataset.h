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
#include "core/data/datafile.h"
#include "core/data/experiment.h"
#include "core/data/image.h"
#include "core/typ/async.h"
#include "core/typ/cache.h"
#include "core/typ/singleton.h"
#include <QSharedPointer> // no auto rm

//! Loaded Datafile|s. Does not include the correction file.

class Dataset final {
public:
    // Modifying methods:
    void clear();
    bool addGivenFiles(const QStringList& filePaths) THROWS;
    void removeFile(int i);
    void assembleExperiment(const vec<int>, const int);

    // Const methods:
    int count() const { return files_.count(); }
    const Datafile* file(int i) const { return files_[i].data(); }
    int offset(const Datafile* file) const { return offsets_[file]; }
    QJsonArray to_json() const;

    vec<int> const& filesSelection() const { return filesSelection_; }

    Experiment experiment_; // cluster collected ...

private:
    QVector<QSharedPointer<const Datafile>> files_; //!< data files
    QMap<const Datafile*,int> offsets_; //!< first index in total list of Measurement|s

    vec<int> filesSelection_; // from these files

    void computeOffsets();

    bool hasFile(rcstr fileName) const;
};

#endif // DATASET_H
