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

#ifndef SESSION_H
#define SESSION_H

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
    Dataset();

    // Modifying methods:
    void clear();
    bool addGivenFiles(const QStringList& filePaths) THROWS;
    void removeFile(int i);

    // Const methods:
    int count() const { return files_.count(); }
    const Datafile* file(int i) const { return files_[i].data(); }

    vec<int> const& filesSelection() const { return filesSelection_; }

private:
    QVector<QSharedPointer<Datafile>> files_; //!< data files
    vec<int> filesSelection_; // from these files

    bool hasFile(rcstr fileName) const;
};

#endif // SESSION_H
