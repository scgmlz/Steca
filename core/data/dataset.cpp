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

void Dataset::removeFile() {
    int i = highlightedFileIndex();
    // first unhighlight
    qDebug() << "NOW UNHIGH";
    if (i>0)
        highlightFile(i-1);
    else if (countFiles())
        highlightFile(0);
    else {
        unsetHighlight();
    }
    // only then remove
    qDebug() << "NOW REM";
    files_.erase(files_.begin()+i);
    qDebug() << "NOW SIG";
    onFileChanged();
    qDebug() << "NOW IMG";
    gSession->updateImageSize();
    if (files_.empty())
        gSession->setImageCut(true, false, ImageCut());
}

void Dataset::addGivenFiles(const QStringList& filePaths) THROWS {
    for (const QString& path: filePaths) {
        if (path.isEmpty() || hasFile(path))
            continue;
        QSharedPointer<const Rawfile> rawfile = load::loadRawfile(path);
        if (rawfile.isNull())
            continue;
        gSession->setImageSize(rawfile->imageSize());
        files_.push_back(Datafile(rawfile));
        onFileChanged();
    }
    if (!highlight_ && countFiles())
        setHighlight(&files_[0]);
}

void Dataset::highlightFile(int i) {
    if (countFiles() && i>=0)
        setHighlight(files_.at(i).clusters_.front());
    else
        unsetHighlight();
}

void Dataset::highlightCluster(int i) {
    if (countFiles() && i>=0)
        setHighlight(allClusters_.at(i).data());
    else
        unsetHighlight();
}

void Dataset::unsetHighlight() {
    highlight_ = nullptr;
    emit gSession->sigHighlight();
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
    debug::ensure(file);
    highlight_ = file->clusters_.front();
    emit gSession->sigHighlight();
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
    const Datafile& fil = fileAt(index);
    bool on = fil.activated()!=Qt::Checked;
    for (Cluster* cluster : fil.clusters_)
        cluster->setActivated(on);
    updateExperiment();
    emit gSession->sigActivated();
}

void Dataset::setSelectedMeasurement(int val) {
    selectedMeasurement_ = qMin( val, highlightedCluster()->count()-1 );
    qDebug() << "setSelectedMeasurement " << val << " -> " << selectedMeasurement_;
    emit gSession->sigHighlight();
}


void Dataset::onFileChanged() {
    int idx = 0;
    int cnt = 0;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.count();
    }
    qDebug() << "Ds:oFS 1";
    updateClusters();
    qDebug() << "Ds:oFS 2";
    emit gSession->sigFiles();
    qDebug() << "Ds:oFS 3";
    emit gSession->sigClusters();
    qDebug() << "Ds:oFS 4";
    emit gSession->sigActivated();
    qDebug() << "Ds:oFS 5";
}

void Dataset::onClusteringChanged() {
    updateClusters();
    highlightCluster(0); // Simplest way to update highlight.
                         // Alternatively, we could keep pointing to the first measurement
                         // of the previously highlighted cluster. Then, we should also
                         // keep the selected Image constant.
    emit gSession->sigClusters();
    emit gSession->sigActivated();
    emit gSession->sigHighlight();
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

int Dataset::countFiles() const {
    return files_.size();
}

int Dataset::countClusters() const {
    return allClusters_.count();
}

const Datafile& Dataset::fileAt(int i) const {
    debug::ensure(countFiles());
    debug::ensure(0<=i && i<countFiles());
    return files_[i];
}

const Cluster& Dataset::clusterAt(int i) const {
    debug::ensure(countClusters());
    debug::ensure(0<=i && i<countClusters());
    return *allClusters_[i];
}

const Cluster* Dataset::highlightedCluster() const {
    return highlight_;
}

int Dataset::highlightedClusterIndex() const {
    return highlight_ ? highlight_->index() : -1;
}

const Datafile* Dataset::highlightedFile() const {
    return highlight_ ? &highlight_->file() : nullptr;
}

int Dataset::highlightedFileIndex() const {
    const Datafile* file = highlightedFile();
    return file ? file->index_ : -1;
}

int Dataset::selectedMeasurementIndex() const {
    return selectedMeasurement_;
}

const Measurement* Dataset::selectedMeasurement() const {
    return highlightedCluster()->at(selectedMeasurement_);
}

QJsonArray Dataset::to_json() const {
    QJsonArray ret;
    for (const Datafile& file : files_) {
        str relPath = QDir::current().relativeFilePath(file.raw_->fileInfo().absoluteFilePath());
        ret.append(relPath);
    }
    return ret;
}

bool Dataset::hasFile(rcstr fileName) const {
    QFileInfo fileInfo(fileName);
    for (const Datafile& file : files_)
        if (fileInfo == file.raw_->fileInfo())
            return true;
    return false;
}
