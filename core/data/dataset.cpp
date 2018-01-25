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

#include "core/session.h"
#include "core/loaders/loaders.h"
#include <QDir>

void Dataset::clear() {
    while (countFiles())
        removeFile(0);
}

//! Returns true if some file was loaded
void Dataset::addGivenFiles(const QStringList& filePaths) THROWS {
    for (const QString& path: filePaths) {
        if (path.isEmpty() || hasFile(path))
            continue;
        QSharedPointer<const Rawfile> rawfile = load::loadRawfile(path);
        if (rawfile.isNull())
            continue;
        gSession->setImageSize(rawfile->imageSize());
        files_.push_back(Datafile(rawfile));
    }
    onFileChanged();
}

void Dataset::removeFile(int i) { // TODO rm arg
    files_.erase(files_.begin()+i);
    onFileChanged();
    gSession->updateImageSize();
    // setHighlight(i-1); // TODO
}

void Dataset::setHighlight(const Cluster* cluster) {
    if (cluster==highlight_)
        return;
    highlight_ = cluster;
    TR("DS::sH(C) emit>");
    emit gSession->sigHighlight();
    TR("DS::sH(C) emit<");
}

void Dataset::setHighlight(const Datafile* file) {
    if (highlight_ && file==&highlight_->file())
        return;
    for (const shp_Cluster& cluster : allClusters_) {
        if (&cluster->file()==file) {
            highlight_ = cluster.data();
            TR("DS::sH(F) emit>");
            emit gSession->sigHighlight();
            TR("DS::sH(F) emit<");
            return;
        }
    }
    TR("DS::sH(F) never");
    NEVER
}

void Dataset::setBinning(int by) {
    if (by==binning_)
        return;
    binning_ = by;
    onClusteringChanged();
}

void Dataset::setDropIncomplete(bool on) {
    if (on==dropIncomplete_)
        return;
    dropIncomplete_ = on;
    onClusteringChanged();
}

void Dataset::activateCluster(int index, bool on) {
    allClusters_.at(index)->setActivated(on);
}

void Dataset::onFileChanged() {
    int idx = 0;
    int cnt = 0;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.count();
    }
    onClusteringChanged();
    TR("DS::oFC emit>");
    emit gSession->sigFiles();
    TR("DS::oFC emit<");
}

void Dataset::onClusteringChanged() {
    allClusters_.clear();
    hasIncomplete_ = false;
    for (const Datafile& file : files_) {
        for (int i=0; i<file.count(); i+=binning_) {
            if (i+binning_>file.count()) {
                hasIncomplete_ = true;
                if (dropIncomplete_)
                    break;
            }
            int ii;
            QVector<const Measurement*> group;
            for (ii=i; ii<file.count() && ii<i+binning_; ii++)
                group.append(file.raw_->measurements().at(ii));
            shp_Cluster cluster(new Cluster(group, file, allClusters_.size(), i));
            allClusters_.append(cluster);
        }
    }
    TR("DS::oCC emit>");
    emit gSession->sigClusters();
    TR("DS::oCC emit<");
}

void Dataset::assembleExperiment() {
    experiment_ = {};

    for (const Datafile& file : files_) {
        for (int i=0; i<file.count(); i+=binning_) {
            int ii;
            QVector<const Measurement*> group;
            for (ii=i; ii<file.count() && ii<i+binning_; ii++)
                group.append(file.raw_->measurements().at(ii));
            experiment_.appendHere(new Cluster(group, file, experiment_.size(), i));
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
