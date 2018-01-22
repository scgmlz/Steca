// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dataset.cpp
//! @brief     Implements class Dataset
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "core/data/dataset.h"
#include "core/calc/reflection.h"
#include "core/calc/reflection_info.h"
#include "core/data/angle_map.h"
#include "core/data/rawfile.h"
#include "core/data/experiment.h"
#include "core/data/image.h"
#include "core/session.h"
#include "core/typ/async.h"
#include "core/typ/cache.h"
#include "core/typ/singleton.h"
#include "core/loaders/loaders.h"
#include <QDir>
#include <QSharedPointer>

void Dataset::clear() {
    while (countFiles())
        removeFile(0);
}

//! Returns true if some file was loaded
bool Dataset::addGivenFiles(const QStringList& filePaths) THROWS {
    bool ret = false;
    for (const QString& path: filePaths) {
        if (path.isEmpty() || hasFile(path))
            continue;
        QSharedPointer<const Rawfile> rawfile = load::loadRawfile(path);
        if (rawfile.isNull())
            continue;
        ret = true;
        gSession->setImageSize(rawfile->imageSize());
        files_.push_back(Datafile(rawfile));
    }
    updateCache();
    return ret;
}

void Dataset::removeFile(int i) { // TODO rm arg
    files_.erase(files_.begin()+i);
    updateCache();
    gSession->updateImageSize();
    // setHighlight(i-1); // TODO
}

void Dataset::updateCache() {
    int idx = 0;
    int cnt = 0;
    QVector<const Measurement*> all;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.count();
        for (const Measurement* one: file.raw_->measurements())
            all.append(one);
    }
    allMeasurements_ = all.count() ? QSharedPointer<Sequence> (new Sequence(all)) : nullptr;
}

void Dataset::setHighlight(const Datafile& file) {
    int i = file.index_;
    if (i==highlight_)
        return;
    highlight_ = i;
    emit gSession->sigFileHighlight(file);
}

void Dataset::assembleExperiment(const vec<int> fileNums, const int combineBy) {
    filesSelection_ = fileNums;
    experiment_ = { combineBy };

    for (int jFile : filesSelection_) {
        const Datafile& file = files_.at(jFile);
        for (int i=0; i<file.count(); i+=combineBy) {
            int ii;
            QVector<const Measurement*> group;
            for (ii=i; ii<file.count() && ii<i+combineBy; ii++)
                group.append(file.raw_->measurements().at(ii));
            shp_Cluster cd(new Cluster(group, file, i));
            experiment_.appendHere(cd);
        }
    }
}

bool Dataset::hasFile(rcstr fileName) const {
    QFileInfo fileInfo(fileName);
    for (const Datafile& file : files_)
        if (fileInfo == file.raw_->fileInfo())
            return true;
    return false;
}

QJsonArray Dataset::to_json() const {
    QJsonArray ret;
    for (const Datafile& file : files_) {
        str relPath = QDir::current().relativeFilePath(file.raw_->fileInfo().absoluteFilePath());
        ret.append(relPath);
    }
    return ret;
}
