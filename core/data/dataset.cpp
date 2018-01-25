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

// ************************************************************************** //
//  class Datafile
// ************************************************************************** //

Qt::CheckState Datafile::activated() const {
    bool allActivated = true;
    bool noneActivated = true;
    for (const Cluster* cluster : clusters_) {
        if (cluster->isActivated())
            noneActivated = false;
        else
            allActivated = false;
    }
    if (allActivated)
        return Qt::Checked;
    else if (noneActivated)
        return Qt::Unchecked;
    return Qt::PartiallyChecked;
}

// ************************************************************************** //
//  class Dataset
// ************************************************************************** //

void Dataset::clear() {
    files_.clear();
    onFileChanged();
    gSession->updateImageSize();
    gSession->setImageCut(true, false, ImageCut());
}

void Dataset::removeFile(int i) { // TODO rm arg
    files_.erase(files_.begin()+i);
    onFileChanged();
    gSession->updateImageSize();
    if (files_.empty())
        gSession->setImageCut(true, false, ImageCut());
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

void Dataset::setHighlight(const Cluster* cluster) {
    if (cluster==highlight_)
        return;
    highlight_ = cluster;
    emit gSession->sigHighlight();
}

void Dataset::setHighlight(const Datafile* file) {
    if (highlight_ && file==&highlight_->file())
        return;
    for (const shp_Cluster& cluster : allClusters_) {
        if (&cluster->file()==file) {
            highlight_ = cluster.data();
            emit gSession->sigHighlight();
            return;
        }
    }
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
    updateExperiment();
    emit gSession->sigActivated();
}

void Dataset::flipClusterActivation(int index) {
    allClusters_.at(index)->setActivated(!allClusters_.at(index)->isActivated());
    updateExperiment();
    emit gSession->sigActivated();
}

void Dataset::cycleFileActivation(int index) {
    const Datafile& fil = file(index);
    bool on = fil.activated()!=Qt::Checked;
    for (Cluster* cluster : fil.clusters_)
        cluster->setActivated(on);
    updateExperiment();
    emit gSession->sigActivated();
}

void Dataset::onFileChanged() {
    int idx = 0;
    int cnt = 0;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.count();
    }
    updateClusters();
    emit gSession->sigFiles();
    emit gSession->sigClusters();
    emit gSession->sigActivated();
}

void Dataset::onClusteringChanged() {
    updateClusters();
    emit gSession->sigClusters();
    emit gSession->sigActivated();
}

void Dataset::updateClusters() {
    allClusters_.clear();
    hasIncomplete_ = false;
    for (Datafile& file : files_) {
        file.clusters_.clear();
        for (int i=0; i<file.count(); i+=binning_) {
            if (i+binning_>file.count()) {
                hasIncomplete_ = true;
                if (dropIncomplete_)
                    break;
            }
            QVector<const Measurement*> group;
            for (int ii=i; ii<file.count() && ii<i+binning_; ii++)
                group.append(file.raw_->measurements().at(ii));
            shp_Cluster cluster(new Cluster(group, file, allClusters_.size(), i));
            allClusters_.append(cluster);
            file.clusters_.push_back(cluster.data());
        }
    }
    updateExperiment();
}

void Dataset::updateExperiment() {
    experiment_ = {};
    for (const shp_Cluster& cluster : allClusters_) {
        if (cluster->isActivated())
            experiment_.appendHere(cluster.data());
    }
    qDebug() << "assembled experiment => size=" << experiment_.size();
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
