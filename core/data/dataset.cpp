//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "core/session.h"
#include "core/data/cluster.h"
#include "core/loaders/loaders.h"
#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class Datafile

Qt::CheckState Datafile::activated() const
{
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

//  ***********************************************************************************************
//! @class HighlightedData

void HighlightedData::clear()
{
    current_ = nullptr;
}

//! temporarily clear, don't emit signal
void HighlightedData::unset()
{
    current_ = nullptr;
}

void HighlightedData::setFile(int i)
{
    if (i<0)
        return clear();
    ASSERT(i<gSession->dataset().countFiles());
    setCluster(gSession->dataset().fileAt(i).clusters_[0]->index());
    ASSERT(i==current_->file().index_);
}

void HighlightedData::setCluster(int i)
{
    if (i<0)
        return clear();
    ASSERT(i<gSession->dataset().countClusters());
    current_ = &gSession->dataset().clusterAt(i);
}

void HighlightedData::reset()
{
    if (!gSession->dataset().countClusters())
        return clear();
    setCluster(0);
}

const Datafile* HighlightedData::file() const
{
    if (!current_)
        return nullptr;
    return &current_->file();
}

int HighlightedData::fileIndex() const
{
    return current_ ? file()->index_ : -1;
}

int HighlightedData::clusterIndex() const
{
    return current_ ? current_->index() : -1;
}


//  ***********************************************************************************************
//! @class Dataset

Dataset::Dataset() {
    binning.       setPostHook( [this](int)  { onClusteringChanged(); } );
    dropIncomplete.setPostHook( [this](bool) { onClusteringChanged(); } );
}

void Dataset::clear()
{
    qDebug() << "Dataset::clear";
    highlight().clear();
    files_.clear();
    onFileChanged();
    gSession->updateImageSize();
    gSession->imageCut().clear();
    qDebug() << "Dataset::clear/";
}

void Dataset::removeFile()
{
    int i = highlight().fileIndex();
    highlight().unset(); // to avoid conflicts; will be reset below
    files_.erase(files_.begin()+i);
    onFileChanged();
    gSession->updateImageSize();
    if (files_.empty())
        gSession->imageCut().clear();
    if (countFiles()) {
        // reset highlight, which was temporarily unset at the beginning of this function
        if (i<countFiles())
            highlight().setFile(i);
        else if (i>0)
            highlight().setFile(i-1);
        else
            qFatal("impossible case in Dataset::removeFile");
    } else
        highlight().clear(); // TODO or directly emit signal ?
    remakeAll("Dataset::removeFile");
}

void Dataset::addGivenFiles(const QStringList& filePaths)
{
    int i = highlight().fileIndex();
    highlight().unset(); // to avoid conflicts; will be reset below
    for (const QString& path: filePaths) {
        if (path.isEmpty() || hasFile(path))
            continue;
        Rawfile rawfile { load::loadRawfile(path) };
        gSession->setImageSize(rawfile.imageSize());
        files_.push_back(Datafile {std::move(rawfile)});
        onFileChanged();
    }
    if (countFiles())
        highlight().setFile( i<0 ? 0 : i );
}

void Dataset::activateCluster(int index, bool on)
{
    allClusters_.at(index)->setActivated(on);
    updateActiveClusters();
}

void Dataset::setFileActivation(int index, bool on)
{
    const Datafile& fil = fileAt(index);
    for (Cluster* cluster : fil.clusters_)
        cluster->setActivated(on);
    updateActiveClusters();
}

void Dataset::onFileChanged()
{
    int idx = 0;
    int cnt = 0;
    for (Datafile& file: files_) {
        file.index_ = idx++;
        file.offset_ = cnt;
        cnt += file.numMeasurements();
    }
    updateClusters();
    remakeAll("Dataset::onFileChanged");
}

void Dataset::onClusteringChanged()
{
    updateClusters();
    highlight().reset();
    remakeAll("Dataset::onClusteringChanged");
}

void Dataset::updateClusters()
{
    allClusters_.clear();
    hasIncomplete_ = false;
    for (Datafile& file : files_) {
        file.clusters_.clear();
        for (int i=0; i<file.numMeasurements(); i+=binning.val()) {
            if (i+binning.val()>file.numMeasurements()) {
                hasIncomplete_ = true;
                if (dropIncomplete.val())
                    break;
            }
            std::vector<const Measurement*> group;
            for (int ii=i; ii<file.numMeasurements() && ii<i+binning.val(); ii++)
                group.push_back(file.raw_.measurements().at(ii));
            std::unique_ptr<Cluster> cluster(new Cluster(group, file, allClusters_.size(), i));
            file.clusters_.push_back(cluster.get());
            allClusters_.push_back(std::move(cluster));
        }
    }
    updateActiveClusters();
}

void Dataset::updateActiveClusters()
{
    activeClusters_ = {};
    for (const auto& cluster : allClusters_) {
        if (cluster->isActivated())
            activeClusters_.appendHere(cluster.get());
    }
}

const Datafile& Dataset::fileAt(int i) const
{
    ASSERT(0<=i && i<countFiles());
    return files_[i];
}

const Cluster& Dataset::clusterAt(int i) const
{
    ASSERT(0<=i && i<countClusters());
    return *allClusters_[i];
}

QJsonObject Dataset::toJson() const
{
    QJsonObject ret;
    QJsonArray arr;
    for (const Datafile& file : files_)
        arr.append(file.raw_.fileInfo().absoluteFilePath());
    ret.insert("files", arr);
    ret.insert("binning", binning.val());
    return ret;
}

void Dataset::fromJson(const JsonObj& obj)
{
    const QJsonArray& files = obj.loadArr("files");
    QStringList paths;
    for (const QJsonValue& file : files)
        paths.append(file.toString());
    addGivenFiles(paths);
    binning.setVal(obj.loadPint("binning", 1));
}

bool Dataset::hasFile(const QString& fileName) const
{
    QFileInfo fileInfo(fileName);
    for (const Datafile& file : files_)
        if (fileInfo == file.raw_.fileInfo())
            return true;
    return false;
}
